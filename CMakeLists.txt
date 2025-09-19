cmake_minimum_required(VERSION 3.10.0)
project(opencv_project VERSION 0.1.0 LANGUAGES C CXX)


include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_executable(opencv_project main.cpp)

target_link_libraries(opencv_project PRIVATE ${OpenCV_LIBS})


include_directories(${OpenCV_INCLUDE_DIRS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
