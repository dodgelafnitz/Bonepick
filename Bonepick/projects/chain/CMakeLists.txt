add_executable(Chain)

target_link_libraries(Chain
PRIVATE
  BonepickEngine
)

target_sources(Chain
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)
add_interface_source_group(BonepickEngine engine)
