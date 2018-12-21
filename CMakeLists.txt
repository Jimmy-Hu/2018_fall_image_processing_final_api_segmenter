cmake_minimum_required (VERSION 3.1)											#	set cmake minimum version

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

project(2018_fall_image_processing_final_api_segmenter)

include_directories("${CMAKE_CURRENT_LIST_DIR}/include")

#
# Prepare modules
#

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/submodule/sweet")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/utility.cmake")

# Q. can 'project(projname)' command used before 'include("cmake/HunterGate")' ??
show_variables(CMAKE_SYSTEM_PREFIX_PATH)
show_variables(CMAKE_PREFIX_PATH)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/export/bin")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/export/lib")
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/export/lib")

set(CMAKE_C_STANDARD              90)
set(CMAKE_C_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_STANDARD            14)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)

include_directories("${CMAKE_CURRENT_LIST_DIR}/include")
find_package(OpenCV REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(packages fmt OpenCV)


foreach(package ${packages})
  config_from_generic_package("${package}" "${package}")
  message(STATUS " config: ${package}=${${package}_CONFIG_SETTINGS}")
endforeach()

sweet_config_add(Threads LIBRARIES PRIVATE Threads::Threads)
sweet_config_add(CImg INCLUDES PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/submodule")
sweet_config_add(CImg INCLUDES PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/submodule/CImg")

#
# Targets
#

# add_library(api_segmenter INTERFACE )
add_executable(test_api_segmenter_cv_bridge test/test_api_segmenter_cv_bridge.cpp)
sweet_target_add(test_api_segmenter_cv_bridge CONFIGS OpenCV)

add_executable(test_api_segmenter_cimg_bridge test/test_api_segmenter_cimg_bridge.cpp)
sweet_target_add(test_api_segmenter_cimg_bridge CONFIGS OpenCV CImg Threads)

add_executable(test_api_segmenter_segmenter_cimg test/test_api_segmenter_segmenter_cimg.cpp)
sweet_target_add(test_api_segmenter_segmenter_cimg CONFIGS OpenCV CImg Threads)

add_executable(test_api_segmenter_segmenter_std test/test_api_segmenter_segmenter_std.cpp)
sweet_target_add(test_api_segmenter_segmenter_std CONFIGS OpenCV)
