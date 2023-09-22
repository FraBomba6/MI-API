cmake_minimum_required(VERSION 3.8)
include(FindPackageHandleStandardArgs)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
project(small_project)

set(CMAKE_CXX_STANDARD 23)

add_library(gkov src/gkov.cpp include/gkov.h)
add_library(hist src/hist.cpp include/hist.h)
add_library(utils src/utils.cpp include/utils.h)
add_library(simulator src/simulator.cpp include/simulator.h)
target_link_libraries(simulator utils)

add_executable(simulation test/simulate.cpp)
target_link_libraries(simulation simulator utils)
add_executable(attack cluster/attack_cluster.cpp)
target_link_libraries(attack gkov hist simulator utils)

find_package(MLPACK REQUIRED)
include_directories(${MLPACK_INCLUDE_DIRS})
find_package(Armadillo REQUIRED)
target_link_libraries(gkov ${MLPACK_LIBRARIES})
target_link_libraries(gkov ${ARMADILLO_LIBRARIES})

find_package(GSL REQUIRED)
target_link_libraries(hist GSL::gsl GSL::gslcblas)

find_package(HDF5 REQUIRED COMPONENTS C CXX HL)
include_directories (${HDF5_INCLUDE_DIR})
target_link_libraries(utils ${HDF5_LIBRARIES})