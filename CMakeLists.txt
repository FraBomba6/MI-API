cmake_minimum_required(VERSION 3.8)
include(FindPackageHandleStandardArgs)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
project(small_project)

set(CMAKE_CXX_STANDARD 23)

add_library(small_project gkov.cpp gkov.h)
add_executable(test test.cpp)
target_link_libraries(test small_project)

find_package(MLPACK REQUIRED)
include_directories(${MLPACK_INCLUDE_DIRS})
find_package(Armadillo REQUIRED)
target_link_libraries(small_project ${MLPACK_LIBRARIES})
target_link_libraries(small_project ${ARMADILLO_LIBRARIES})
