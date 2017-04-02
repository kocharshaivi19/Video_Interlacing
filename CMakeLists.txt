cmake_minimum_required(VERSION 3.6)
project(test_video)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(BUILD_1 main)
set(SOURCE_FILES_1 main.cpp)
add_executable(${BUILD_1} ${SOURCE_FILES_1})

FIND_PACKAGE( OpenCV REQUIRED )

target_link_libraries(${BUILD_1} ${OpenCV_LIBS})

#
#SET(OpenCV_INSTALL_PATH /usr/local)
#
#SET(OpenCV_INCLUDE_DIRS "${OpenCV_INSTALL_PATH}/include/opencv2;${OpenCV_INSTALL_PATH}/include")
#
#SET(OpenCV_LIB_DIR "${OpenCV_INSTALL_PATH}/lib")
#
#LINK_DIRECTORIES(${OpenCV_LIB_DIR})
#
#set(OpenCV_LIBS opencv_core opencv_imgproc opencv_calib3d opencv_video opencv_features2d opencv_ml opencv_highgui opencv_objdetect opencv_contrib opencv_legacy opencv_gpu)
