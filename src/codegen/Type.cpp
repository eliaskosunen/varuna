// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "codegen/Type.h"
#include "codegen/TypeOperation.h"
#include "codegen/TypeTable.h"
#include "codegen/TypedValue.h"

// API changed for LLVM 4.0
#if VARUNA_LLVM_VERSION == 39
#define BASIC_TYPE_SIZE(num) num, num
#else
#define BASIC_TYPE_SIZE(num) num
#endif

namespace codegen
{
Type::Type(TypeTable* list, std::unique_ptr<TypeOperationBase> op, Type::Kind k,
           llvm::LLVMContext& c, llvm::Type* t, llvm::DIType* d, std::string n)
    : typeTable(list), context(c), type(t), dtype(d), kind(k),
      name(std::move(n)), operations(std::move(op))
{
}

Type::~Type() noexcept = default;

bool Type::isSameOrImplicitlyCastable(ast::Node* node,
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

std::string VoidType::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "X";
#else
    return "v";
#endif
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

Int8Type::Int8Type(TypeTable* list, llvm::LLVMContext& c,
                   llvm::DIBuilder& dbuilder)
    : IntegralType(list, 8, INT8, c, llvm::Type::getInt8Ty(c),
                   dbuilder.createBasicType("int8", BASIC_TYPE_SIZE(8),
                                            llvm::dwarf::DW_ATE_signed),
                   "int8")
{
}

std::string Int8Type::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "C";
#else
    return "a";
#endif
}

Int16Type::Int16Type(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder)
    : IntegralType(list, 16, INT16, c, llvm::Type::getInt16Ty(c),
                   dbuilder.createBasicType("i16", BASIC_TYPE_SIZE(16),
                                            llvm::dwarf::DW_ATE_signed),
                   "i16")
{
}

std::string Int16Type::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "F";
#else
    return "s";
#endif
}

Int32Type::Int32Type(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder)
    : IntegralType(list, 32, INT32, c, llvm::Type::getInt32Ty(c),
                   dbuilder.createBasicType("i32", BASIC_TYPE_SIZE(32),
                                            llvm::dwarf::DW_ATE_signed),
                   "i32")
{
}

std::string Int32Type::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "H";
#else
    return "i";
#endif
}

Int64Type::Int64Type(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& dbuilder)
    : IntegralType(list, 64, INT64, c, llvm::Type::getInt64Ty(c),
                   dbuilder.createBasicType("i64", BASIC_TYPE_SIZE(64),
                                            llvm::dwarf::DW_ATE_signed),
                   "i64")
{
}

std::string Int64Type::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "J";
#else
    return "l";
#endif
}

BoolType::BoolType(TypeTable* list, llvm::LLVMContext& c,
                   llvm::DIBuilder& dbuilder)
    : Type(list, std::make_unique<BoolTypeOperation>(this), BOOL, c,
           llvm::Type::getInt1Ty(c),
           dbuilder.createBasicType("bool", BASIC_TYPE_SIZE(1),
                                    llvm::dwarf::DW_ATE_boolean),
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

std::string BoolType::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "_N";
#else
    return "b";
#endif
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
                    dbuilder.createBasicType("char", BASIC_TYPE_SIZE(32),
                                             llvm::dwarf::DW_ATE_unsigned_char),
                    "char")
{
}

std::string CharType::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "_I";
#else
    return "Di";
#endif
}

ByteCharType::ByteCharType(TypeTable* list, llvm::LLVMContext& c,
                           llvm::DIBuilder& dbuilder)
    : CharacterType(list, 8, BCHAR, c, llvm::Type::getInt8Ty(c),
                    dbuilder.createBasicType("bchar", BASIC_TYPE_SIZE(8),
                                             llvm::dwarf::DW_ATE_unsigned_char),
                    "bchar")
{
}

std::string ByteCharType::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "D";
#else
    return "c";
#endif
}

ByteType::ByteType(TypeTable* list, llvm::LLVMContext& c,
                   llvm::DIBuilder& dbuilder)
    : Type(list, std::make_unique<ByteTypeOperation>(this), BYTE, c,
           llvm::Type::getInt8Ty(c),
           dbuilder.createBasicType("byte", BASIC_TYPE_SIZE(8),
                                    llvm::dwarf::DW_ATE_unsigned),
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

std::string ByteType::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "E";
#else
    return "h";
#endif
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

F32Type::F32Type(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder)
    : FPType(list, 32, F32, c, llvm::Type::getFloatTy(c),
             dbuilder.createBasicType("f32", BASIC_TYPE_SIZE(32),
                                      llvm::dwarf::DW_ATE_float),
             "f32")
{
}

std::string F32Type::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "M";
#else
    return "f";
#endif
}

