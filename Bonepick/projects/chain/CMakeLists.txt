add_executable(Chain)

set_property(TARGET Chain PROPERTY FOLDER projects)
set_property(TARGET Chain PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET Chain PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_features(Chain PUBLIC cxx_std_17)

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
