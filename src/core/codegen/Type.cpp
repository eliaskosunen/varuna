// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "core/codegen/Type.h"
#include "core/codegen/TypedValue.h"

namespace core
{
namespace codegen
{
    VoidType::VoidType(llvm::LLVMContext& c, llvm::DIBuilder&)
        : Type(VOID, c, llvm::Type::getVoidTy(c), nullptr, "void")
    {
    }

    Type::CastTuple VoidType::cast(Type::CastType c, const Type& to) const
    {
        if(c == IMPLICIT && to.kind == VOID)
        {
            return CastTuple{false, false, defaultCast};
        }

        return castError("Invalid cast: Cannot cast void");
    }

    std::unique_ptr<TypedValue>
    VoidType::operation(Type::OperatorType kind, core::lexer::TokenType op,
                        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on void");
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

    IntegralType::IntegralType(uint32_t w, Kind k, llvm::LLVMContext& c,
                               llvm::Type* t, llvm::DIType* d,
                               const std::string& n)
        : Type(k, c, t, d, n), width(w)
    {
    }

    Type::CastTuple IntegralType::cast(Type::CastType c, const Type& to) const
    {
        using llvm::Instruction;

        if(c == IMPLICIT)
        {
            if(to.kind == kind)
            {
                return CastTuple{false, false, defaultCast};
            }
            return castError("Invalid implicit cast: Cannot convert from "
                             "{} to {} implicitly",
                             name, to.name);
        }

        switch(to.kind)
        {
        case INT:
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case BOOL:
        {
            const auto& castedto = dynamic_cast<const IntegralType&>(to);
            if(castedto.width == width)
            {
                return CastTuple{false, false, defaultCast};
            }
            if(castedto.width > width)
            {
                return CastTuple{false, true, Instruction::SExt};
            }
            return CastTuple{false, true, Instruction::Trunc};
        }
        case FLOAT:
        case F32:
        case F64:
            return CastTuple{false, true, Instruction::SIToFP};
        case VOID:
        case BYTE:
        case CHAR:
        case STRING:
        case FUNCTION:
            if(c == BITCAST)
            {
                return CastTuple{false, true, Instruction::BitCast};
            }
            return castError("Invalid cast: Cannot convert from {} to {}", name,
                             to.name);
        }

        llvm_unreachable("Logic error in IntegralType::cast");
    }

    std::unique_ptr<TypedValue> IntegralType::operation(
        Type::OperatorType kind, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on {}", name);
    }

    bool IntegralType::isIntegral() const
    {
        return true;
    }
    bool IntegralType::isFloatingPoint() const
    {
        return false;
    }

    CharType::CharType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
        : Type(CHAR, c, llvm::Type::getInt8Ty(c),
               dbuilder.createBasicType("char", 8, 8,
                                        llvm::dwarf::DW_ATE_unsigned_char),
               "char")
    {
    }

    Type::CastTuple CharType::cast(Type::CastType c, const Type& to) const
    {
        using llvm::Instruction;

        if(c == IMPLICIT)
        {
            if(to.kind == kind)
            {
                return CastTuple{false, false, defaultCast};
            }

            return castError("Invalid implicit cast: Cannot convert from "
                             "{} to {} implicitly",
                             name, to.name);
        }

        switch(to.kind)
        {
        case INT8:
            return CastTuple{false, false, defaultCast};
        case BOOL:
            return CastTuple{false, true, Instruction::ZExt};
        case INT:
        case INT16:
        case INT32:
        case INT64:
            return CastTuple{false, true, Instruction::Trunc};
        case VOID:
        case FLOAT:
        case F32:
        case F64:
        case BYTE:
        case STRING:
        case CHAR:
        case FUNCTION:
            if(c == BITCAST)
            {
                return CastTuple{false, true, Instruction::BitCast};
            }
            return castError("Invalid cast: Cannot convert from {} to {}", name,
                             to.name);
        }

        llvm_unreachable("Logic error in CharType::cast");
    }

    std::unique_ptr<TypedValue>
    CharType::operation(Type::OperatorType kind, core::lexer::TokenType op,
                        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on {}", name);
    }

    bool CharType::isIntegral() const
    {
        return false;
    }
    bool CharType::isFloatingPoint() const
    {
        return false;
    }

    ByteType::ByteType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
        : Type(BYTE, c, llvm::Type::getInt32Ty(c),
               dbuilder.createBasicType("byte", 32, 32,
                                        llvm::dwarf::DW_ATE_unsigned),
               "byte")
    {
    }

    Type::CastTuple ByteType::cast(Type::CastType c, const Type& to) const
    {
        using llvm::Instruction;

        if(c == IMPLICIT)
        {
            if(to.kind == kind)
            {
                return CastTuple{false, false, defaultCast};
            }

            return castError("Invalid implicit cast: Cannot convert from "
                             "{} to {} implicitly",
                             name, to.name);
        }

        switch(to.kind)
        {
        case INT8:
        case INT16:
        case BOOL:
            return CastTuple{false, true, Instruction::ZExt};
        case INT:
        case INT32:
            return CastTuple{false, false, defaultCast};
        case INT64:
            return CastTuple{false, true, Instruction::Trunc};
        case VOID:
        case FLOAT:
        case F32:
        case F64:
        case CHAR:
        case STRING:
        case BYTE:
        case FUNCTION:
            if(c == BITCAST)
            {
                return CastTuple{false, true, Instruction::BitCast};
            }
            return castError("Invalid cast: Cannot convert from {} to {}", name,
                             to.name);
        }

        llvm_unreachable("Logic error in ByteType::cast");
    }

    std::unique_ptr<TypedValue>
    ByteType::operation(Type::OperatorType kind, core::lexer::TokenType op,
                        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on {}", name);
    }

    bool ByteType::isIntegral() const
    {
        return false;
    }
    bool ByteType::isFloatingPoint() const
    {
        return false;
    }

    FPType::FPType(uint32_t w, Kind k, llvm::LLVMContext& c, llvm::Type* t,
                   llvm::DIType* d, const std::string& n)
        : Type(k, c, t, d, n), width(w)
    {
    }

    Type::CastTuple FPType::cast(Type::CastType c, const Type& to) const
    {
        using llvm::Instruction;

        if(c == IMPLICIT)
        {
            if(to.kind == kind)
            {
                return CastTuple{false, false, defaultCast};
            }

            return castError("Invalid implicit cast: Cannot convert from "
                             "{} to {} implicitly",
                             name, to.name);
        }

        switch(to.kind)
        {
        case FLOAT:
        case F32:
        case F64:
        {
            const auto& castedto = dynamic_cast<const FPType&>(to);
            if(castedto.width == width)
            {
                return CastTuple{false, false, defaultCast};
            }
            if(castedto.width > width)
            {
                return CastTuple{false, true, Instruction::FPExt};
            }
            return CastTuple{false, true, Instruction::FPTrunc};
        }
        case INT:
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case BOOL:
            return CastTuple{false, true, Instruction::FPToSI};
        case VOID:
        case BYTE:
        case CHAR:
        case STRING:
        case FUNCTION:
            if(c == BITCAST)
            {
                return CastTuple{false, true, Instruction::BitCast};
            }
            return castError("Invalid cast: Cannot convert from {} to {}", name,
                             to.name);
        }

        llvm_unreachable("Logic error in FPType::cast");
    }

    std::unique_ptr<TypedValue>
    FPType::operation(Type::OperatorType kind, core::lexer::TokenType op,
                      std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on {}", name);
    }

    bool FPType::isIntegral() const
    {
        return false;
    }
    bool FPType::isFloatingPoint() const
    {
        return true;
    }

    StringType::StringType(llvm::LLVMContext& c, llvm::DIBuilder&)
        : Type(STRING, c,
               /*llvm::StructType::create(c, {
                   llvm::Type::getInt64Ty(c),
                   llvm::Type::getInt8PtrTy(c)
               }, "string_t", true),*/
               llvm::Type::getInt8PtrTy(c), nullptr, "string_t")
    {
    }

    Type::CastTuple StringType::cast(Type::CastType c, const Type& to) const
    {
        using llvm::Instruction;

        if(c == IMPLICIT)
        {
            if(to.kind == kind)
            {
                return CastTuple{false, false, defaultCast};
            }

            return castError("Invalid implicit cast: Cannot convert from "
                             "{} to {} implicitly",
                             name, to.name);
        }

        return castError("Invalid cast: {} cannot be casted", name);
    }

    std::unique_ptr<TypedValue> StringType::operation(
        Type::OperatorType kind, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on {}", name);
    }

    bool StringType::isIntegral() const
    {
        return false;
    }
    bool StringType::isFloatingPoint() const
    {
        return false;
    }

    CallableType::CallableType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder,
                               Type* pReturnType, std::vector<Type*> pParams)
        : Type(FUNCTION, c, getLLVMFunctionType(pReturnType, pParams), nullptr,
               functionTypeToString(pReturnType, pParams)),
          returnType(pReturnType), params(std::move(pParams))
    {
    }