F64Type::F64Type(TypeTable* list, llvm::LLVMContext& c,
                 llvm::DIBuilder& dbuilder)
    : FPType(list, 64, F64, c, llvm::Type::getDoubleTy(c),
             dbuilder.createBasicType("f64", BASIC_TYPE_SIZE(64),
                                      llvm::dwarf::DW_ATE_float),
             "f64")
{
}

std::string F64Type::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "N";
#else
    return "d";
#endif
}

StringType::StringType(TypeTable* list, llvm::LLVMContext& c,
                       llvm::DIBuilder& /*dbuilder*/)
    : Type(list, std::make_unique<StringTypeOperation>(this), STRING, c,
           getLLVMStringType(c), nullptr, "string")
{
}

llvm::StructType* StringType::getLLVMStringType(llvm::LLVMContext& c)
{
    return llvm::StructType::create(
        c, {llvm::Type::getInt64Ty(c), llvm::Type::getInt8PtrTy(c)}, "string");
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
    auto stringLen = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
    auto stringConst = llvm::ConstantDataArray::getString(context, "", false);
    auto stringGlobal = new llvm::GlobalVariable(
        *typeTable->getModule(), stringConst->getType(), true,
        llvm::GlobalValue::InternalLinkage, stringConst, ".str");
    auto indexConst =
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    std::vector<llvm::Constant*> indexList = {indexConst, indexConst};
    auto stringPtr = llvm::ConstantExpr::getGetElementPtr(
        stringConst->getType(), stringGlobal, indexList, true);

    auto val = llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(type),
                                         {stringLen, stringPtr});

    /*auto val = llvm::ConstantStruct::get(
        llvm::cast<llvm::StructType>(type),
        llvm::ConstantInt::get(type->getInt64Ty(type->getContext()), 0),
        llvm::ConstantInt::get(type->getInt8PtrTy(type->getContext()), 0),
        nullptr);*/
    return std::make_unique<TypedValue>(this, val, TypedValue::LVALUE, false);
}

std::string StringType::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "Ustring@@";
#else
    return "6string";
#endif
}

CStringType::CStringType(TypeTable* list, llvm::LLVMContext& c,
                         llvm::DIBuilder& /*dbuilder*/)
    : Type(list, std::make_unique<CStringTypeOperation>(this), CSTRING, c,
           llvm::Type::getInt8PtrTy(c), nullptr, "cstring")
{
}

bool CStringType::isIntegral() const
{
    return false;
}
bool CStringType::isFloatingPoint() const
{
    return false;
}

std::unique_ptr<TypedValue> CStringType::zeroInit()
{
    auto stringConst = llvm::ConstantDataArray::getString(context, "", true);
    auto stringGlobal = new llvm::GlobalVariable(
        *typeTable->getModule(), stringConst->getType(), true,
        llvm::GlobalValue::InternalLinkage, stringConst, ".str");
    auto indexConst =
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    std::vector<llvm::Constant*> indexList = {indexConst, indexConst};
    auto stringPtr = llvm::ConstantExpr::getGetElementPtr(
        stringConst->getType(), stringGlobal, indexList, true);

    auto val = stringPtr;

    /*auto val = llvm::ConstantStruct::get(
        llvm::cast<llvm::StructType>(type),
        llvm::ConstantInt::get(type->getInt64Ty(type->getContext()), 0),
        llvm::ConstantInt::get(type->getInt8PtrTy(type->getContext()), 0),
        nullptr);*/
    return std::make_unique<TypedValue>(this, val, TypedValue::LVALUE, false);
}

std::string CStringType::getMangleEncoding() const
{
#if VARUNA_WIN32
    return "PAD";
#else
    return "Pc";
#endif
}

FunctionType::FunctionType(TypeTable* list, llvm::LLVMContext& c,
                           llvm::DIBuilder&, Type* pReturnType,
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

std::string FunctionType::getMangleEncoding() const
{
    throw std::logic_error("Unimplemented FunctionType::getMangleEncoding");
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
    const std::vector<Type*>& params, llvm::DIFile* /*file*/)
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

AliasType::AliasType(TypeTable* list, llvm::LLVMContext& c,
                     llvm::DIBuilder& /*dbuilder*/,
                     const std::string& pAliasName, Type* pUnderlying)
    : Type(list, nullptr, pUnderlying->kind, c, pUnderlying->type, nullptr,
           pAliasName),
      underlying(pUnderlying)
{
}

bool AliasType::isIntegral() const
{
    return underlying->isIntegral();
}
bool AliasType::isFloatingPoint() const
{
    return underlying->isFloatingPoint();
}
std::unique_ptr<TypedValue> AliasType::zeroInit()
{
    return underlying->zeroInit();
}
TypeOperationBase* AliasType::getOperations() const
{
    return underlying->getOperations();
}

std::string AliasType::getMangleEncoding() const
{
    return underlying->getMangleEncoding();
}

} // namespace codegen
