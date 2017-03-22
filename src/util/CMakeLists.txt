file(GLOB sources_util *.cpp)
file(GLOB headers_util *.h)

add_library(util ${sources_util} ${headers_util})
target_link_libraries(util binreloc guid)
if(WIN32)
    target_link_libraries(util PathCch)
endif()
