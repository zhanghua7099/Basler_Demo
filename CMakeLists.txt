cmake_minimum_required(VERSION 2.8)
project( Basler_Demo )

find_package( OpenCV 3.4 REQUIRED )

############
# find pylon
############
find_package(Pylon QUIET)
if (NOT ${Pylon_FOUND})
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindPylon.cmake")
endif()
############
# Note: FindPylon.cmake must be changed.
############
############
# find pylon
############

include_directories( ${OpenCV_INCLUDE_DIRS} ${Pylon_INCLUDE_DIRS})

add_executable( Grab Grab.cpp )
add_executable( multi_grab_v1 multi_grab_v1.cpp)
add_executable( multi_grab_v2 multi_grab_v2.cpp)

target_link_libraries( Grab ${OpenCV_LIBS} ${Pylon_LIBRARIES})
target_link_libraries( multi_grab_v1 ${OpenCV_LIBS} ${Pylon_LIBRARIES})
target_link_libraries( multi_grab_v2 ${OpenCV_LIBS} ${Pylon_LIBRARIES})
