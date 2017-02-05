// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#pragma once

#include "util/String.h"
#include <algorithm>
#include <cctype>
#include <functional>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace util
{
/**
 * String utility functions
 */
namespace StringUtils
{
    /**
     * Replace all occurences of needle with replacement in haystack
     * @param haystack    String to search in
     * @param needle      String to replace
     * @param replacement Replacement string
     */
    inline void replaceAll(string_t& haystack, const string_t& needle,
                           const string_t& replacement)
    {
        size_t start_pos = 0;
        while((start_pos = haystack.find(needle, start_pos)) != string_t::npos)
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
    inline string_t replaceAllCopy(string_t haystack, const string_t& needle,
                                   const string_t& replacement)
    {
        replaceAll(haystack, needle, replacement);
        return haystack;
    }

    /**
     * Trim off whitespace from the beginning of the string
     * @param str String to trim
     */
    inline void ltrim(string_t& str)
    {
        str.erase(
            str.begin(),
            std::find_if(str.begin(), str.end(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))));
    }

    /**
     * Trim off whitespace from the end of the string
     * @param str String to trim
     */
    inline void rtrim(string_t& str)
    {
        str.erase(std::find_if(str.rbegin(), str.rend(),
                               std::not1(std::ptr_fun<int, int>(std::isspace)))
                      .base(),
                  str.end());
    }

    /**
     * Trim off whitespace from both sides of the string
     * @param str String to trim
     */
    inline void trim(string_t& str)
    {
        ltrim(str);
        rtrim(str);
    }

    /**
     * Trim off whitespace from the beginning of the string and return a copy
     * @param  str String to trim
     * @return     Trimmed string
     */
    inline string_t ltrimCopy(string_t str)
    {
        ltrim(str);
        return str;
    }

    /**
     * Trim off whitespace from the end of the string and return a copy
     * @param  str String to trim
     * @return     Trimmed string
     */
    inline string_t rtrimCopy(string_t str)
    {
        rtrim(str);
        return str;
    }

    /**
     * Trim off whitespace from both sides of the string and return a copy
     * @param  str String to trim
     * @return     Trimmed string
     */
    inline string_t trimCopy(string_t str)
    {
        trim(str);
        return str;
    }

    /**
     * Trim off all consecutive spaces from a string
     * @param str String to trim
     */
    inline void trimConsecutiveSpaces(string_t& str)
    {
        string_t::iterator newEnd =
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
    inline string_t trimConsecutiveSpacesCopy(string_t str)
    {
        trimConsecutiveSpaces(str);
        return str;
    }

    /**
     * Convert a C string (const char_t*) to a C++ string (string_t)
     * @param  cstr C string to convert
     * @return      Equivalent C++ string
     */
    inline string_t cstrToString(const char_t* cstr)
    {
        stringstream_t ss;
        ss << cstr;
        return ss.str();
    }

    /**
     * Convert a C string (const char_t*) to a C++ string (string_t) with length
     * information
     * @param  cstr C string to convert
     * @param  len  Length of the C string
     * @return      Equivalent C++ string
     */
    inline string_t cstrToStringLen(const char_t* cstr, size_t len)
    {
        return string_t(cstr, len);
    }

    /**
     * Convert a char to C++ string
     * @param  c Char to convert
     * @return   Equivalent C++ string
     */
    inline string_t charToString(const char_t& c)
    {
        stringstream_t ss;
        ss << c;
        return ss.str();
    }

    inline void split(const char* str, char_t delim,
                      std::vector<string_t>& result)
    {
        do
        {
            const char* begin = str;

            while(*str != delim && *str)
            {
                str++;
            }

            result.push_back(string_t(begin, str));
        } while(*str++ != 0x0);
    }

    inline std::vector<string_t> split(const char* str, char_t delim)
    {
        std::vector<string_t> result;
        split(str, delim, result);
        return result;
    }

    inline void split(const string_t& str, char_t delim,
                      std::vector<string_t>& result)
    {
        split(str.c_str(), delim, result);
    }

    inline std::vector<string_t> split(const string_t& str, char_t delim)
    {
        return split(str.c_str(), delim);
    }

    inline bool isCharAlpha(char_t c)
    {
        return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
    }

    inline bool isCharDigit(char_t c)
    {
        return (c >= '0' && c <= '9');
    }

    inline bool isCharAlnum(char_t c)
    {
        return (isCharAlpha(c) || isCharDigit(c));
    }

    inline bool isCharHexDigit(char_t c)
    {
        return (isCharDigit(c) || (c >= 'A' && c <= 'F') ||
                (c >= 'a' && c <= 'f'));
    }

    inline bool isCharOctDigit(char_t c)
    {
        return (c >= '0' && c <= '7');
    }

    inline bool isCharBinDigit(char_t c)
    {
        return (c == '0' || c == '1');
    }

    inline bool isCharPunctuation(char_t c)
    {
        return ((c >= 0x21 && c <= 0x2F) || (c >= 0x3A && c <= 0x40) ||
                (c >= 0x5B && c <= 0x60) || (c >= 0x7B && c <= 0x7E));
    }

    /**
     * Is char whitespace
     * @param  c Char to check
     * @return   true = whitespace
     */
    inline bool isCharWhitespace(char_t c)
    {
        return ((c >= 0x09 && c <= 0x0D) || c == 0x20);
    }

    inline bool isCharControlCharacter(char_t c)
    {
        return (isCharWhitespace(c) || (c >= 0x0E && c <= 0x1F) || c == 0x7F);
    }

    inline bool isValidIdentifierBeginningChar(char_t c)
    {
        return (isCharAlpha(c) || c == '_');
    }

    inline bool isValidIdentifierChar(char_t c)
    {
        return (isCharAlnum(c) || c == '_');
    }

    inline bool isValidStringChar(char_t c)
    {
        return (isCharAlnum(c) || isCharPunctuation(c) || isCharWhitespace(c) ||
#if CHAR_SIGNED
                c < 0);
#else
                c > 127);
#endif
    }

    inline std::string createEmptyStringWithLength(size_t len)
    {
        std::string ret;
        ret.reserve(len);
        for(size_t i = 1; i <= len; ++i)
        {
            ret.push_back(' ');
        }
        return ret;
    }
} // namespace StringUtils
} // namespace util
