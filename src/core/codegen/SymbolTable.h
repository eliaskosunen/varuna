// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "core/codegen/Symbol.h"
#include "util/Logger.h"

namespace core
{
namespace codegen
{
    class SymbolTable
    {
    public:
        SymbolTable() = default;

        Symbol* find(const std::string& name, Type::Kind type,
                     bool logError = true);
        Symbol* find(const std::string& name, Type* type = nullptr,
                     bool logError = true);
        const Symbol* find(const std::string& name, Type::Kind type,
                           bool logError = true) const;
        const Symbol* find(const std::string& name, Type* type = nullptr,
                           bool logError = true) const;

        bool isDefined(const std::string& name, Type::Kind kind) const;
        bool isDefined(const std::string& name, Type* type = nullptr) const;

        void addBlock();
        void removeTopBlock();
        void clear();

        auto& getTop()
        {
            assert(!list.empty() &&
                   "Cannot get the top of a empty symbol list");
            return list.back();
        }
        const auto& getTop() const
        {
            assert(!list.empty() &&
                   "Cannot get the top of a empty symbol list");
            return list.back();
        }

        auto& getList()
        {
            return list;
        }
        const auto& getList() const
        {
            return list;
        }

        void dump() const;

    private:
        std::vector<std::unordered_map<std::string, std::unique_ptr<Symbol>>>
            list;
    };

    inline bool SymbolTable::isDefined(const std::string& name,
                                       Type::Kind kind) const
    {
        return find(name, kind);
    }
    inline bool SymbolTable::isDefined(const std::string& name,
                                       Type* type) const
    {
        return find(name, type);
    }

    inline void SymbolTable::addBlock()
    {
        list.push_back({});
    }
    inline void SymbolTable::removeTopBlock()
    {
        getTop().clear();
        list.pop_back();
    }

    inline void SymbolTable::clear()
    {
        list.clear();
    }

    inline Symbol* SymbolTable::find(const std::string& name, Type::Kind type,
                                     bool logError)
    {
        Symbol* var = nullptr;
        std::all_of(list.rbegin(), list.rend(), [&](auto& block) {
            auto it = block.find(name);
            if(it != block.end())
            {
                if(type != it->second->value->type->kind)
                {
                    return true; // continue
                }
                var = it->second.get();
                return false; // break
            }
            return true; // continue
        });
        if(!var)
        {
            if(logError)
            {
                util::logger->error("Symbol '{}' with the kind of {} not found",
                                    name, type);
            }
        }
        return var;
    }

    inline Symbol* SymbolTable::find(const std::string& name, Type* type,
                                     bool logError)
    {
        Symbol* var = nullptr;
        std::all_of(list.rbegin(), list.rend(), [&](auto& block) {
            auto it = block.find(name);
            if(it != block.end())
            {
                if(type)
                {
                    if(type != it->second->getType())
                    {
                        return true; // continue
                    }
                }
                var = it->second.get();
                return false; // break
            }
            return true; // continue
        });
        if(!var)
        {
            if(logError)
            {
                if(type)
                {
                    util::logger->error("Symbol '{}' with type '{}' not found",
                                        name, type->name);
                }
                else
                {
                    util::logger->error("Symbol '{}' not found", name);
                }
            }
        }
        return var;
    }

    inline const Symbol* SymbolTable::find(const std::string& name,
                                           Type::Kind type, bool logError) const
    {
        const Symbol* var = nullptr;
        std::all_of(list.rbegin(), list.rend(), [&](auto& block) {
            auto it = block.find(name);
            if(it != block.end())
            {
                if(type != it->second->value->type->kind)
                {
                    return true; // continue
                }
                var = it->second.get();
                return false; // break
            }
            return true; // continue
        });
        if(!var)
        {
            if(logError)
            {
                util::logger->error("Symbol '{}' with the kind of {} not found",
                                    name, type);
            }
        }
        return var;
    }

    inline const Symbol* SymbolTable::find(const std::string& name, Type* type,
                                           bool logError) const
    {
        const Symbol* var = nullptr;
        std::all_of(list.rbegin(), list.rend(), [&](auto& block) {
            auto it = block.find(name);
            if(it != block.end())
            {
                if(type)
                {
                    if(type != it->second->getType())
                    {
                        return true; // continue
                    }
                }
                var = it->second.get();
                return false; // break
            }
            return true; // continue
        });
        if(!var)
        {
            if(logError)
            {
                if(type)
                {
                    util::logger->error("Symbol '{}' with type '{}' not found",
                                        name, type->name);
                }
                else
                {
                    util::logger->error("Symbol '{}' not found", name);
                }
            }
        }
        return var;
    }

    inline void SymbolTable::dump() const
    {
        auto stlogger = spdlog::stdout_logger_st("SymbolTableDump");
        stlogger->set_pattern("DumpSymbolTable: %v");

        stlogger->trace("*** SYMBOLTABLE DUMP ***");
        stlogger->trace("SymbolTable");
        for(size_t i = 0; i < list.size(); ++i)
        {
            const std::string prefix = [i]() {
                using namespace std::string_literals;

                if(i == 0)
                {
                    return "\\-"s;
                }
                std::string buf = "\\-";
                for(size_t j = 1; j <= i; ++j)
                {
                    buf.append("--");
                }
                return buf;
            }();
            stlogger->set_pattern("DumpSymbolTable: " + prefix + "%v");

            for(const auto& s : list[i])
            {
                stlogger->trace("Symbol:");
                stlogger->trace(" * Name: {}", s.second->name);
                stlogger->trace(" * Type: {}", s.second->getType()->name);
            }
        }
        stlogger->set_pattern("DumpSymbolTable: %v");
        stlogger->trace("*** END SYMBOLTABLE DUMP ***");
    }
}
}