    bool CallableType::isIntegral() const
    {
        return false;
    }
    bool CallableType::isFloatingPoint() const
    {
        return false;
    }

    llvm::FunctionType*
    CallableType::getLLVMFunctionType(Type* returnType,
                                      const std::vector<Type*>& params)
    {
        if(!llvm::FunctionType::isValidReturnType(returnType->type))
        {
            throw std::invalid_argument(fmt::format(
                "Invalid return type for a function: {}", returnType->name));
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
                    "Invalid argument type for a function: {}", p->name));
            }
            paramList.push_back(p->type);
        }

        return llvm::FunctionType::get(returnType->type, paramList, false);
    }

    std::string
    CallableType::paramVectorToString(const std::vector<Type*>& params,
                                      bool addBeginningComma)
    {
        std::string ret;
        for(const auto& p : params)
        {
            if(!ret.empty() || addBeginningComma)
            {
                ret.push_back(',');
            }
            ret.append(p->name);
        }
        return ret;
    }

    std::string
    CallableType::functionTypeToString(Type* returnType,
                                       const std::vector<Type*>& params)
    {
        return fmt::format("{}({})", returnType->name,
                           paramVectorToString(params, false));
    }

    FunctionType::FunctionType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder,
                               Type* pReturnType, std::vector<Type*> pParams)
        : CallableType(c, dbuilder, pReturnType, std::move(pParams))
    {
    }

    std::unique_ptr<TypedValue> FunctionType::operation(
        Type::OperatorType kind, core::lexer::TokenType op,
        std::vector<std::unique_ptr<TypedValue>> operands) const
    {
        return operationError(
            "Invalid operation: Cannot make any operations on {}", name);
    }

    Type::CastTuple FunctionType::cast(Type::CastType c, const Type& to) const
    {
        using llvm::Instruction;

        if(c == IMPLICIT)
        {
            if(to.kind == kind)
            {
                return CastTuple{false, false, defaultCast};
            }

            return castError("Invalid implicit cast: Cannot convert from "
                             "{} to {} implicitly",
                             name, to.name);
        }

        return castError("Invalid cast: {} cannot be casted", name);
    }
} // namespace codegen
} // namespace core
