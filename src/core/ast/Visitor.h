// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

namespace core
{
namespace ast
{
    class Visitor
    {
    public:
        Visitor() = default;

        Visitor(const Visitor&) = delete;
        Visitor(Visitor&&) noexcept = default;

        Visitor& operator=(const Visitor&) = delete;
        Visitor& operator=(Visitor&&) noexcept = default;

        virtual ~Visitor() noexcept = default;
    };
} // namespace ast
} // namespace core
