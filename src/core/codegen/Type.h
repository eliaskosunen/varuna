// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/Logger.h"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <tuple>

namespace core
{
namespace codegen
{
    class Type
    {
    public:
        enum Kind
        {
            VOID,
            INT,
            INT8,
            INT16,
            INT32,
            INT64,
            BOOL,
            FLOAT,
            F32,
            F64,
            BYTE,
            CHAR,
            STRING
        };
        enum CastType
        {
            IMPLICIT,
            CAST,
            BITCAST
        };

        /*
        tuple<bool, bool, CastOps>
        isError
        castNeeded
        castOperation
         */
        using CastTuple = std::tuple<bool, bool, llvm::Instruction::CastOps>;
        const decltype(llvm::Instruction::BitCast) defaultCast =
            llvm::Instruction::BitCast;

        Type(Kind k, llvm::LLVMContext& c, llvm::Type* t, llvm::DIType* d,
             std::string n)
            : context(c), type(t), dtype(d), name(std::move(n)), kind(k)
        {
        }
        Type(const Type& t) = default;
        Type& operator=(const Type& t) = default;
        Type(Type&&) noexcept = default;
        Type& operator=(Type&&) noexcept = default;
        virtual ~Type() noexcept = default;

        template <typename... Args>
        CastTuple typeError(const std::string& format, Args... args) const
        {
            util::logger->error(format.c_str(), args...);
            return CastTuple{true, false, defaultCast};
        }

        virtual CastTuple cast(CastType c, const Type& to) = 0;

        virtual bool isSized()
        {
            return true;
        }
        virtual bool isIntegral() = 0;
        virtual bool isFloatingPoint() = 0;

        bool operator==(const Type& t) const
        {
            return kind == t.kind && name == t.name;
        }
        bool operator!=(const Type& t) const
        {
            return !(*this == t);
        }

        llvm::LLVMContext& context;
        llvm::Type* type;
        llvm::DIType* dtype;
        std::string name;
        Kind kind;
    };

    class VoidType : public Type
    {
    public:
        VoidType(llvm::LLVMContext& c, llvm::DIBuilder&)
            : Type(VOID, c, llvm::Type::getVoidTy(c), nullptr, "void")
        {
        }

        CastTuple cast(CastType c, const Type& to) override
        {
            if(c == IMPLICIT && to.kind == VOID)
            {
                return CastTuple{false, false, defaultCast};
            }

            return typeError("Invalid cast: Cannot cast void");
        }

        bool isSized() override
        {
            return false;
        }
        bool isIntegral() override
        {
            return false;
        }
        bool isFloatingPoint() override
        {
            return false;
        }
    };

    class IntegralType : public Type
    {
    public:
        IntegralType(uint32_t w, Kind k, llvm::LLVMContext& c, llvm::Type* t,
                     llvm::DIType* d, const std::string& n)
            : Type(k, c, t, d, n), width(w)
        {
        }

        CastTuple cast(CastType c, const Type& to) override
        {
            using llvm::Instruction;

            if(c == IMPLICIT)
            {
                if(to.kind == kind)
                {
                    return CastTuple{false, false, defaultCast};
                }
                return typeError("Invalid implicit cast: Cannot convert from "
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
                if(c == BITCAST)
                {
                    return CastTuple{false, true, Instruction::BitCast};
                }
                return typeError("Invalid cast: Cannot convert from {} to {}",
                                 name, to.name);
            }
        }

        bool isIntegral() override
        {
            return true;
        }
        bool isFloatingPoint() override
        {
            return false;
        }

        uint32_t width;
    };

    class IntType : public IntegralType
    {
    public:
        IntType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : IntegralType(32, INT, c, llvm::Type::getInt32Ty(c),
                           dbuilder.createBasicType("int", 32, 32,
                                                    llvm::dwarf::DW_ATE_signed),
                           "int")
        {
        }
    };

    class Int8Type : public IntegralType
    {
    public:
        Int8Type(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : IntegralType(8, INT8, c, llvm::Type::getInt8Ty(c),
                           dbuilder.createBasicType("int8", 8, 8,
                                                    llvm::dwarf::DW_ATE_signed),
                           "int8")
        {
        }
    };

    class Int16Type : public IntegralType
    {
    public:
        Int16Type(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : IntegralType(16, INT16, c, llvm::Type::getInt16Ty(c),
                           dbuilder.createBasicType("int16", 16, 16,
                                                    llvm::dwarf::DW_ATE_signed),
                           "int16")
        {
        }
    };

    class Int32Type : public IntegralType
    {
    public:
        Int32Type(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : IntegralType(32, INT32, c, llvm::Type::getInt32Ty(c),
                           dbuilder.createBasicType("int32", 32, 32,
                                                    llvm::dwarf::DW_ATE_signed),
                           "int32")
        {
        }
    };

    class Int64Type : public IntegralType
    {
    public:
        Int64Type(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : IntegralType(64, INT64, c, llvm::Type::getInt64Ty(c),
                           dbuilder.createBasicType("int64", 64, 64,
                                                    llvm::dwarf::DW_ATE_signed),
                           "int64")
        {
        }
    };

