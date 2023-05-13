cmake_minimum_required(VERSION 3.24)
project(BarrelDistortion)

set(CMAKE_CXX_STANDARD 23)

add_executable(BarrelDistortion src/main.cpp src/Application.cpp)

find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(BarrelDistortion PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

find_package(Armadillo REQUIRED)
target_include_directories(BarrelDistortion PRIVATE ${ARMADILLO_INCLUDE_DIRS})
target_link_libraries(BarrelDistortion PRIVATE ${ARMADILLO_LIBRARIES})