cmake_minimum_required(VERSION 3.18)
project(GUI)

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB_RECURSE MainSources src/*.cpp)
FILE(GLOB IMGUI_SOURCES vendor/imgui/*.cpp)
list(APPEND IMGUI_SOURCES vendor/imgui/backends/imgui_impl_win32.cpp vendor/imgui/backends/imgui_impl_opengl3.cpp)

add_executable(${PROJECT_NAME} WIN32 ${MainSources} ${IMGUI_SOURCES} ./vendor/glew/src/glew.c)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_subdirectory(vendor/glm)

add_definitions(-DGLEW_STATIC)
add_definitions(-DIMGUI)

target_include_directories(${PROJECT_NAME}
        PUBLIC ./vendor/glew/include
        PUBLIC ./vendor/stb
        PUBLIC ./vendor/glm
        PUBLIC ./vendor/imgui
        PUBLIC vendor/PhysX/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/lib
        PRIVATE ${CMAKE_BINARY_DIR}/bin
        PRIVATE ./vendor/physx/lib)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W0)
endif()


file(GLOB PhysX_DLLs
        "./vendor/PhysX/lib/*.dll"
        )

foreach(DLL IN LISTS PhysX_DLLs)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL}
        ${CMAKE_CURRENT_BINARY_DIR} )
endforeach()

target_link_libraries(${PROJECT_NAME} d2d1 opengl32 Ws2_32 ole32 comctl32 gdi32 comdlg32 uuid dwmapi xinput dbghelp
        PhysX_64 PhysXCommon_64 PhysXFoundation_64 PVDRuntime_64 PhysXExtensions_static_64)


