// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/Type.h"
#include "codegen/TypeOperation.h"
#include "codegen/TypeTable.h"
#include "codegen/TypedValue.h"

namespace codegen
{
Type::Type(TypeTable* list, std::unique_ptr<TypeOperationBase> op, Type::Kind k,
           llvm::LLVMContext& c, llvm::Type* t, llvm::DIType* d, std::string n)
    : typeTable(list), context(c), type(t), dtype(d), kind(k),
      name(std::move(n)), operations(std::move(op))
{
}

Type::~Type() noexcept = default;

bool Type::isSameOrImplicitlyCastable(ast::ASTNode* node,
                                      llvm::IRBuilder<>& builder,
                                      TypedValue* val, Type* to) const
{
    if(basicEqual(to))
    {
        return true;
    }

    auto result = cast(node, builder, IMPLICIT, val, to);
    return result != nullptr;
}

TypeOperationBase* Type::getOperations() const
{
    return operations.get();
}

VoidType::VoidType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder&)
    : Type(list, std::make_unique<VoidTypeOperation>(this), VOID, c,
           llvm::Type::getVoidTy(c), nullptr, "void")
{
}

bool VoidType::isSized() const
{
    return false;
}
bool VoidType::isIntegral() const
{
    return false;
}
bool VoidType::isFloatingPoint() const
{
    return false;
}

std::unique_ptr<TypedValue> VoidType::zeroInit()
{
    return nullptr;
}

IntegralType::IntegralType(TypeTable* list, size_t w, Kind k,
                           llvm::LLVMContext& c, llvm::Type* t, llvm::DIType* d,
                           const std::string& n)
    : Type(list, std::make_unique<IntegralTypeOperation>(this), k, c, t, d, n),
      width(w)
{
}

bool IntegralType::isIntegral() const
{
    return true;
}
bool IntegralType::isFloatingPoint() const
{
    return false;
}

std::unique_ptr<TypedValue> IntegralType::zeroInit()
{
    auto val = llvm::Constant::getNullValue(type);
    return std::make_unique<TypedValue>(this, val, TypedValue::RVALUE, true);
}

IntType::IntType(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder)
    : IntegralType(
          list, getIntSize(), INT, c,
          llvm::Type::getIntNTy(c, static_cast<unsigned int>(getIntSize())),
          dbuilder.createBasicType("int", getIntSize(), getIntSize(),
                                   llvm::dwarf::DW_ATE_signed),
          "int")
{
}

Int8Type::Int8Type(TypeTable* list, llvm::LLVMContext& c,
                   llvm::DIBuilder& dbuilder)
    : IntegralType(
          list, 8, INT8, c, llvm::Type::getInt8Ty(c),
          dbuilder.createBasicType("int8", 8, 8, llvm::dwarf::DW_ATE_signed),
          "int8")
{
}

Int16Type::Int16Type(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder)
    : IntegralType(
          list, 16, INT16, c, llvm::Type::getInt16Ty(c),
          dbuilder.createBasicType("int16", 16, 16, llvm::dwarf::DW_ATE_signed),
          "int16")
{
}

Int32Type::Int32Type(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder)
    : IntegralType(
          list, 32, INT32, c, llvm::Type::getInt32Ty(c),
          dbuilder.createBasicType("int32", 32, 32, llvm::dwarf::DW_ATE_signed),
          "int32")
{
}

Int64Type::Int64Type(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder)
    : IntegralType(
          list, 64, INT64, c, llvm::Type::getInt64Ty(c),
          dbuilder.createBasicType("int64", 64, 64, llvm::dwarf::DW_ATE_signed),
          "int64")
{
}

BoolType::BoolType(TypeTable* list, llvm::LLVMContext& c,
                   llvm::DIBuilder& dbuilder)
    : Type(list, std::make_unique<BoolTypeOperation>(this), BOOL, c,
           llvm::Type::getInt1Ty(c),
           dbuilder.createBasicType("bool", 1, 1, llvm::dwarf::DW_ATE_boolean),
           "bool")
{
}

bool BoolType::isIntegral() const
{
    return false;
}
bool BoolType::isFloatingPoint() const
{
    return false;
}

std::unique_ptr<TypedValue> BoolType::zeroInit()
{
    auto val = llvm::Constant::getNullValue(type);
    return std::make_unique<TypedValue>(this, val, TypedValue::RVALUE, true);
}

CharacterType::CharacterType(TypeTable* list, size_t w, Kind k,
                             llvm::LLVMContext& c, llvm::Type* t,
                             llvm::DIType* d, const std::string& n)
    : Type(list, std::make_unique<CharacterTypeOperation>(this), k, c, t, d, n),
      width(w)
{
}

bool CharacterType::isIntegral() const
{
    return false;
}
bool CharacterType::isFloatingPoint() const
{
    return false;
}

std::unique_ptr<TypedValue> CharacterType::zeroInit()
{
    auto val = llvm::Constant::getNullValue(type);
    return std::make_unique<TypedValue>(this, val, TypedValue::RVALUE, true);
}

CharType::CharType(TypeTable* list, llvm::LLVMContext& c,
                   llvm::DIBuilder& dbuilder)
    : CharacterType(list, 32, CHAR, c, llvm::Type::getInt32Ty(c),
                    dbuilder.createBasicType("char", 32, 32,
                                             llvm::dwarf::DW_ATE_unsigned_char),
                    "char")
{
}

