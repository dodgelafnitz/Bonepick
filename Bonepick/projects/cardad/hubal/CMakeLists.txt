add_executable(Hubal)

set_property(TARGET Hubal PROPERTY FOLDER projects/cardad)
set_property(TARGET Hubal PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET Hubal PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_features(Hubal PUBLIC cxx_std_17)

target_link_libraries(Hubal
PRIVATE
  BonepickEngine
)

target_sources(Hubal
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)
add_interface_source_group(BonepickEngine engine)
