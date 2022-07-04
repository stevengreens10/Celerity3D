cmake_minimum_required(VERSION 3.22)
project(GUI)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB_RECURSE MainSources src/*.cpp)
FILE(GLOB IMGUI_SOURCES vendor/imgui/*.cpp)
list(APPEND IMGUI_SOURCES vendor/imgui/backends/imgui_impl_win32.cpp vendor/imgui/backends/imgui_impl_opengl3.cpp)

list(REMOVE_ITEM MainSources src/main.cpp)

add_executable(${PROJECT_NAME} ${MainSources} ${IMGUI_SOURCES} src/graphics/Renderable.h src/graphics/Cube.cpp src/graphics/Cube.h)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)


add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glew/build/cmake)
add_subdirectory(vendor/glm)

add_definitions(-DGLEW_STATIC)

target_include_directories(${PROJECT_NAME}
        PUBLIC ./vendor/glfw/include
        PUBLIC ./vendor/glfw/deps
        PUBLIC ./vendor/glew/include
        PUBLIC ./vendor/stb
        PUBLIC ./vendor/glm
        PUBLIC ./vendor/imgui)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/glfw/src
        PRIVATE ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(${PROJECT_NAME} glfw3 libglew32d.a d2d1 opengl32 Ws2_32 ole32 comctl32 gdi32 comdlg32 uuid dwmapi xinput)