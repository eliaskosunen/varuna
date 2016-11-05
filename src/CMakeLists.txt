file(GLOB_RECURSE sources	app/*.cpp core/*.cpp util/*.cpp)
file(GLOB_RECURSE headers	app/*.h core/*.h util/*.h)

add_library(varunasrc ${sources} ${headers})
add_executable(varuna Main.cpp)
target_link_libraries(varuna thirdparty varunasrc)
