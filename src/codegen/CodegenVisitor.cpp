// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/CodegenVisitor.h"
#include "ast/ASTControlStatement.h"
#include "ast/ASTExpression.h"
#include "ast/ASTFunctionStatement.h"
#include "ast/ASTLiteralExpression.h"
#include "ast/ASTNode.h"
#include "ast/ASTOperatorExpression.h"
#include "ast/ASTStatement.h"
#include "ast/FwdDecl.h"
#include "codegen/ModuleFile.h"
#include "util/ProgramInfo.h"
#include "util/ProgramOptions.h"
#include "util/StringUtils.h"

#define USE_LLVM_MODULE_VERIFY 0

namespace codegen
{
CodegenVisitor::CodegenVisitor(llvm::LLVMContext& c, llvm::Module* m,
                               CodegenInfo i)
    : context{c}, module(m), info(i), builder(context), dbuilder(*m),
      dcu{dbuilder.createCompileUnit(
          // Let's pretend for a moment that we're C
          // That's of course very dumb,
          // since we're definitely going to be more popular than C
          llvm::dwarf::DW_LANG_C, info.file->getFilename(), ".",
          util::programinfo::getIdentifier(), info.optEnabled(), "", 1)},
      dfile{dbuilder.createFile(dcu->getFilename(), dcu->getDirectory())},
      symbols{std::make_unique<SymbolTable>()},
      types{std::make_unique<TypeTable>(module)}
{
    // Create types
    types->insertTypeWithVariants<VoidType>(context, dbuilder);
    types->insertTypeWithVariants<Int8Type>(context, dbuilder);
    types->insertTypeWithVariants<Int16Type>(context, dbuilder);
    types->insertTypeWithVariants<Int32Type>(context, dbuilder);
    types->insertTypeWithVariants<Int64Type>(context, dbuilder);
    types->insertTypeWithVariants<F32Type>(context, dbuilder);
    types->insertTypeWithVariants<F64Type>(context, dbuilder);
    types->insertTypeWithVariants<BoolType>(context, dbuilder);
    types->insertTypeWithVariants<CharType>(context, dbuilder);
    types->insertTypeWithVariants<ByteCharType>(context, dbuilder);
    types->insertTypeWithVariants<ByteType>(context, dbuilder);
    types->insertTypeWithVariants<StringType>(context, dbuilder);
    types->insertTypeWithVariants<CStringType>(context, dbuilder);
}

bool CodegenVisitor::codegen(ast::AST* ast)
{
    //module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 3);
    module->addModuleFlag(llvm::Module::Error, "Debug Info Version",
                          llvm::DEBUG_METADATA_VERSION);

    // Set source filename
    module->setSourceFileName(ast->file->getFilename());

    // Codegen all children
    auto root = ast->globalNode.get();
    symbols->addBlock();
    for(auto& child : root->nodes)
    {
        if(!child->accept(this))
        {
            return false;
        }
    }

    auto exports = symbols->findExports();
    writeExports(std::move(exports));

    // Only dump global symbols because all other symbols have been popped
    symbols->dump();

    symbols->removeTopBlock();

    stripInstructionsAfterTerminators();

    dbuilder.finalize();

#if USE_LLVM_MODULE_VERIFY
    // Buggy, don't use
    util::logger->trace("Verifying module");
    if(!llvm::verifyModule(*module))
    {
        util::logger->error("Module verification failed");
        util::loggerBasic->info("Module dump:");
        dumpModule();
        return false;
    }
    util::logger->trace("Module verification successful");
#endif

    return true;
}

void CodegenVisitor::emitDebugLocation(ast::ASTNode* node)
{
    if(info.emitDebug)
    {
        if(!node)
        {
            // We're on function prologue, unset the location
            // This way the debugger will skip it
            builder.SetCurrentDebugLocation(llvm::DebugLoc());
            return;
        }

        builder.SetCurrentDebugLocation(llvm::DebugLoc::get(
            node->loc.line, node->loc.col, getTopDebugScope()));
    }
}

void CodegenVisitor::writeExports(std::unique_ptr<SymbolTable> exports)
{
    if(util::viewProgramOptions().outputFilename == "stdout")
    {
        util::logger->info("Not writing module export file");
        return;
    }

    auto filename = getModuleFilename(module->getName());
    ModuleFile mod(filename);
    mod.write(ModuleFile::ModuleFileSymbolTable::createFromSymbolTable(
        std::move(exports)));
}

void CodegenVisitor::stripInstructionsAfterTerminators()
{
    // Iterate every function
    for(auto& func : *module)
    {
        // Iterate every BasicBlock
        for(auto& bb : func)
        {
            bool termFound = false; // Did we find a block terminator
            // Iterate every instruction
            for(auto inst = bb.begin(); inst != bb.end(); /* NO STEP */)
            {
                if(!termFound && inst->isTerminator())
                {
                    // Current instruction is a terminator,
                    // and no termination has been found
                    // previously in this block
                    termFound = true;
                    // Step to the next instruction and continue
                    // This will not skip one instruction like it
                    // normally would in for-loops, because the loop
                    // has no step expression
                    ++inst;
                    continue;
                }

                // Terminator has been found in this block
                if(termFound)
                {
                    // Remove the instruction
                    inst = inst->eraseFromParent();
                }
                else
                {
                    // Otherwise go to the next instruction
                    ++inst;
                }
            }

            // No terminator instruction has been found in this block
            // Insert an 'unreachable'-instruction
            // TODO: This might lead to unexpected bugs in generated code
            if(!termFound)
            {
                // Save current insert point
                auto insertBlock = builder.GetInsertBlock();
                auto insertPoint = builder.GetInsertPoint();

                // Add 'unreachable'
                builder.SetInsertPoint(&bb);
                builder.CreateUnreachable();

                // Roll back to the previous insert point
                builder.SetInsertPoint(insertBlock, insertPoint);
            }
        }
    }
}

FunctionSymbol* CodegenVisitor::findFunction(const std::string& name,
                                             ast::ASTNode* node, bool logError)
{
    // Find a symbol with a similar name
    auto f = symbols->find(name, Type::FUNCTION);
    if(f)
    {
        // Found one!
        // SymbolTable::find() returns a Symbol*, but we need a FunctionSymbol*.
        // Because we searched with Type::FUNCTION, it's guaranteed to actually
        // be a FunctionSymbol.
        // Therefore, a static_cast is safe
        auto func = static_cast<FunctionSymbol*>(f);
        assert(func->value->value);
        return func;
    }

    if(logError)
    {
        return codegenError(node, "Undefined function: '{}'", name);
    }
    return nullptr;
}

FunctionSymbol*
CodegenVisitor::declareFunction(FunctionType* type, const std::string& name,
                                ast::ASTFunctionPrototypeStatement* proto)
{
    // Check if function with the same name is already declared
    auto func = findFunction(name, nullptr, false);
    if(func)
    {
        // Check if the types match
        if(func->value->type == type)
        {
            // Declaration's already been done, no need to redo it.
            return func;
        }
        else
        {
            // Function signatures don't match!
            return codegenError(proto,
                                "Function declaration failed: Mismatching "
                                "prototypes for similarly named functions: "
                                "'{}' and '{}'",
                                func->value->type->getDecoratedName(),
                                type->getDecoratedName());
        }
    }

    // Codegen prototype
    auto accept = proto->accept(this);
    if(!accept)
    {
        return nullptr;
    }

    // Add symbol to current scope
    auto val = std::make_unique<TypedValue>(type, accept->value,
                                            TypedValue::STMTVALUE, false);
    auto var = std::make_unique<FunctionSymbol>(proto->loc, std::move(val),
                                                name, proto);
    var->isExport = proto->isExport;
    auto varptr = var.get();
    symbols->getTop().insert(std::make_pair(name, std::move(var)));
    return varptr;
}

llvm::AllocaInst*
CodegenVisitor::createEntryBlockAlloca(llvm::Function* func, llvm::Type* type,
                                       const std::string& name)
{
    emitDebugLocation(nullptr);
    llvm::IRBuilder<> tmp(&func->getEntryBlock(),
                          func->getEntryBlock().begin());
    return tmp.CreateAlloca(type, nullptr, name.c_str());
}

ast::ASTFunctionPrototypeStatement*
CodegenVisitor::getASTNodeFunction(ast::ASTNode* node) const
{
    // Use ASTNode::getFunction(), which was made for this
    auto f = node->getFunction();
    if(!f)
    {
        return nullptr;
    }
    return f->proto.get();
}

bool CodegenVisitor::importModule(ast::ASTImportStatement* import)
{
    auto moduleName = getModuleFilename(import->importee->value);
    ModuleFile mod(moduleName);
    auto tree = [&]() -> decltype(mod.read().toAST()) {
        try
        {
            auto t = mod.read();
            return t.toAST();
        }
        catch(const std::exception& e)
        {
            codegenError(import, "Invalid import: {}", e.what());
            codegenInfo(import, "Did you forget to compile the importee "
                                "module?");
            return nullptr;
        }
    }();
    if(!tree)
    {
        return false;
    }
    for(auto& node : tree->globalNode->nodes)
    {
        if(!node->accept(this))
        {
            return false;
        }
    }
    return true;
}

std::pair<Type*, std::unique_ptr<TypedValue>>
CodegenVisitor::inferVariableDefType(ast::ASTVariableDefinitionExpression* node)
{
    // Define some lambdas for easier returning
    auto ret = [](Type* t, std::unique_ptr<TypedValue> i) {
        return std::make_pair(t, std::move(i));
    };
    auto err = [](std::nullptr_t n = nullptr) { return std::make_pair(n, n); };

    // Initializer
    std::unique_ptr<TypedValue> init = nullptr;
    if(node->init->nodeType != ast::ASTNode::EMPTY_EXPR)
    {
        // Only codegen if there actually is an initializer
        init = node->init->accept(this);
        if(!init)
        {
            return err();
        }
    }

    // Infer type
    Type* type = nullptr;
    if(node->typeInferred)
    {
        type = types->find(init->type->getName());
    }
    else
    {
        type = types->find(node->type->value);
    }
    assert(type);

    // Type checks
    if(!type->isSized())
    {
        // It needs to be sized
        return err(codegenError(node->type.get(),
                                "Cannot create a variable of unsized type: {}",
                                type->getDecoratedName()));
    }
    if(types->isDefinedUndecorated(node->name->value) != 0)
    {
        // Cannot name variable as an already defined typename
        return err(codegenError(
            node->type.get(), "Cannot name variable as '{}': Reserved typename",
            node->name->value));
    }

    // If there's no initializer, zero-initialize
    if(!init)
    {
        init = type->zeroInit();
    }

    // Check init type
    if(!init->type->isSameOrImplicitlyCastable(node->init.get(), builder,
                                               init.get(), type))
    {
        return err(codegenError(
            node->init.get(), "Invalid init nodeession: Cannot assign {} to {}",
            init->type->getDecoratedName(), type->getDecoratedName()));
    }

    // Check for existing similarly named symbol
    if(symbols->find(node->name->value, nullptr, false))
    {
        return err(
            codegenError(node->name.get(),
                         "Cannot name variable as '{}', name already defined",
                         node->name->value));
    }

    return ret(type, std::move(init));
}

std::string CodegenVisitor::getModuleFilename() const
{
    return getModuleFilename(info.file->getFilename());
}
std::string
CodegenVisitor::getModuleFilename(const std::string& moduleName) const
{
    auto pathparts = util::stringutils::split(moduleName, '/');
    auto nameparts = util::stringutils::split(pathparts.back(), '.');
    if(nameparts.back() != "va")
    {
        nameparts.push_back("vamod");
    }
    else
    {
        nameparts.back() = "vamod";
    }
    return util::stringutils::join(nameparts, '.');
}

llvm::DIScope* CodegenVisitor::getTopDebugScope() const
{
    if(dblocks.empty())
    {
        return dcu;
    }
    return dblocks.back();
}
} // namespace codegen
