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
     * \param  name     Name of the symbol
     * \param  type     Kind of the symbol
     * \param  logError Log the error
     * \return          Found symbol or nullptr on error
     */
    Symbol* find(const std::string& name, Type::Kind type,
                 bool logError = false);
    /**
     * Find symbol by name and optionally by Type
     * \param  name     Name of the symbol
     * \param  type     Type of the symbol
     * \param  logError Log the error
     * \return          Found symbol or nullptr on error
     */
    Symbol* find(const std::string& name, Type* type = nullptr,
                 bool logError = false);
    const Symbol* find(const std::string& name, Type::Kind type,
                       bool logError = false) const;
    const Symbol* find(const std::string& name, Type* type = nullptr,
                       bool logError = false) const;

    /**
     * Is symbol defined
     * \param  name Name of the symbol
     * \param  kind Kind of the symbol
     * \return      isDefined
     */
    bool isDefined(const std::string& name, Type::Kind kind) const;
    /**
     * Is symbol defined
     * \param  name Name of the symbol
     * \param  type Type of the symbol
     * \return      isDefined
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
     * \return Top scope
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
     * \return Symbol list
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

    std::unique_ptr<SymbolTable> findExports() const;

    bool import(std::unique_ptr<SymbolTable> symbols);

private:
    /// The table itself
    std::vector<std::unordered_map<std::string, std::unique_ptr<Symbol>>> list;
};

inline bool SymbolTable::isDefined(const std::string& name,
                                   Type::Kind kind) const
{
    return find(name, kind) != nullptr;
}
inline bool SymbolTable::isDefined(const std::string& name, Type* type) const
{
    return find(name, type) != nullptr;
}

inline void SymbolTable::addBlock()
{
    list.emplace_back();
}
inline void SymbolTable::removeTopBlock()
{
    list.pop_back();
}

inline void SymbolTable::clear()
{
    list.clear();
}

inline std::unique_ptr<SymbolTable> SymbolTable::findExports() const
{
    auto found = std::make_unique<SymbolTable>();
    for(const auto& block : list)
    {
        found->addBlock();
        for(const auto& symbol : block)
        {
            if(symbol.second->isExport)
            {
                found->getTop().insert(
                    std::make_pair(symbol.first, symbol.second->clone()));
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
                                    name, type->getName());
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
                                    name, type->getName());
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
        const auto prefix = [i]() {
            std::string buf = "\\-";
            if(i == 0)
            {
                return buf;
            }
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
            stlogger->trace(" * Type: {}", s.second->getType()->getName());
        }
    }
    stlogger->set_pattern("DumpSymbolTable: %v");
    stlogger->trace("*** END SYMBOLTABLE DUMP ***");
}

inline bool SymbolTable::import(std::unique_ptr<SymbolTable> symbols)
{
    auto slist = symbols->consumeList();
    if(slist.size() > 1)
    {
        util::logger->warn(
            "Import has more than one scope, only importing the top one");
    }
    if(slist.empty())
    {
        return true;
    }
    assert(!list.empty());
    for(auto& s : slist[0])
    {
        if(find(s.first))
        {
            util::logger->error("Name conflict: Imported name '{}' can already "
                                "be found in this scope",
                                s.first);
            return false;
        }
        // FIXME: Try to do this once and not for each element
        list[0].insert(std::move(s));
    }
    // Not possible, tries to copy elements instead of moving
    // list[0].insert(slist[0].begin(), slist[0].end());

    return true;
}
} // namespace codegen
