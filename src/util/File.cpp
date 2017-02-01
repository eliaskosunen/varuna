// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/File.h"
#include <utf8.h>
#include <fstream>
#include <sstream>

#if defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION >= 1000)
#define HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR 1
#else
#define HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR 0
#endif

namespace util
{
bool File::readFile()
{
    if(!content.empty())
    {
        return true;
    }
    try
    {
        content = _readFile(filename);
    }
    catch(const std::exception& e)
    {
        util::logger->error(e.what());
        return false;
    }
    contentValid = true;
    return true;
}

bool File::checkUtf8()
{
    return utf8::is_valid(content.begin(), content.end());
}

void File::calculateChecksum()
{
    throw std::logic_error(
        "util::FileCache::File::calculateChecksum() not implemented");
}

std::string File::_readFile(const std::string& fname)
{
    try
    {
        std::ifstream s;
        s.exceptions(std::ios::badbit | std::ios::failbit);
        s.open(fname.c_str(), std::ios::binary | std::ios::in);

        if(s.is_open() && s.good())
        {
            std::stringstream ss;
            ss << s.rdbuf();
            s.close();
            return ss.str();
        }

        throw std::runtime_error("File reading failed");
    }
#if HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR
    catch(const std::ios_base::failure& e)
    {
        //
        // e.code() is only available if the lib actually follows iso
        // §27.5.3.1.1
        // and derives ios_base::failure from system_error
        // like e.g. libc++
        // http://llvm.org/viewvc/llvm-project/libcxx/trunk/include/ios?revision=193085&view=markup
        // (line 415)
        //
        // and not keeps on deriving it directly from runtime_error
        // like libstdc++
        // https://github.com/mirrors/gcc/blob/master/libstdc%2B%2B-v3/include/bits/ios_base.h#L209
        //

        util::logger->error("Error reading file: Error #{} (libc++): {}",
                            e.code().value(), e.code().message());
    }
#else
    catch(const std::ios_base::failure&)
    {
        std::string errmsg;
#ifdef _MSC_VER
        char buf[80];
        strerror_s<80>(buf, errno);
        errmsg = util::StringUtils::cstrToString(buf);
#else
        errmsg = strerror(errno);
#endif // defined _MSC_VER

        util::logger->error("Error reading file: Error #{} (libc): {}", errno,
                            errmsg);
    }
#endif
    throw std::runtime_error("File reading failed");
}
} // namespace util
