add_library(guid guid/guid.cpp guid/guid.h)

if(WIN32)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        target_compile_options(guid PRIVATE /DGUID_WINDOWS=1)
        target_compile_options(guid PRIVATE /w)
    else()
        target_compile_options(guid PRIVATE -DGUID_WINDOWS=1)
        target_compile_options(guid PRIVATE -w)
    endif()
elseif(APPLE)
    macro(add_osx_framework target fwname)
        find_library(FRAMEWORK_${fwname}
        NAMES ${fwname}
        PATHS ${CMAKE_OSX_SYSROOT}/System/Library
        PATH_SUFFIXES Frameworks
        NO_DEFAULT_PATH)
        if(${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
            message(ERROR "Framework ${fwname} not found")
        else()
            target_link_libraries(${target} "${FRAMEWORK_${fwname}}/${fwname}")
            message(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
        endif()
    endmacro(add_osx_framework)

    target_compile_options(guid PRIVATE -DGUID_CFUUID=1)
    target_compile_options(guid PRIVATE -w)
    add_osx_framework(guid Foundation)
    add_osx_framework(guid CoreFoundation)
else()
    target_compile_options(guid PRIVATE -DGUID_LIBUUID=1)
    target_compile_options(guid PRIVATE -w)
    target_link_libraries(guid uuid)
endif()
