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

    /**
     * Get the location as string.
     * Format: `{file}:{line}:{col}`
     * \pre `file` to be valid
     */
    std::string toString() const;
    /**
     * Get the string contents in location.
     * \pre `len > 0`
     */
    std::string contentToString() const;

    /**
     * Format an error message on this location and return it.
     * Format:
     \verbatim
      | {previous line contents}
 line | {erroneous line contents}
      |    ^^^ (shows the error)
     \endverbatim
     * \pre `file` must be valid
     * \pre `line > 0`
     * \pre `col > 0`
     * \pre `it != getEnd()`
     */
    std::string getErrorMessage() const;

    /**
     * Dereferences `it`
     * @return Character `it` points to or `\0` if pointing to the end
     */
    char operator*() const noexcept;

    SourceLocation& operator++() noexcept;
    SourceLocation operator++(int dummy) noexcept;
    SourceLocation operator+(std::ptrdiff_t dist) const noexcept;
    SourceLocation& operator+=(std::ptrdiff_t dist) noexcept;

    SourceLocation& operator--() noexcept;
    SourceLocation operator--(int dummy) noexcept;
    SourceLocation operator-(std::ptrdiff_t dist) const noexcept;
    SourceLocation& operator-=(std::ptrdiff_t dist) noexcept;

    bool operator==(const SourceLocation& rhs) const noexcept;
    bool operator!=(const SourceLocation& lhs) const noexcept;

    /**
     * Is there characters after `it`.
     * Basically `it != getEnd()`
     */
    bool hasMore() const noexcept;
    /**
     * How many steps is there between `it` and `end`.
     * Calls `std::distance` between `it` and `getEnd()`
     */
    size_t remaining() const noexcept;

    /// File
    std::shared_ptr<util::File> file{nullptr};
    /// Line
    uint32_t line{0};
    /// Column
    uint32_t col{0};
    /// Iterator pointing to the right position File::content
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
} // namespace util
