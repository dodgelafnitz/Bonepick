add_library(stb INTERFACE)

target_include_directories(stb
INTERFACE
  .
)

target_sources(stb
INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX stb FILES ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image.h)
