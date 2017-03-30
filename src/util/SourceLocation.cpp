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
    return fmt::format("{}:{}:{}", file->getFilename(), line, col);
}

std::string SourceLocation::contentToString() const
{
    assert(len != 0);
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
SourceLocation SourceLocation::operator++(int)noexcept
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
SourceLocation SourceLocation::operator--(int)noexcept
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
    auto getBeginningOfLine = [&](SourceLocation l) {
        assert(l.it < l.getEnd());
        for(;; --l)
        {
            if(*l == '\n')
            {
                return ++l;
            }
            if(l.it == l.getContent().begin())
            {
                return l;
            }
        }
    };

    auto getLine = [&](SourceLocation l) -> std::string {
        const auto begin = getBeginningOfLine(l);
        assert(begin.it < l.getEnd());
        const auto end = [&]() {
            auto b = begin;
            for(;; ++b)
            {
                if(*b == '\n')
                {
                    return b;
                }
                if(b.it == l.getEnd())
                {
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
            return ""s;
        }
        auto i = getBeginningOfLine(*this);
        assert(i.it < getEnd());
        if(i.it == content.begin())
        {
            return ""s;
        }
        assert((i - 2).it >= content.begin());
        return getLine(i - 2);
    }();

    auto getDisplayedWidth = [](std::string::const_iterator begin,
                                std::string::const_iterator end) {
        size_t result = 0;
        std::for_each(begin, end, [&](const char c) {
            ++result;
            if(c == '\t')
            {
                result += 3;
            }
        });
        return result;
    };

    const auto underline = [&]() {
        const auto lineBegin = getBeginningOfLine(*this);
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
