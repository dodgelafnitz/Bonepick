add_executable(Sift)

set_property(TARGET Sift PROPERTY FOLDER projects)
set_property(TARGET Sift PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET Sift PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_features(Sift PUBLIC cxx_std_17)

target_link_libraries(Sift
PRIVATE
  BonepickEngine
)

target_sources(Sift
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)
add_interface_source_group(BonepickEngine engine)
