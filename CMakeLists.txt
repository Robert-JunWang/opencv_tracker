set(the_description "OpenCV Tracking API")

cmake_minimum_required(VERSION 2.8)
project(ocv_tracker)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Ofast -Wfatal-errors")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#add opencv
FIND_PACKAGE(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARIES_DIRS})

AUX_SOURCE_DIRECTORY(src SOURCES) 

add_library(ocv_tracker ${SOURCES})

install(TARGETS ocv_tracker DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/)
