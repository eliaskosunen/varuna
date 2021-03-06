// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "ast/AST.h"
#include "ast/FunctionStmt.h"
#include "util/Compatibility.h"
#include "util/Logger.h"
#include "util/OperatorType.h"
#include "util/ProgramOptions.h"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <tuple>

#if VARUNA_MSVC
#undef VOID // defined in winnt.h as void
#endif

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
        INT8,
        INT16,
        INT32,
        INT64,
        BOOL,
        F32,
        F64,
        BYTE,
        CHAR,
        BCHAR,
        STRING,
        CSTRING,
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
    Type(const Type& t) = delete;
    Type& operator=(const Type& t) = delete;
    Type(Type&&) noexcept = delete;
    Type& operator=(Type&&) noexcept = delete;
    virtual ~Type() noexcept;

    /**
     * Cast a value to a type
     * \param  node    Node where the cast happens, used to create error
     *                 messages, set to `nullptr` to disable error reporting
     * \param  builder LLVM IRBuilder
     * \param  c       Cast type
     * \param  val     Value to cast
     * \param  to      Type to cast to
     * \return         Casted value, nullptr on failure
     */
    virtual std::unique_ptr<TypedValue> cast(ast::Node* node,
                                             llvm::IRBuilder<>& builder,
                                             CastType c, TypedValue* val,
                                             Type* to) const = 0;

    /**
     * Is a Type the same or implicitly castable to it
     * \param  node    Node where the implicit cast happens, used to create
     *                 error messages, set to `nullptr` to disable error
     * reporting
     * \param  builder LLVM IRBuilder
     * \param  val     Value to cast
     * \param  to      Type to cast to
     * \return         Is same or implicitly castable
     */
    bool isSameOrImplicitlyCastable(ast::Node* node, llvm::IRBuilder<>& builder,
                                    TypedValue* val, Type* to) const;

    virtual bool isSized() const
    {
        return true;
    }
    virtual bool isIntegral() const = 0;
    virtual bool isFloatingPoint() const = 0;

    /// Get a zero-initialized value of this type
    virtual std::unique_ptr<TypedValue> zeroInit() = 0;

    /// Get name of type
    std::string getName() const
    {
        return name;
    }

    /// Get type operations
    virtual TypeOperationBase* getOperations() const;

    /// Get the code for this function parameter name mangling
    virtual std::string getMangleEncoding() const = 0;

    /// Are types equal
    bool equal(const Type& t) const
    {
        return kind == t.kind && getName() == t.getName();
    }
    bool inequal(const Type& t) const
    {
        return !equal(t);
    }
    bool equal(Type* t) const
    {
        assert(t);
        return kind == t->kind && getName() == t->getName();
    }
    bool inequal(Type* t) const
    {
        assert(t);
        return !equal(t);
    }

    bool operator==(const Type& t) const
    {
        return equal(t);
    }
    bool operator!=(const Type& t) const
    {
        return inequal(t);
    }

    /// Type table
    TypeTable* typeTable;
    /// LLVM context
    llvm::LLVMContext& context;
    /// Underlying LLVM type
    llvm::Type* type;
    /// LLVM debug type
    llvm::DIType* dtype;
    /// Type kind
    Kind kind;
    /// Type name
    std::string name;

protected:
    std::unique_ptr<TypedValue> implicitCast(ast::Node* node,
                                             llvm::IRBuilder<>& builder,
                                             TypedValue* val, Type* to) const;

    template <typename... Args>
    std::nullptr_t castError(ast::Node* node, const std::string& format,
                             Args&&... args) const
    {
        return util::logCompilerError(node->loc, format,
                                      std::forward<Args>(args)...);
    }

    template <typename... Args>
    void castWarning(ast::Node* node, const std::string& format,
                     Args&&... args) const
    {
        util::logCompilerWarning(node->loc, format,
                                 std::forward<Args>(args)...);
    }

    template <typename... Args>
    void castInfo(ast::Node* node, const std::string& format,
                  Args&&... args) const
    {
        util::logCompilerInfo(node->loc, format, std::forward<Args>(args)...);
    }

private:
    /// Type operations
    std::unique_ptr<TypeOperationBase> operations;
};

class VoidType : public Type
{
public:
    VoidType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder&);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isSized() const override;
    bool isIntegral() const override;
    bool isFloatingPoint() const override;
    std::string getMangleEncoding() const override;
};

class IntegralType : public Type
{
public:
    IntegralType(TypeTable* list, size_t w, Kind k, llvm::LLVMContext& c,
                 llvm::Type* t, llvm::DIType* d, const std::string& n);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;

    size_t width;
};

class Int8Type : public IntegralType
{
public:
    Int8Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::string getMangleEncoding() const override;
};

class Int16Type : public IntegralType
{
public:
    Int16Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::string getMangleEncoding() const override;
};

class Int32Type : public IntegralType
{
public:
    Int32Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::string getMangleEncoding() const override;
};

class Int64Type : public IntegralType
{
public:
    Int64Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::string getMangleEncoding() const override;
};

class BoolType : public Type
{
public:
    BoolType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
    std::string getMangleEncoding() const override;
};

class CharacterType : public Type
{
public:
    CharacterType(TypeTable* list, size_t w, Kind k, llvm::LLVMContext& c,
                  llvm::Type* t, llvm::DIType* d, const std::string& n);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
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

    std::string getMangleEncoding() const override;
};

class ByteCharType : public CharacterType
{
public:
    ByteCharType(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder);

    std::string getMangleEncoding() const override;
};

class ByteType : public Type
{
public:
    ByteType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
    std::string getMangleEncoding() const override;
};

class FPType : public Type
{
public:
    FPType(TypeTable* list, size_t w, Kind k, llvm::LLVMContext& c,
           llvm::Type* t, llvm::DIType* d, const std::string& n);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;

    size_t width;
};

class F32Type : public FPType
{
public:
    F32Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::string getMangleEncoding() const override;
};

class F64Type : public FPType
{
public:
    F64Type(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder);

    std::string getMangleEncoding() const override;
};

class StringType : public Type
{
public:
    StringType(TypeTable* list, llvm::LLVMContext& c,
               llvm::DIBuilder& dbuilder);

    static llvm::StructType* getLLVMStringType(llvm::LLVMContext& c);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
    std::string getMangleEncoding() const override;
};

class CStringType : public Type
{
public:
    CStringType(TypeTable* list, llvm::LLVMContext& c,
                llvm::DIBuilder& dbuilder);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
    std::string getMangleEncoding() const override;
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

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
    std::string getMangleEncoding() const override;

    Type* returnType;
    std::vector<Type*> params;
};

class AliasType : public Type
{
public:
    AliasType(TypeTable* list, llvm::LLVMContext& c, llvm::DIBuilder& dbuilder,
              const std::string& pAliasName, Type* pUnderlying);

    std::unique_ptr<TypedValue> cast(ast::Node* node,
                                     llvm::IRBuilder<>& builder, CastType c,
                                     TypedValue* val, Type* to) const override;

    std::unique_ptr<TypedValue> zeroInit() override;

    bool isIntegral() const override;
    bool isFloatingPoint() const override;
    std::string getMangleEncoding() const override;

    TypeOperationBase* getOperations() const override;

    Type* underlying;
};
} // namespace codegen
