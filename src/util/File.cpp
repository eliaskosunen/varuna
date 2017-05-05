// Copyright (C) 2016-2017 Elias Kosunen
// This file is distributed under the 3-Clause BSD License
// See LICENSE for details

#include "util/File.h"
#include "util/Compatibility.h"
#include "util/Logger.h"
#include "util/StringUtils.h"
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
    // Allow file reading only once
    assert(content.empty());
    assert(!contentValid);

    try
    {
        // _readFile throws on error
        content = _readFile(filename);
    }
    catch(const std::exception& e)
    {
        util::logger->debug(e.what());
        return false;
    }
    contentValid = true;
    return true;
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

        util::logger->error("Error reading file '{}': Error #{} (libc++): {}",
                            fname, e.code().value(), e.code().message());
    }
#else
    catch(const std::ios_base::failure&)
    {
        std::string errmsg;

#if VARUNA_MSVC
        // Use strerror_s on MSVC
        const auto bufsize = 80u;
        char buf[bufsize];
        strerror_s<bufsize>(buf, errno);
        errmsg = util::stringutils::cstrToString(buf);
#else
        errmsg = strerror(errno);
#endif // VARUNA_MSVC

        util::logger->error("Error reading file '{}': Error #{} (libc): {}",
                            fname, errno, errmsg);
    }
#endif // HAS_IOS_BASE_FAILURE_DERIVED_FROM_SYSTEM_ERROR
    throw std::runtime_error("File reading failed");
}
} // namespace util
