// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include <fmt.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <functional>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#define CHAR_SIGNED CHAR_MIN < 0

namespace util
{
/**
 * String utility functions
 */
namespace stringutils
{
    /**
     * Replace all occurences of needle with replacement in haystack
     * @param haystack    String to search in
     * @param needle      String to replace
     * @param replacement Replacement string
     */
    inline void replaceAll(std::string& haystack, const std::string& needle,
                           const std::string& replacement)
    {
        size_t start_pos = 0;
        while((start_pos = haystack.find(needle, start_pos)) !=
              std::string::npos)
        {
            haystack.replace(start_pos, needle.length(), replacement);
            start_pos += replacement.length(); // Handles case where
                                               // 'replacement' is a substring
                                               // of 'needle'
        }
    }

    /**
     * Replace all occurences of needle with replacement in haystack and return
     * a copy
     * @param  haystack    String to search in
     * @param  needle      String to replace
     * @param  replacement Replacement string
     * @return             Copy of the final string
     */
    inline std::string replaceAllCopy(std::string haystack,
                                      const std::string& needle,
                                      const std::string& replacement)
    {
        replaceAll(haystack, needle, replacement);
        return haystack;
    }

    inline std::string trim(const std::string& s)
    {
        auto wsfront = std::find_if_not(s.begin(), s.end(),
                                        [](int c) { return std::isspace(c); });
        return std::string(
            wsfront,
            std::find_if_not(s.rbegin(),
                             std::string::const_reverse_iterator(wsfront),
                             [](int c) { return std::isspace(c); })
                .base());
    }

    /**
     * Trim off all consecutive spaces from a string
     * @param str String to trim
     */
    inline void trimConsecutiveSpaces(std::string& str)
    {
        std::string::iterator newEnd =
            std::unique(str.begin(), str.end(), [](char lhs, char rhs) {
                return (lhs == rhs) && (lhs == ' ');
            });
        str.erase(newEnd, str.end());
    }

    /**
     * Trim off all consecutive spaces from a string and return a copy
     * @param  str String to trim
     * @return     Trimmed string
     */
    inline std::string trimConsecutiveSpacesCopy(std::string str)
    {
        trimConsecutiveSpaces(str);
        return str;
    }

    /**
     * Convert a C string (const char*) to a C++ string (std::string)
     * @param  cstr C string to convert
     * @return      Equivalent C++ string
     */
    inline std::string cstrToString(const char* cstr)
    {
        std::stringstream ss;
        ss << cstr;
        return ss.str();
    }

    /**
     * Convert a C string (const char*) to a C++ string (std::string) with
     * length
     * information
     * @param  cstr C string to convert
     * @param  len  Length of the C string
     * @return      Equivalent C++ string
     */
    inline std::string cstrToStringLen(const char* cstr, size_t len)
    {
        return std::string(cstr, len);
    }

    /**
     * Convert a char to C++ string
     * @param  c Char to convert
     * @return   Equivalent C++ string
     */
    inline std::string charToString(const char& c)
    {
        std::stringstream ss;
        ss << c;
        return ss.str();
    }

    inline void split(const char* str, char delim,
                      std::vector<std::string>& result)
    {
        do
        {
            const char* begin = str;

            while(*str != delim && *str)
            {
                str++;
            }

            result.emplace_back(begin, str);
        } while(*str++ != 0x0);
    }

    inline std::vector<std::string> split(const char* str, char delim)
    {
        std::vector<std::string> result;
        split(str, delim, result);
        return result;
    }

    inline void split(const std::string& str, char delim,
                      std::vector<std::string>& result)
    {
        split(str.c_str(), delim, result);
    }

    inline std::vector<std::string> split(const std::string& str, char delim)
    {
        return split(str.c_str(), delim);
    }

    inline bool isCharAlpha(char c)
    {
        return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
    }

    inline bool isCharDigit(char c)
    {
        return (c >= '0' && c <= '9');
    }

    inline bool isCharAlnum(char c)
    {
        return (isCharAlpha(c) || isCharDigit(c));
    }

    inline bool isCharHexDigit(char c)
    {
        return (isCharDigit(c) || (c >= 'A' && c <= 'F') ||
                (c >= 'a' && c <= 'f'));
    }

    inline bool isCharOctDigit(char c)
    {
        return (c >= '0' && c <= '7');
    }

    inline bool isCharBinDigit(char c)
    {
        return (c == '0' || c == '1');
    }

    inline bool isCharPunctuation(char c)
    {
        return ((c >= 0x21 && c <= 0x2F) || (c >= 0x3A && c <= 0x40) ||
                (c >= 0x5B && c <= 0x60) || (c >= 0x7B && c <= 0x7E));
    }

    /**
     * Is char whitespace
     * @param  c Char to check
     * @return   true = whitespace
     */
    inline bool isCharWhitespace(char c)
    {
        return ((c >= 0x09 && c <= 0x0D) || c == 0x20);
    }

    inline bool isCharControlCharacter(char c)
    {
        return (isCharWhitespace(c) || (c >= 0x0E && c <= 0x1F) || c == 0x7F);
    }

    inline bool isValidIdentifierBeginningChar(char c)
    {
        return (isCharAlpha(c) || c == '_');
    }

    inline bool isValidIdentifierChar(char c)
    {
        return (isCharAlnum(c) || c == '_');
    }

    inline bool isValidStringChar(char c)
    {
        return (isCharAlnum(c) || isCharPunctuation(c) || isCharWhitespace(c) ||
#if CHAR_SIGNED
                c < 0);
#else
                c > 127);
#endif
    }

    inline std::string createStringWithLength(const std::string& s, size_t len)
    {
        auto it = s.begin();
        std::string ret(len, '\0');
        std::generate(ret.begin(), ret.end(), [&]() {
            if(it == s.end())
            {
                it = s.begin();
            }
            return *it++;
        });
        return ret;
    }
    inline std::string createStringWithLength(char c, size_t len)
    {
        return std::string(len, c);
    }
    inline std::string createEmptyStringWithLength(size_t len)
    {
        return createStringWithLength(' ', len);
    }

    template <typename ForwardIterator>
    std::string join(ForwardIterator begin, ForwardIterator end, char glue)
    {
        if(begin == end)
        {
            return {};
        }
        std::stringstream ss;
        std::for_each(begin, end, [&](const auto& p) { ss << p << glue; });
        std::string str = ss.str();
        str.pop_back();
        return str;
    }

    template <typename Container>
    std::string join(const Container& parts, char glue)
    {
        return join(parts.begin(), parts.end(), glue);
    }
} // namespace stringutils
} // namespace util
