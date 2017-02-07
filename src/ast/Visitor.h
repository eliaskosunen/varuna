// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

namespace ast
{
/// Base class of the AST visitros
class Visitor
{
public:
    Visitor() = default;

    Visitor(const Visitor&) = delete;
    Visitor(Visitor&&) = default;

    Visitor& operator=(const Visitor&) = delete;
    Visitor& operator=(Visitor&&) = default;

    virtual ~Visitor() noexcept = default;
};
} // namespace ast
