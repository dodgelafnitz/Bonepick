cmake_minimum_required(VERSION 3.15.0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Bonepick C CXX)

add_subdirectory(external)
add_subdirectory(engine)
add_subdirectory(projects)
add_subdirectory(test)
