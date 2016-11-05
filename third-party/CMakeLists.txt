file(GLOB_RECURSE sources_thirdparty	src/*.cpp)
file(GLOB_RECURSE headers_thirdparty	include/*.h include/*.hpp include-private/*.h include-private/*.hpp)

add_library(thirdparty ${sources_thirdparty} ${headers_thirdparty})
target_include_directories(thirdparty PRIVATE src)
