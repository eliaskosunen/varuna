// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "ast/ASTFunctionStatement.h"
#include "util/Logger.h"
#include "util/OperatorType.h"
#include "util/ProgramOptions.h"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <tuple>

namespace codegen
{
struct TypedValue;
class TypeOperationBase;
class Type;
class TypeTable;

class Type
{
public:
    enum _Kind
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
        BCHAR,
        STRING,
        FUNCTION
    };
    enum CastType
    {
        IMPLICIT,
        CAST,
        BITCAST
    };

    using Kind = util::SafeEnum<_Kind>;

    Type(TypeTable* list, std::unique_ptr<TypeOperationBase> op, Kind k,
         llvm::LLVMContext& c, llvm::Type* t, llvm::DIType* d, std::string n);
    Type(const Type& t) = default;
    Type& operator=(const Type& t) = default;
    Type(Type&&) = default;
    Type& operator=(Type&&) = default;
    virtual ~Type() noexcept;

    virtual std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                             llvm::IRBuilder<>& builder,
                                             CastType c, TypedValue* val,
                                             Type* to) const = 0;

    bool isSameOrImplicitlyCastable(ast::ASTNode* node,
                                    llvm::IRBuilder<>& builder, TypedValue* val,
                                    Type* to) const;

    virtual bool isSized() const
    {
        return true;
    }
    virtual bool isIntegral() const = 0;
    virtual bool isFloatingPoint() const = 0;

    virtual std::unique_ptr<TypedValue> zeroInit() = 0;

    /// Get basic name of type
    std::string getName() const
    {
        return name;
    }
    /// Get complete name of type
    std::string getDecoratedName() const
    {
        return name;
    }

    /// Get type operations
    TypeOperationBase* getOperations() const;

    /// Are types completely equal
    /// e.g. Same mutability
    bool equal(const Type& t) const
    {
        return kind == t.kind && getDecoratedName() == t.getDecoratedName();
    }
    bool inequal(const Type& t) const
    {
        return equal(t);
    }
    bool equal(Type* t) const
    {
        return kind == t->kind && getDecoratedName() == t->getDecoratedName();
    }
    bool inequal(Type* t) const
    {
        return equal(t);
    }

    bool operator==(const Type& t) const
    {
        return equal(t);
    }
    bool operator!=(const Type& t) const
    {
        return inequal(t);
    }

    /// Are types equal
    /// e.g. Mutability can differ
    bool basicEqual(const Type& t) const
    {
        return kind == t.kind && getName() == t.getName();
    }
    bool basicInequal(const Type& t) const
    {
        return !basicEqual(t);
    }
    bool basicEqual(Type* t) const
    {
        return kind == t->kind && getName() == t->getName();
    }
    bool basicInequal(Type* t) const
    {
        return !basicEqual(t);
    }

    TypeTable* typeTable;
    llvm::LLVMContext& context;
    llvm::Type* type;
    llvm::DIType* dtype;
    Kind kind;

protected:
    std::unique_ptr<TypedValue> implicitCast(ast::ASTNode* node,
                                             llvm::IRBuilder<>& builder,
                                             TypedValue* val, Type* to) const;

    template <typename... Args>
    std::nullptr_t castError(ast::ASTNode* node, const std::string& format,
                             Args&&... args) const
    {
        return util::logCompilerError(node->loc, format,
                                      std::forward<Args>(args)...);
    }

    template <typename... Args>
    void castWarning(ast::ASTNode* node, const std::string& format,
                     Args&&... args) const
    {
        util::logCompilerWarning(node->loc, format,
                                 std::forward<Args>(args)...);
    }

    template <typename... Args>
    void castInfo(ast::ASTNode* node, const std::string& format,
                  Args&&... args) const
    {
        util::logCompilerInfo(node->loc, format, std::forward<Args>(args)...);
    }

private:
    std::string name;

    std::unique_ptr<TypeOperationBase> operations;
};

class VoidType : public Type
{
public:
    VoidType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder&);

    std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isSized() const override;
    bool isIntegral() const override;
    bool isFloatingPoint() const override;
};

class IntegralType : public Type
{
public:
    IntegralType(TypeTable* list, size_t w, Kind k, llvm::LLVMContext& c,
                 llvm::Type* t, llvm::DIType* d, const std::string& n);

    std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;

    size_t getIntSize() const noexcept
    {
        assert(util::viewProgramOptions().intSize % 8 == 0);
        assert(util::viewProgramOptions().intSize <= 64);
        assert(util::viewProgramOptions().intSize >= 8);
        return util::viewProgramOptions().intSize;
    }

    size_t width;
};

class IntType : public IntegralType
{
public:
    IntType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class Int8Type : public IntegralType
{
public:
    Int8Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class Int16Type : public IntegralType
{
public:
    Int16Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class Int32Type : public IntegralType
{
public:
    Int32Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class Int64Type : public IntegralType
{
public:
    Int64Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class BoolType : public Type
{
public:
    BoolType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
};

class CharacterType : public Type
{
public:
    CharacterType(TypeTable* list, size_t w, Kind k, llvm::LLVMContext& c,
                  llvm::Type* t, llvm::DIType* d, const std::string& n);

    std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;

    size_t width;
};

class CharType : public CharacterType
{
public:
    CharType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class ByteCharType : public CharacterType
{
public:
    ByteCharType(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder);
};

class ByteType : public Type
{
public:
    ByteType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
};

class FPType : public Type
{
public:
    FPType(TypeTable* list, size_t w, Kind k, llvm::LLVMContext& c,
           llvm::Type* t, llvm::DIType* d, const std::string& n);

    std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;

    size_t width;
};

class FloatType : public FPType
{
public:
    FloatType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class F32Type : public FPType
{
public:
    F32Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class F64Type : public FPType
{
public:
    F64Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);
};

class StringType : public Type
{
public:
    StringType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder&);

    std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
};

class FunctionType : public Type
{
public:
    FunctionType(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder, Type* pReturnType,
                 std::vector<Type*> pParams);

    static llvm::FunctionType*
    getLLVMFunctionType(Type* returnType, const std::vector<Type*>& params);
    static std::string paramVectorToString(const std::vector<Type*>& params,
                                           bool addBeginningComma = false);
    static std::string functionTypeToString(Type* returnType,
                                            const std::vector<Type*>& params);

    static llvm::DISubroutineType*
    createDebugFunctionType(llvm::DIBuilder* dbuilder, Type* returnType,
                            const std::vector<Type*>& params,
                            llvm::DIFile* file);

    std::unique_ptr<TypedValue> cast(ast::ASTNode* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;

    Type* returnType;
    std::vector<Type*> params;
};
} // namespace codegen
