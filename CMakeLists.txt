cmake_minimum_required(VERSION 3.22)
project(GUI)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB_RECURSE MainSources src/*.cpp)
FILE(GLOB IMGUI_SOURCES vendor/imgui/*.cpp)
list(APPEND IMGUI_SOURCES vendor/imgui/backends/imgui_impl_win32.cpp vendor/imgui/backends/imgui_impl_opengl3.cpp)

add_executable(${PROJECT_NAME} WIN32 ${MainSources} ${IMGUI_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_subdirectory(vendor/glm)

add_definitions(-DGLEW_STATIC)
add_definitions(-DDEBUG -DIMGUI)

target_include_directories(${PROJECT_NAME}
        PUBLIC ./vendor/glew/include
        PUBLIC ./vendor/stb
        PUBLIC ./vendor/glm
        PUBLIC ./vendor/imgui)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/lib
        PRIVATE ${CMAKE_BINARY_DIR}/bin)

execute_process(COMMAND mkdir -p ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(${PROJECT_NAME} d2d1 opengl32 Ws2_32 ole32 comctl32 gdi32 comdlg32 uuid dwmapi xinput dbghelp)
