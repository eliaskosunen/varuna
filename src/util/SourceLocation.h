// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/File.h"
#include <cereal.h>

namespace util
{
class SourceLocation
{
public:
    static std::string::const_iterator invalidIterator();

    SourceLocation() = default;
    SourceLocation(std::shared_ptr<util::File> f, uint32_t l, uint32_t c,
                   std::string::const_iterator i, size_t pLen = 1);

    std::string toString() const;
    std::string contentToString() const;

    std::string getErrorMessage() const;

    char operator*() const noexcept;

    SourceLocation& operator++() noexcept;
    SourceLocation operator++(int)noexcept;
    SourceLocation operator+(std::ptrdiff_t dist) const noexcept;
    SourceLocation& operator+=(std::ptrdiff_t dist) noexcept;

    SourceLocation& operator--() noexcept;
    SourceLocation operator--(int)noexcept;
    SourceLocation operator-(std::ptrdiff_t dist) const noexcept;
    SourceLocation& operator-=(std::ptrdiff_t dist) noexcept;

    bool operator==(const SourceLocation& rhs) const noexcept;
    bool operator!=(const SourceLocation& lhs) const noexcept;

    bool hasMore() const noexcept;
    size_t remaining() const noexcept;

    /// File
    std::shared_ptr<util::File> file{nullptr};
    /// Line
    uint32_t line{0};
    /// Column
    uint32_t col{0};
    /// Iterator
    std::string::const_iterator it{invalidIterator()};
    /// Length
    size_t len{1};

    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(file), CEREAL_NVP(line), CEREAL_NVP(col),
                CEREAL_NVP(len));
    }

private:
    const std::string& getContent() const;
    const std::string::const_iterator getEnd() const;

    uint32_t lastLineLen{0};
};
}
