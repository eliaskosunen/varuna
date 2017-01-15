// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/codegen/Type.h"
#include "util/Logger.h"

namespace core
{
namespace codegen
{
    class TypeTable
    {
    public:
        TypeTable() = default;

        std::vector<Type*> findUndecorated(const std::string& name,
                                           bool logError = true);
        Type* findDecorated(const std::string& name, bool logError = true);
        std::vector<Type*> findLLVM(llvm::Type* type, bool logError = true);

        const std::vector<Type*> findUndecorated(const std::string& name,
                                                 bool logError = true) const;
        const Type* findDecorated(const std::string& name,
                                  bool logError = true) const;
        const std::vector<Type*> findLLVM(llvm::Type* type,
                                          bool logError = true) const;

        size_t isDefinedUndecorated(const std::string& name) const;
        size_t isDefinedDecorated(const std::string& name) const;
        size_t isDefinedLLVM(llvm::Type* type) const;

        template <typename T>
        void insertType(llvm::LLVMContext& context, llvm::DIBuilder& dbuilder);
        template <typename T>
        void insertType(llvm::LLVMContext& context, llvm::DIBuilder& dbuilder, bool mut);

        void insertType(std::unique_ptr<Type> type);

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
    inline void TypeTable::insertType(llvm::LLVMContext& context, llvm::DIBuilder& dbuilder)
    {
        list.push_back(std::make_unique<T>(this, context, dbuilder));
    }
    template <typename T>
    inline void TypeTable::insertType(llvm::LLVMContext& context,
                                      llvm::DIBuilder& dbuilder, bool mut)
    {
        list.push_back(std::make_unique<T>(this, context, dbuilder, mut));
    }
    inline void TypeTable::insertType(std::unique_ptr<Type> type)
    {
        list.push_back(std::move(type));
    }
    template <typename T>
    inline void TypeTable::insertTypeWithVariants(llvm::LLVMContext& context,
                                                  llvm::DIBuilder& dbuilder,
                                                  bool mutableAllowed)
    {
        insertType<T>(context, dbuilder);
        if(mutableAllowed)
        {
            insertType<T>(context, dbuilder, true);
        }
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

    inline std::vector<Type*>
    TypeTable::findUndecorated(const std::string& name, bool logError)
    {
        std::vector<Type*> ret;
        auto it = list.begin();
        auto end = list.end();
        while(it != end)
        {
            it = std::find_if(it, end, [&](const std::unique_ptr<Type>& t) {
                return t.get()->getName() == name;
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
    inline Type* TypeTable::findDecorated(const std::string& name,
                                          bool logError)
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
    inline std::vector<Type*> TypeTable::findLLVM(llvm::Type* type,
                                                  bool logError)
    {
        std::vector<Type*> ret;
        auto it = list.begin();
        auto end = list.end();
        while(true)
        {
            it = std::find_if(it, end, [&](const std::unique_ptr<Type>& t) {
                return t.get()->type == type;
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

    inline const std::vector<Type*>
    TypeTable::findUndecorated(const std::string& name, bool logError) const
    {
        std::vector<Type*> ret;
        auto it = list.begin(), end = list.end();
        while(it != end)
        {
            it = std::find_if(it, end, [&](const std::unique_ptr<Type>& t) {
                return t.get()->getName() == name;
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
    inline const std::vector<Type*> TypeTable::findLLVM(llvm::Type* type,
                                                        bool logError) const
    {
        std::vector<Type*> ret;
        auto it = list.begin(), end = list.end();
        while(it != end)
        {
            it = std::find_if(it, end, [&](const std::unique_ptr<Type>& t) {
                return t.get()->type == type;
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
} // namespace core