ByteCharType::ByteCharType(TypeTable* list, llvm::LLVMContext& c,
                           llvm::DIBuilder& dbuilder)
    : CharacterType(list, 8, BCHAR, c, llvm::Type::getInt8Ty(c),
                    dbuilder.createBasicType("bchar", 8, 8,
                                             llvm::dwarf::DW_ATE_unsigned_char),
                    "bchar")
{
}

ByteType::ByteType(TypeTable* list, llvm::LLVMContext& c,
                   llvm::DIBuilder& dbuilder)
    : Type(list, std::make_unique<ByteTypeOperation>(this), BYTE, c,
           llvm::Type::getInt8Ty(c),
           dbuilder.createBasicType("byte", 8, 8, llvm::dwarf::DW_ATE_unsigned),
           "byte")
{
}

bool ByteType::isIntegral() const
{
    return false;
}
bool ByteType::isFloatingPoint() const
{
    return false;
}

std::unique_ptr<TypedValue> ByteType::zeroInit()
{
    auto val = llvm::Constant::getNullValue(type);
    return std::make_unique<TypedValue>(this, val, TypedValue::RVALUE, true);
}

FPType::FPType(TypeTable* list, size_t w, Kind k, llvm::LLVMContext& c,
               llvm::Type* t, llvm::DIType* d, const std::string& n)
    : Type(list, std::make_unique<FPTypeOperation>(this), k, c, t, d, n),
      width(w)
{
}

bool FPType::isIntegral() const
{
    return false;
}
bool FPType::isFloatingPoint() const
{
    return true;
}

std::unique_ptr<TypedValue> FPType::zeroInit()
{
    auto val = llvm::Constant::getNullValue(type);
    return std::make_unique<TypedValue>(this, val, TypedValue::RVALUE, true);
}

FloatType::FloatType(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder)
    : FPType(
          list, 32, FLOAT, c, llvm::Type::getFloatTy(c),
          dbuilder.createBasicType("float", 32, 32, llvm::dwarf::DW_ATE_float),
          "float")
{
}

F32Type::F32Type(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder)
    : FPType(list, 32, F32, c, llvm::Type::getFloatTy(c),
             dbuilder.createBasicType("f32", 32, 32, llvm::dwarf::DW_ATE_float),
             "f32")
{
}

F64Type::F64Type(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder)
    : FPType(list, 64, F64, c, llvm::Type::getFloatTy(c),
             dbuilder.createBasicType("f64", 64, 64, llvm::dwarf::DW_ATE_float),
             "f64")
{
}

StringType::StringType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder&)
    : Type(list, std::make_unique<StringTypeOperation>(this), STRING, c,
           llvm::StructType::create(
               c, {llvm::Type::getInt64Ty(c), llvm::Type::getInt8PtrTy(c)},
               "string_t", true),
           nullptr, "string_t")
{
}

bool StringType::isIntegral() const
{
    return false;
}
bool StringType::isFloatingPoint() const
{
    return false;
}

std::unique_ptr<TypedValue> StringType::zeroInit()
{
    return nullptr;
}

FunctionType::FunctionType(TypeTable* list, llvm::LLVMContext& c,
                           llvm::DIBuilder& dbuilder, Type* pReturnType,
                           std::vector<Type*> pParams)
    : Type(list, std::make_unique<FunctionTypeOperation>(this), FUNCTION, c,
           getLLVMFunctionType(pReturnType, pParams), nullptr,
           functionTypeToString(pReturnType, pParams)),
      returnType(pReturnType), params(std::move(pParams))
{
}

bool FunctionType::isIntegral() const
{
    return false;
}
bool FunctionType::isFloatingPoint() const
{
    return false;
}

std::unique_ptr<TypedValue> FunctionType::zeroInit()
{
    return nullptr;
}

llvm::FunctionType*
FunctionType::getLLVMFunctionType(Type* returnType,
                                  const std::vector<Type*>& params)
{
    if(!llvm::FunctionType::isValidReturnType(returnType->type))
    {
        throw std::invalid_argument(fmt::format(
            "Invalid return type for a function: {}", returnType->getName()));
    }
    if(params.empty())
    {
        return llvm::FunctionType::get(returnType->type, false);
    }

    std::vector<llvm::Type*> paramList;
    for(const auto& p : params)
    {
        if(!llvm::FunctionType::isValidArgumentType(p->type))
        {
            throw std::invalid_argument(fmt::format(
                "Invalid argument type for a function: {}", p->getName()));
        }
        paramList.push_back(p->type);
    }

    return llvm::FunctionType::get(returnType->type, paramList, false);
}

std::string FunctionType::paramVectorToString(const std::vector<Type*>& params,
                                              bool addBeginningComma)
{
    std::string ret;
    for(const auto& p : params)
    {
        if(!ret.empty() || addBeginningComma)
        {
            ret.push_back(',');
        }
        ret.append(p->getDecoratedName());
    }
    return ret;
}

std::string FunctionType::functionTypeToString(Type* returnType,
                                               const std::vector<Type*>& params)
{
    return fmt::format("({}) -> {}", paramVectorToString(params, false),
                       returnType->getDecoratedName());
}

llvm::DISubroutineType* FunctionType::createDebugFunctionType(
    llvm::DIBuilder* dbuilder, Type* returnType,
    const std::vector<Type*>& params, llvm::DIFile* file)
{
    llvm::SmallVector<llvm::Metadata*, 8> elementTypes;
    elementTypes.push_back(returnType->dtype);

    for(const auto& p : params)
    {
        elementTypes.push_back(p->dtype);
    }

    return dbuilder->createSubroutineType(
        dbuilder->getOrCreateTypeArray(elementTypes));
}

} // namespace codegen
