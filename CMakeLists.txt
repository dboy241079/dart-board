cmake_minimum_required(VERSION 3.20)
project(DartCameraTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED COMPONENTS core highgui videoio imgproc)

add_executable(DartCameraTest src/main.cpp)
target_link_libraries(DartCameraTest PRIVATE ${OpenCV_LIBS})
target_include_directories(DartCameraTest PRIVATE ${OpenCV_INCLUDE_DIRS})

if(MSVC)
  target_compile_options(DartCameraTest PRIVATE /W4 /permissive-)
endif()
