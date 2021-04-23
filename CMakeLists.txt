cmake_minimum_required(VERSION 3.19)
project(fps)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

add_executable(fps src/main.cpp)
target_link_libraries(fps
        glfw
        ${GLFW_LIBRARIES}
)
