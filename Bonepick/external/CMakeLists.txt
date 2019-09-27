add_subdirectory(SDL2)
add_subdirectory(stb)

macro(add_interface_source_group target_name source_prefix)
  get_property(target_source_dir TARGET ${target_name} PROPERTY INTERFACE_SOURCE_DIR)
  get_property(target_sources TARGET ${target_name} PROPERTY INTERFACE_SOURCES)

  source_group(TREE ${target_source_dir}
    PREFIX ${source_prefix}
    FILES ${target_sources}
  )
endmacro()