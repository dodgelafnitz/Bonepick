add_library(Eigen INTERFACE)

target_include_directories(Eigen
INTERFACE
  .
)

file(GLOB_RECURSE eigen_files "*")

target_sources(Eigen
INTERFACE
  ${eigen_files}
)

set_target_properties(Eigen PROPERTIES INTERFACE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
