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
    TypeTable() = default;

    enum FindFlags_t : uint32_t
    {
        FIND_DEFAULT = 0,
        FIND_MUTABLE = 1 << 0
    };
    using FindFlags = util::SafeEnum<FindFlags_t, uint32_t>;

    Type* find(const std::string& name, FindFlags flags = FIND_DEFAULT,
               bool logError = true);
    std::vector<Type*> findUndecorated(const std::string& name,
                                       bool logError = true);
    Type* findDecorated(const std::string& name, bool logError = true);
    std::vector<Type*> findLLVM(llvm::Type* type, bool logError = true);

    const Type* find(const std::string& name, FindFlags flags = FIND_DEFAULT,
                     bool logError = true) const;
    const std::vector<Type*> findUndecorated(const std::string& name,
                                             bool logError = true) const;
    const Type* findDecorated(const std::string& name,
                              bool logError = true) const;
    const std::vector<Type*> findLLVM(llvm::Type* type,
                                      bool logError = true) const;

    size_t isDefined(const std::string& name, FindFlags = FIND_DEFAULT) const;
    size_t isDefinedUndecorated(const std::string& name) const;
    size_t isDefinedDecorated(const std::string& name) const;
    size_t isDefinedLLVM(llvm::Type* type) const;

    template <typename T>
    Type* insertType(llvm::LLVMContext& context, llvm::DIBuilder& dbuilder,
                     bool mut = false);

    Type* insertType(std::unique_ptr<Type> type);

    template <typename T>
    void insertTypeWithVariants(llvm::LLVMContext& context,
                                llvm::DIBuilder& dbuilder,
                                bool mutableAllowed = true);

    auto& getList()
    {
        return list;
    }
    const auto& getList() const
    {
        return list;
    }

private:
    std::vector<std::unique_ptr<Type>> list;
};

template <typename T>
inline Type* TypeTable::insertType(llvm::LLVMContext& context,
                                   llvm::DIBuilder& dbuilder, bool mut)
{
    auto t = std::make_unique<T>(this, context, dbuilder, mut);
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
                                              llvm::DIBuilder& dbuilder,
                                              bool mutableAllowed)
{
    auto t = insertType<T>(context, dbuilder);
    t->dtype =
        dbuilder.createQualifiedType(llvm::dwarf::DW_TAG_const_type, t->dtype);
    if(mutableAllowed)
    {
        insertType<T>(context, dbuilder, true);
    }
}

inline size_t TypeTable::isDefined(const std::string& name,
                                   TypeTable::FindFlags flags) const
{
    return (find(name, flags, false) != nullptr);
}
inline size_t TypeTable::isDefinedUndecorated(const std::string& name) const
{
    return findUndecorated(name, false).size();
}
inline size_t TypeTable::isDefinedDecorated(const std::string& name) const
{
    return (findDecorated(name, false) != nullptr);
}
inline size_t TypeTable::isDefinedLLVM(llvm::Type* type) const
{
    return findLLVM(type, false).size();
}

inline Type* TypeTable::find(const std::string& name,
                             TypeTable::FindFlags flags, bool logError)
{
    auto p = [&](const std::unique_ptr<Type>& t) {
        if(t->getName() != name)
        {
            return false;
        }
        if(flags.isSet(FIND_MUTABLE))
        {
            if(t->isMutable)
            {
                return true;
            }
            return false;
        }
        if(t->isMutable)
        {
            return false;
        }
        return true;
    };
    auto it = std::find_if(list.begin(), list.end(), p);
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
inline std::vector<Type*> TypeTable::findUndecorated(const std::string& name,
                                                     bool logError)
{
    std::vector<Type*> ret;
    auto it = list.begin();
    auto end = list.end();
    while(it != end)
    {
        it = std::find_if(it, end, [&](const std::unique_ptr<Type>& t) {
            return t->getName() == name;
        });
        if(it != end)
        {
            ret.push_back((*it).get());
        }
    }
    if(logError && ret.empty())
    {
        util::logger->error("Undefined type: '{}'", name);
    }
    return ret;
}
inline Type* TypeTable::findDecorated(const std::string& name, bool logError)
{
    auto it = std::find_if(list.begin(), list.end(),
                           [&](const std::unique_ptr<Type>& t) {
                               return t.get()->getDecoratedName() == name;
                           });
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
                                   TypeTable::FindFlags flags,
                                   bool logError) const
{
    auto p = [&](const std::unique_ptr<Type>& t) {
        if(t->getName() != name)
        {
            return false;
        }
        if(flags.isSet(FIND_MUTABLE))
        {
            if(t->isMutable)
            {
                return true;
            }
            return false;
        }
        if(t->isMutable)
        {
            return false;
        }
        return true;
    };
    auto it = std::find_if(list.begin(), list.end(), p);
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
inline const std::vector<Type*>
TypeTable::findUndecorated(const std::string& name, bool logError) const
{
    std::vector<Type*> ret;
    auto it = list.begin(), end = list.end();
    while(it != end)
    {
        it = std::find_if(it, end, [&](const std::unique_ptr<Type>& t) {
            return t->getName() == name;
        });
        if(it != end)
        {
            ret.push_back((*it).get());
        }
    }
    if(logError && ret.empty())
    {
        util::logger->error("Undefined type: '{}'", name);
    }
    return ret;
}
inline const Type* TypeTable::findDecorated(const std::string& name,
                                            bool logError) const
{
    auto it = std::find_if(list.begin(), list.end(),
                           [&](const std::unique_ptr<Type>& t) {
                               return t->getDecoratedName() == name;
                           });
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
