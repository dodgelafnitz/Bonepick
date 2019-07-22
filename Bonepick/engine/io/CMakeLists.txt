add_subdirectory(common)

add_library(bonepick_io_common STATIC)

target_sources(bonepick_io_common
PUBLIC
  Graphics.h
  Input.h