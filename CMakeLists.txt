cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

find_package(SDL REQUIRED)
find_package(GLM REQUIRED)
message(${SDL_INCLUDE_DIR})

include_directories(SDL_INCLUDE_DIR)
include_directories("/usr/include/SDL")
include_directories(GLM_INCLUDE_DIR)

add_executable(main_raytracer Source/raytracer.cpp)

target_link_libraries(main_raytracer ${SDL_LIBRARY})