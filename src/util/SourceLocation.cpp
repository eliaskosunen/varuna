// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/SourceLocation.h"
#include "util/Logger.h"
#include "util/MathUtils.h"
#include "util/StringUtils.h"
#include <iterator>

namespace util
{
SourceLocation::SourceLocation(std::shared_ptr<util::File> f, uint32_t l,
                               uint32_t c, std::string::const_iterator i,
                               size_t pLen)
    : file(std::move(f)), line(l), col(c), it(i), len(pLen)
{
}

std::string::const_iterator SourceLocation::invalidIterator()
{
    static const std::string invalid(1, '\0');
    return invalid.cend();
}

const std::string& SourceLocation::getContent() const
{
    return file->getContent();
}
const std::string::const_iterator SourceLocation::getEnd() const
{
    return getContent().end();
}

std::string SourceLocation::toString() const
{
    assert(file);
    // code.va:10:3
    return fmt::format("{}:{}:{}", file->getFilename(), line, col);
}

std::string SourceLocation::contentToString() const
{
    assert(len > 0);
    assert(it < getEnd());
    assert(static_cast<size_t>(std::distance(it, getEnd())) >= len);
    return std::string(it, it + static_cast<ptrdiff_t>(len));
}

char SourceLocation::operator*() const noexcept
{
    if(it == getEnd())
    {
        static const char tmp = '\0';
        return tmp;
    }
    return *it;
}

SourceLocation& SourceLocation::operator++() noexcept
{
    if(it == getEnd())
    {
        return *this;
    }
    if(*it == '\n')
    {
        ++line;
        lastLineLen = std::exchange(col, 1);
    }
    else
    {
        ++col;
    }
    ++it;
    return *this;
}
SourceLocation SourceLocation::operator++(int dummy) noexcept
{
    SourceLocation ret(*this);
    ++(*this);
    return ret;
}
SourceLocation SourceLocation::operator+(std::ptrdiff_t dist) const noexcept
{
    SourceLocation ret(*this);
    if(dist < 0)
    {
        return (*this) - (-dist);
    }
    for(std::ptrdiff_t i = 0; i < dist; ++i)
    {
        ++ret;
    }
    return ret;
}
SourceLocation& SourceLocation::operator+=(std::ptrdiff_t dist) noexcept
{
    *this = (*this) + dist;
    return *this;
}

SourceLocation& SourceLocation::operator--() noexcept
{
    --it;
    if(*it == '\n')
    {
        --line;
        col = lastLineLen;
    }
    else
    {
        --col;
    }
    return *this;
}
SourceLocation SourceLocation::operator--(int dummy) noexcept
{
    SourceLocation ret(*this);
    --(*this);
    return ret;
}
SourceLocation SourceLocation::operator-(std::ptrdiff_t dist) const noexcept
{
    SourceLocation ret(*this);
    if(dist < 0)
    {
        return (*this) + (-dist);
    }
    for(std::ptrdiff_t i = 0; i < dist; ++i)
    {
        --ret;
    }
    return ret;
}
SourceLocation& SourceLocation::operator-=(std::ptrdiff_t dist) noexcept
{
    *this = (*this) - dist;
    return *this;
}

bool SourceLocation::operator==(const SourceLocation& rhs) const noexcept
{
    return it == rhs.it;
}
bool SourceLocation::operator!=(const SourceLocation& lhs) const noexcept
{
    return !(*this == lhs);
}

bool SourceLocation::hasMore() const noexcept
{
    return it != getEnd();
}
size_t SourceLocation::remaining() const noexcept
{
    return static_cast<size_t>(std::distance(it, getEnd()));
}

std::string SourceLocation::getErrorMessage() const
{
    using namespace fmt::literals;
    using namespace std::string_literals;

    assert(file);
    assert(line > 0);
    assert(col > 0);
    assert(it != getEnd());

    const auto& content = getContent();

    // Get SourceLocation pointing to the beginning of a line
    auto getBeginningOfLine = [&](SourceLocation l) {
        assert(l.it < l.getEnd());
        for(;; --l)
        {
            if(*l == '\n')
            {
                // If '\n', go forward one character to get first of the line
                return ++l;
            }
            if(l.it == l.getContent().begin())
            {
                // There's no previous line of the first character
                return l;
            }
        }
    };

    const auto lineBegin = getBeginningOfLine(*this);
    assert(lineBegin.it < getEnd());
    assert(lineBegin.it >= file->getContent().begin());

    auto getLine = [&](SourceLocation l) {
        const auto begin = getBeginningOfLine(l);
        assert(begin.it < l.getEnd());
        assert(lineBegin.it >= file->getContent().begin());

        const auto end = [&]() {
            // Use a similar tactic as in getBeginningOfLine but reversed
            auto b = begin;
            for(;; ++b)
            {
                if(*b == '\n')
                {
                    // '\n' is always the last character of its line...
                    return b;
                }
                if(b.it == l.getEnd())
                {
                    // ...except when it's the last line
                    // --b because otherwise we'd return an iterator to the end
                    return --b;
                }
            }
        }();

        assert(begin.it <= end.it);
        if(begin.it == end.it)
        {
            return ""s;
        }
        return std::string(begin.it, end.it);
    };

    const auto lineContent = getLine(*this);
    const auto prevLineContent = [&]() {
        assert(line > 0);
        if(line == 1)
        {
            // The first line doesn't have a previous line
            return ""s;
        }

        // lineBegin points to the beginning of *this line
        // lineBegin - 1 is is the line break of the previous line
        // If we'd use that we'd get the contents of *this line
        // So we'll use lineBegin - 2
        assert((lineBegin - 2).it >= content.begin());
        return getLine(lineBegin - 2);
    }();

    auto getDisplayedWidth = [](std::string::const_iterator begin,
                                std::string::const_iterator end) {
        size_t result = 0;
        std::for_each(begin, end, [&](const char c) {
            // We *could* be Unicode-friendly,
            // but we aren't
            // Every character is 1 'unit' wide
            ++result;
            if(c == '\t')
            {
                // Except a tab is 4 (1 + 3)
                result += 3;
            }
        });
        return result;
    };

    const auto underline = [&]() {
        assert(lineBegin.it <= it);
        const auto leftPad = util::stringutils::createEmptyStringWithLength(
            getDisplayedWidth(lineBegin.it, it));

        const auto underlineStr =
            util::stringutils::createStringWithLength('^', len);
        assert(underlineStr.length() == len);

        return leftPad + underlineStr;
    }();

    const auto padding =
        util::stringutils::createEmptyStringWithLength(numDigits(line));
    return fmt::format("{padding} | {prevLineContent}\n{line} | "
                       "{lineContent}\n{padding} | {underline}",
                       "padding"_a = padding, "line"_a = line,
                       "prevLineContent"_a = prevLineContent,
                       "lineContent"_a = lineContent,
                       "underline"_a = underline);
}
} // namespace util
