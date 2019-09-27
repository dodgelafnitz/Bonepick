add_library(stb INTERFACE)

target_include_directories(stb
INTERFACE
  .
)

target_sources(stb
INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image.h
)

set_target_properties(stb PROPERTIES INTERFACE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})