    class BoolType : public IntegralType
    {
    public:
        BoolType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : IntegralType(1, BOOL, c, llvm::Type::getInt1Ty(c),
                           dbuilder.createBasicType(
                               "bool", 1, 1, llvm::dwarf::DW_ATE_boolean),
                           "bool")
        {
        }
    };

    class CharType : public Type
    {
    public:
        CharType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : Type(CHAR, c, llvm::Type::getInt8Ty(c),
                   dbuilder.createBasicType("char", 8, 8,
                                            llvm::dwarf::DW_ATE_unsigned_char),
                   "char")
        {
        }

        CastTuple cast(CastType c, const Type& to) override
        {
            using llvm::Instruction;

            if(c == IMPLICIT)
            {
                if(to.kind == kind)
                {
                    return CastTuple{false, false, defaultCast};
                }

                return typeError("Invalid implicit cast: Cannot convert from "
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
                if(c == BITCAST)
                {
                    return CastTuple{false, true, Instruction::BitCast};
                }
                return typeError("Invalid cast: Cannot convert from {} to {}",
                                 name, to.name);
            }
        }

        bool isIntegral() override
        {
            return false;
        }
        bool isFloatingPoint() override
        {
            return false;
        }
    };

    class ByteType : public Type
    {
    public:
        ByteType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : Type(BYTE, c, llvm::Type::getInt32Ty(c),
                   dbuilder.createBasicType("byte", 32, 32,
                                            llvm::dwarf::DW_ATE_unsigned),
                   "byte")
        {
        }

        CastTuple cast(CastType c, const Type& to) override
        {
            using llvm::Instruction;

            if(c == IMPLICIT)
            {
                if(to.kind == kind)
                {
                    return CastTuple{false, false, defaultCast};
                }

                return typeError("Invalid implicit cast: Cannot convert from "
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
                if(c == BITCAST)
                {
                    return CastTuple{false, true, Instruction::BitCast};
                }
                return typeError("Invalid cast: Cannot convert from {} to {}",
                                 name, to.name);
            }
        }

        bool isIntegral() override
        {
            return false;
        }
        bool isFloatingPoint() override
        {
            return false;
        }
    };

    class FPType : public Type
    {
    public:
        FPType(uint32_t w, Kind k, llvm::LLVMContext& c, llvm::Type* t,
               llvm::DIType* d, const std::string& n)
            : Type(k, c, t, d, n), width(w)
        {
        }

        CastTuple cast(CastType c, const Type& to) override
        {
            using llvm::Instruction;

            if(c == IMPLICIT)
            {
                if(to.kind == kind)
                {
                    return CastTuple{false, false, defaultCast};
                }

                return typeError("Invalid implicit cast: Cannot convert from "
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
                if(c == BITCAST)
                {
                    return CastTuple{false, true, Instruction::BitCast};
                }
                return typeError("Invalid cast: Cannot convert from {} to {}",
                                 name, to.name);
            }
        }

        bool isIntegral() override
        {
            return false;
        }
        bool isFloatingPoint() override
        {
            return true;
        }

        uint32_t width;
    };

    class FloatType : public FPType
    {
    public:
        FloatType(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : FPType(32, FLOAT, c, llvm::Type::getFloatTy(c),
                     dbuilder.createBasicType("float", 32, 32,
                                              llvm::dwarf::DW_ATE_float),
                     "float")
        {
        }
    };

    class F32Type : public FPType
    {
    public:
        F32Type(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : FPType(32, F32, c, llvm::Type::getFloatTy(c),
                     dbuilder.createBasicType("f32", 32, 32,
                                              llvm::dwarf::DW_ATE_float),
                     "f32")
        {
        }
    };

    class F64Type : public FPType
    {
    public:
        F64Type(llvm::LLVMContext& c, llvm::DIBuilder& dbuilder)
            : FPType(64, F64, c, llvm::Type::getFloatTy(c),
                     dbuilder.createBasicType("f64", 64, 64,
                                              llvm::dwarf::DW_ATE_float),
                     "f64")
        {
        }
    };

    class StringType : public Type
    {
    public:
        StringType(llvm::LLVMContext& c, llvm::DIBuilder&)
            : Type(STRING, c,
                   /*llvm::StructType::create(c, {
                       llvm::Type::getInt64Ty(c),
                       llvm::Type::getInt8PtrTy(c)
                   }, "string_t", true),*/
                   llvm::Type::getInt8PtrTy(c), nullptr, "string_t")
        {
        }

        CastTuple cast(CastType c, const Type& to) override
        {
            using llvm::Instruction;

            if(c == IMPLICIT)
            {
                if(to.kind == kind)
                {
                    return CastTuple{false, false, defaultCast};
                }

                return typeError("Invalid implicit cast: Cannot convert from "
                                 "{} to {} implicitly",
                                 name, to.name);
            }

            return typeError("Invalid cast: {} cannot be casted", name);
        }

        bool isIntegral() override
        {
            return false;
        }
        bool isFloatingPoint() override
        {
            return false;
        }
    };
} // namespace codegen
} // namespace core
