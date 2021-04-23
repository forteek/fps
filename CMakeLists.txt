cmake_minimum_required(VERSION 3.19)
project(fps)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
file(GLOB SOURCES
        src/*.c
        src/*.cpp
)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

add_executable(fps ${SOURCES})
target_link_libraries(fps
        glfw
        ${GLFW_LIBRARIES}
)
