// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/codegen/Type.h"
#include "core/codegen/TypeOperation.h"
#include "core/codegen/TypeTable.h"
#include "core/codegen/TypedValue.h"

namespace core
{
namespace codegen
{
    Type::Type(TypeTable* list, std::unique_ptr<TypeOperationBase> op,
               Type::Kind k, llvm::LLVMContext& c, llvm::Type* t,
               llvm::DIType* d, std::string n, bool mut)
        : typeTable(list), context(c), type(t), dtype(d), kind(k),
          isMutable(mut), name(std::move(n)), operations(std::move(op))
    {
    }

    Type::~Type() noexcept = default;

    bool Type::isSameOrImplicitlyCastable(llvm::IRBuilder<>& builder,
                                          llvm::Value* val, Type* to) const
    {
        if(basicEqual(to))
        {
            return true;
        }

        auto result = cast(builder, IMPLICIT, val, to);
        return result != nullptr;
    }

    TypeOperationBase* Type::getOperations() const
    {
        return operations.get();
    }

    VoidType::VoidType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder&,
                       bool mut)
        : Type(list, std::make_unique<VoidTypeOperation>(this), VOID, c,
               llvm::Type::getVoidTy(c), nullptr, "void", mut)
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

    IntegralType::IntegralType(TypeTable* list, uint32_t w, Kind k,
                               llvm::LLVMContext& c, llvm::Type* t,
                               llvm::DIType* d, const std::string& n, bool mut)
        : Type(list, std::make_unique<IntegralTypeOperation>(this), k, c, t, d,
               n, mut),
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

    IntType::IntType(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder, bool mut)
        : IntegralType(list, 32, INT, c, llvm::Type::getInt32Ty(c),
                       dbuilder.createBasicType("int", 32, 32,
                                                llvm::dwarf::DW_ATE_signed),
                       "int", mut)
    {
    }

    Int8Type::Int8Type(TypeTable* list, llvm::LLVMContext& c,
                       llvm::DIBuilder& dbuilder, bool mut)
        : IntegralType(list, 8, INT8, c, llvm::Type::getInt8Ty(c),
                       dbuilder.createBasicType("int8", 8, 8,
                                                llvm::dwarf::DW_ATE_signed),
                       "int8", mut)
    {
    }

    Int16Type::Int16Type(TypeTable* list, llvm::LLVMContext& c,
                         llvm::DIBuilder& dbuilder, bool mut)
        : IntegralType(list, 16, INT16, c, llvm::Type::getInt16Ty(c),
                       dbuilder.createBasicType("int16", 16, 16,
                                                llvm::dwarf::DW_ATE_signed),
                       "int16", mut)
    {
    }

    Int32Type::Int32Type(TypeTable* list, llvm::LLVMContext& c,
                         llvm::DIBuilder& dbuilder, bool mut)
        : IntegralType(list, 32, INT32, c, llvm::Type::getInt32Ty(c),
                       dbuilder.createBasicType("int32", 32, 32,
                                                llvm::dwarf::DW_ATE_signed),
                       "int32", mut)
    {
    }

    Int64Type::Int64Type(TypeTable* list, llvm::LLVMContext& c,
                         llvm::DIBuilder& dbuilder, bool mut)
        : IntegralType(list, 64, INT64, c, llvm::Type::getInt64Ty(c),
                       dbuilder.createBasicType("int64", 64, 64,
                                                llvm::dwarf::DW_ATE_signed),
                       "int64", mut)
    {
    }

    BoolType::BoolType(TypeTable* list, llvm::LLVMContext& c,
                       llvm::DIBuilder& dbuilder, bool mut)
        : Type(list, std::make_unique<BoolTypeOperation>(this), BOOL, c,
               llvm::Type::getInt1Ty(c),
               dbuilder.createBasicType("bool", 1, 1,
                                        llvm::dwarf::DW_ATE_boolean),
               "bool", mut)
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

    CharType::CharType(TypeTable* list, llvm::LLVMContext& c,
                       llvm::DIBuilder& dbuilder, bool mut)
        : Type(list, std::make_unique<CharTypeOperation>(this), CHAR, c,
               llvm::Type::getInt8Ty(c),
               dbuilder.createBasicType("char", 8, 8,
                                        llvm::dwarf::DW_ATE_unsigned_char),
               "char", mut)
    {
    }

    bool CharType::isIntegral() const
    {
        return false;
    }
    bool CharType::isFloatingPoint() const
    {
        return false;
    }

    ByteType::ByteType(TypeTable* list, llvm::LLVMContext& c,
                       llvm::DIBuilder& dbuilder, bool mut)
        : Type(list, std::make_unique<ByteTypeOperation>(this), BYTE, c,
               llvm::Type::getInt8Ty(c),
               dbuilder.createBasicType("byte", 8, 8,
                                        llvm::dwarf::DW_ATE_unsigned),
               "byte", mut)
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

    FPType::FPType(TypeTable* list, uint32_t w, Kind k, llvm::LLVMContext& c,
                   llvm::Type* t, llvm::DIType* d, const std::string& n,
                   bool mut)
        : Type(list, std::make_unique<FPTypeOperation>(this), k, c, t, d, n,
               mut),
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

    FloatType::FloatType(TypeTable* list, llvm::LLVMContext& c,
                         llvm::DIBuilder& dbuilder, bool mut)
        : FPType(list, 32, FLOAT, c, llvm::Type::getFloatTy(c),
                 dbuilder.createBasicType("float", 32, 32,
                                          llvm::dwarf::DW_ATE_float),
                 "float", mut)
    {
    }

    F32Type::F32Type(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder, bool mut)
        : FPType(list, 32, F32, c, llvm::Type::getFloatTy(c),
                 dbuilder.createBasicType("f32", 32, 32,
                                          llvm::dwarf::DW_ATE_float),
                 "f32", mut)
    {
    }

    F64Type::F64Type(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder, bool mut)
        : FPType(list, 64, F64, c, llvm::Type::getFloatTy(c),
                 dbuilder.createBasicType("f64", 64, 64,
                                          llvm::dwarf::DW_ATE_float),
                 "f64", mut)
    {
    }

    StringType::StringType(TypeTable* list, llvm::LLVMContext& c,
                           llvm::DIBuilder&, bool mut)
        : Type(list, std::make_unique<StringTypeOperation>(this), STRING, c,
               /*llvm::StructType::create(c, {
                   llvm::Type::getInt64Ty(c),
                   llvm::Type::getInt8PtrTy(c)
               }, "string_t", true),*/
               llvm::Type::getInt8PtrTy(c), nullptr, "string_t", mut)
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

    llvm::FunctionType*
    FunctionType::getLLVMFunctionType(Type* returnType,
                                      const std::vector<Type*>& params)
    {
        if(!llvm::FunctionType::isValidReturnType(returnType->type))
        {
            throw std::invalid_argument(
                fmt::format("Invalid return type for a function: {}",
                            returnType->getName()));
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

    std::string
    FunctionType::paramVectorToString(const std::vector<Type*>& params,
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

    std::string
    FunctionType::functionTypeToString(Type* returnType,
                                       const std::vector<Type*>& params)
    {
        return fmt::format("{}({})", returnType->getDecoratedName(),
                           paramVectorToString(params, false));
    }

} // namespace codegen
} // namespace core
