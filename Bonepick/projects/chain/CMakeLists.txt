add_executable(Chain)

target_link_libraries(Chain
PRIVATE
  BonepickEngine
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)

target_sources(Chain
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)
