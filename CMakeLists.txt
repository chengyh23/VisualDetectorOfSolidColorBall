cmake_minimum_required(VERSION 3.5.0)
project(hikvision_ros)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  cv_bridge
  image_transport
  camera_info_manager
)

find_package(OpenCV 3 REQUIRED)
include_directories(${OpenCV_INCLUDES})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDES})

catkin_package(INCLUDE_DIRS include)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

link_directories(lib)

# ADD LIBRARY
add_library(hik_cam src/hik_cam.cpp)
target_link_libraries(hik_cam
    ${catkin_LIBRARIES}
    ${OpenCV_LIBS}
    hcnetsdk PlayCtrl
)
add_library(kmeans src/kmeans_minibatch.cpp)
target_link_libraries(kmeans
        ${catkin_LIBRARIES}
)

add_library(color src/color.cpp)
target_link_libraries(color
    ${catkin_LIBRARIES}
    ${OpenCV_LIBS}
    kmeans
)

add_library(seaskyline src/seaskyline.cpp)

add_library(shape src/shape.cpp)
target_link_libraries(shape
    ${OpenCV_LIBS}
)

add_library(detect_pipeline src/detect_pipeline.cpp)
target_link_libraries(detect_pipeline
        seaskyline color shape
)
# ADD EXECUTABLE
add_executable(localtest src/localtest.cpp)
target_link_libraries(localtest color kmeans seaskyline shape detect_pipeline)
add_executable(shapetest src/shapetest.cpp)
target_link_libraries(shapetest shape)
add_executable(colortest src/colortest.cpp)
target_link_libraries(colortest color)

add_executable(hik_cam_node node/hik_cam_node.cpp)
target_link_libraries(hik_cam_node hik_cam color kmeans seaskyline)
