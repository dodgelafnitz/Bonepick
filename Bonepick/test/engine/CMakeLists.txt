add_library(BonepickEngineTests SHARED)

set_property(TARGET Chain PROPERTY FOLDER projects)
set_property(TARGET Chain PROPERTY CMAKE_CXX_STANDARD 17)
set_property(TARGET Chain PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_features(Chain PUBLIC cxx_std_17)

add_test(NAME test COMMAND NA)

target_link_libraries(BonepickEngineTests
PRIVATE
  BonepickEngine
)


set(inc
  ${CMAKE_CURRENT_SOURCE_DIR}/TestSystems.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TestUtility.h
)

set(src
  ${CMAKE_CURRENT_SOURCE_DIR}/TestSystems.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TestUtility.cpp
)

target_sources(BonepickEngineTests
PUBLIC
  ${inc}
PRIVATE
  ${src}
)

target_include_directories(BonepickEngineTests
PUBLIC
  .
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX inc FILES ${inc})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${src})
add_interface_source_group(BonepickEngine engine)
