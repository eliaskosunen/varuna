// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "codegen/Symbol.h"
#include "util/Logger.h"

namespace codegen
{
/// Module symbol table
class SymbolTable
{
public:
    SymbolTable() = default;

    /**
     * Find a symbol by name and kind
     * @param  name     Name of the symbol
     * @param  type     Kind of the symbol
     * @param  logError Log the error
     * @return          Found symbol or nullptr on error
     */
    Symbol* find(const std::string& name, Type::Kind type,
                 bool logError = false);
    /**
     * Find symbol by name and optionally by Type
     * @param  name     Name of the symbol
     * @param  type     Type of the symbol
     * @param  logError Log the error
     * @return          Found symbol or nullptr on error
     */
    Symbol* find(const std::string& name, Type* type = nullptr,
                 bool logError = false);
    const Symbol* find(const std::string& name, Type::Kind type,
                       bool logError = false) const;
    const Symbol* find(const std::string& name, Type* type = nullptr,
                       bool logError = false) const;

    /**
     * Is symbol defined
     * @param  name Name of the symbol
     * @param  kind Kind of the symbol
     * @return      isDefined
     */
    bool isDefined(const std::string& name, Type::Kind kind) const;
    /**
     * Is symbol defined
     * @param  name Name of the symbol
     * @param  type Type of the symbol
     * @return      isDefined
     */
    bool isDefined(const std::string& name, Type* type = nullptr) const;

    /**
     * Add a new scope
     */
    void addBlock();
    /**
     * Remove the top scope
     */
    void removeTopBlock();
    /**
     * Clear all scopes
     */
    void clear();

    /**
     * Get the top scope
     * @return Top scope
     */
    auto& getTop()
    {
        assert(!list.empty() && "Cannot get the top of a empty symbol list");
        return list.back();
    }
    const auto& getTop() const
    {
        assert(!list.empty() && "Cannot get the top of a empty symbol list");
        return list.back();
    }

    /**
     * Get the entire symbol table
     * @return Symbol list
     */
    auto& getList()
    {
        return list;
    }
    const auto& getList() const
    {
        return list;
    }

    auto&& consumeList()
    {
        return std::move(list);
    }

    /**
     * Dump the symbol table to stdout
     */
    void dump() const;

    std::vector<Symbol*> findExports() const;

    bool import(SymbolTable&& symbols);

private:
    /// The table itself
    std::vector<std::unordered_map<std::string, std::unique_ptr<Symbol>>> list;
};

inline bool SymbolTable::isDefined(const std::string& name,
                                   Type::Kind kind) const
{
    return find(name, kind);
}
inline bool SymbolTable::isDefined(const std::string& name, Type* type) const
{
    return find(name, type);
}

inline void SymbolTable::addBlock()
{
    // Empty initializer list {} errors on some configurations
    // Dunno why
    list.push_back(decltype(list)::value_type{});
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

inline std::vector<Symbol*> SymbolTable::findExports() const
{
    std::vector<Symbol*> found;
    for(const auto& block : list)
    {
        for(const auto& symbol : block)
        {
            if(symbol.second->isExport)
            {
                found.push_back(symbol.second.get());
            }
        }
    }
    return found;
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
                                    name, type->getDecoratedName());
            }
            else
            {
                util::logger->error("Symbol '{}' not found", name);
            }
        }
    }
    return var;
}

inline const Symbol* SymbolTable::find(const std::string& name, Type::Kind type,
                                       bool logError) const
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
                                    name, type->getDecoratedName());
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
    auto stlogger = util::createLogger(false, "dumpsymboltable_logger");
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
            stlogger->trace(" * Type: {}",
                            s.second->getType()->getDecoratedName());
        }
    }
    stlogger->set_pattern("DumpSymbolTable: %v");
    stlogger->trace("*** END SYMBOLTABLE DUMP ***");
}

inline bool SymbolTable::import(SymbolTable&& symbols)
{
    // TODO: Check for name collisions
    auto slist = symbols.consumeList();
    for(auto& s : slist)
    {
        list.push_back(std::move(s));
    }
    return true;
}
} // namespace codegen
