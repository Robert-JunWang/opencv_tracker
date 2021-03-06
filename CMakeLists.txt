cmake_minimum_required(VERSION 2.8)

set(the_description "OpenCV Tracking API")
project(ocv_tracker)

set(CMAKE_INSTALL_PREFIX $ENV{HOME})
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Ofast -Wfatal-errors")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#add opencv
FIND_PACKAGE(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARIES_DIRS})

AUX_SOURCE_DIRECTORY(src SOURCES) 
add_library(ocv_tracker ${SOURCES})
add_executable(ocv_demo "demo/main.cpp")
target_link_libraries(ocv_demo ocv_tracker  ${OpenCV_LIBS})

install(TARGETS ocv_tracker 
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
)

install(TARGETS ocv_demo DESTINATION bin)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/opencv2"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.hpp"
)
