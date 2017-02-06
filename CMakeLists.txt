cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

find_package(SDL REQUIRED)
find_package(GLM REQUIRED)

include_directories(SDL_INCLUDE_DIR)
include_directories(GLM_INCLUDE_DIR)

set(SOURCE_FILES Source/raytracer.cpp Source/SDLauxiliary.cpp Source/TestModel.cpp Source/main_raytracer.cpp)
add_executable(main_raytracer Source/main_raytracer.cpp ${SOURCE_FILES})

target_link_libraries(main_raytracer ${SDL_LIBRARY})