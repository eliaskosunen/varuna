file(GLOB sources_vart vart.c)

add_library(vart ${sources_vart})
add_dependencies(vart varuna)
install(TARGETS vart DESTINATION lib)
