// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "codegen/Type.h"
#include "util/Logger.h"
#include "util/SafeEnum.h"

namespace codegen
{
class TypeTable
{
public:
    explicit TypeTable(llvm::Module* m) : module(m)
    {
    }

    enum _FindFlags : uint32_t
    {
        FIND_DEFAULT = 0
    };
    using FindFlags = util::SafeEnum<_FindFlags, uint32_t>;

    Type* find(const std::string& name, FindFlags flags = FIND_DEFAULT,
               bool logError = false);
    std::vector<Type*> findLLVM(llvm::Type* type, bool logError = false);

    const Type* find(const std::string& name, FindFlags flags = FIND_DEFAULT,
                     bool logError = false) const;
    const std::vector<Type*> findLLVM(llvm::Type* type,
                                      bool logError = false) const;

    size_t isDefined(const std::string& name, FindFlags = FIND_DEFAULT) const;
    size_t isDefinedLLVM(llvm::Type* type) const;

    template <typename T>
    Type* insertType(llvm::LLVMContext& context, llvm::DIBuilder& dbuilder);

    Type* insertType(std::unique_ptr<Type> type);

    template <typename T>
    void insertTypeWithVariants(llvm::LLVMContext& context,
                                llvm::DIBuilder& dbuilder);

    auto& getList()
    {
        return list;
    }
    const auto& getList() const
    {
        return list;
    }

    auto getModule()
    {
        return module;
    }

private:
    std::vector<std::unique_ptr<Type>> list;
    llvm::Module* module;
};

template <typename T>
inline Type* TypeTable::insertType(llvm::LLVMContext& context,
                                   llvm::DIBuilder& dbuilder)
{
    auto t = std::make_unique<T>(this, context, dbuilder);
    auto ptr = t.get();
    list.push_back(std::move(t));
    return ptr;
}
inline Type* TypeTable::insertType(std::unique_ptr<Type> type)
{
    auto ptr = type.get();
    list.push_back(std::move(type));
    return ptr;
}
template <typename T>
inline void TypeTable::insertTypeWithVariants(llvm::LLVMContext& context,
                                              llvm::DIBuilder& dbuilder)
{
    insertType<T>(context, dbuilder);
}

inline size_t TypeTable::isDefined(const std::string& name,
                                   TypeTable::FindFlags flags) const
{
    return (find(name, flags, false) != nullptr);
}
inline size_t TypeTable::isDefinedLLVM(llvm::Type* type) const
{
    return findLLVM(type, false).size();
}

inline Type* TypeTable::find(const std::string& name, TypeTable::FindFlags,
                             bool logError)
{
    auto it = std::find_if(
        list.begin(), list.end(),
        [&](const std::unique_ptr<Type>& t) { return t->getName() == name; });
    if(it == list.end())
    {
        if(logError)
        {
            util::logger->error("Undefined type: '{}'", name);
        }
        return nullptr;
    }
    return (*it).get();
}
inline std::vector<Type*> TypeTable::findLLVM(llvm::Type* type, bool logError)
{
    std::vector<Type*> ret;
    auto it = list.begin();
    auto end = list.end();
    while(true)
    {
        it = std::find_if(it, end, [&](const std::unique_ptr<Type>& t) {
            return t->type == type;
        });

        if(it == end)
        {
            break;
        }
        ret.push_back(it->get());
    }
    if(logError && ret.empty())
    {
        util::logger->error("Undefined type:");
        type->dump();
    }
    return ret;
}

inline const Type* TypeTable::find(const std::string& name,
                                   TypeTable::FindFlags /*unused*/,
                                   bool logError) const
{
    auto it = std::find_if(
        list.begin(), list.end(),
        [&](const std::unique_ptr<Type>& t) { return t->getName() == name; });
    if(it == list.end())
    {
        if(logError)
        {
            util::logger->error("Undefined type: '{}'", name);
        }
        return nullptr;
    }
    return (*it).get();
}
inline const std::vector<Type*> TypeTable::findLLVM(llvm::Type* type,
                                                    bool logError) const
{
    std::vector<Type*> ret;
    auto it = list.begin(), end = list.end();
    while(it != end)
    {
        it = std::find_if(it, end, [&](const std::unique_ptr<Type>& t) {
            return t->type == type;
        });
        if(it != end)
        {
            ret.push_back((*it).get());
        }
    }
    if(logError && ret.empty())
    {
        util::logger->error("Undefined type:");
        type->dump();
    }
    return ret;
}
} // namespace codegen
