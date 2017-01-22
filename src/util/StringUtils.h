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
     * Convert a C string (const char*) to a C++ string (string_t)
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
     * Convert a C string (const char*) to a C++ string (string_t) with length
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

    /**
     * Split a string to a vector
     * @param s     String to split
     * @param delim Delimeter
     * @param elems Element vector
     * @author Evan Teran:
     * http://stackoverflow.com/questions/236129/split-a-string-in-c
     */
    inline void split(const string_t& s, char_t delim,
                      std::vector<string_t>& elems)
    {
        stringstream_t ss;
        ss.str(s);
        string_t item;
        while(std::getline(ss, item, delim))
        {
            elems.push_back(item);
        }
    }

    /**
     * Split a string to a vector
     * @param  s     String to split
     * @param  delim Delimeter
     * @return       Element vector
     * @author Evan Teran:
     * http://stackoverflow.com/questions/236129/split-a-string-in-c
     */
    inline std::vector<string_t> split(const string_t& s, char_t delim)
    {
        std::vector<string_t> elems;
        split(s, delim, elems);
        return elems;
    }

    inline bool isCharAlpha(char_t c)
    {
        return std::isalpha(c) != 0;
    }

    inline bool isCharAlnum(char_t c)
    {
        return std::isalnum(c) != 0;
    }

    inline bool isCharDigit(char_t c)
    {
        return std::isdigit(c) != 0;
    }

    inline bool isCharHexDigit(char_t c)
    {
        return std::isxdigit(c) != 0;
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
        return std::ispunct(c) != 0;
    }

    /**
     * Is char whitespace
     * @param  c Char to check
     * @return   true = whitespace
     */
    inline bool isCharWhitespace(char_t c)
    {
        return std::isspace(c) != 0;
    }

    inline bool isCharControlCharacter(char_t c)
    {
        return std::iscntrl(c) != 0;
    }
} // namespace StringUtils
} // namespace util
