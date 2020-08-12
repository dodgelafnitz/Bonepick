add_executable(Malator)

set_property(TARGET Malator PROPERTY FOLDER projects/dieia)
set_property(TARGET Malator PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET Malator PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_features(Malator PUBLIC cxx_std_17)

target_link_libraries(Malator
PRIVATE
  BonepickEngine
)

target_sources(Malator
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)
add_interface_source_group(BonepickEngine engine)
