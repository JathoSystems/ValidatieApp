cmake_minimum_required(VERSION 3.16)
project(Vuurjongen_Watermeisje LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

cmake_policy(SET CMP0077 NEW)

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set(SKIP_INSTALL_ALL TRUE)

set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)
set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
set(PLUTOSVG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(PLUTOSVG_INSTALL OFF CACHE BOOL "" FORCE)
set(PLUTOVG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(PLUTOVG_INSTALL OFF CACHE BOOL "" FORCE)

macro(install)
endmacro()

macro(export)
endmacro()

add_executable(Vuurjongen_Watermeisje
        src/Main.cpp
        src/LevelGrid.cpp
        src/LevelGrid.h
        src/GridCell.h
)

add_subdirectory(external/GameEngine)
target_link_libraries(Vuurjongen_Watermeisje PRIVATE Engine)
target_include_directories(Vuurjongen_Watermeisje PUBLIC
        External/GameEngine/src
        External/GameEngine/includes
)

add_custom_command(TARGET Vuurjongen_Watermeisje POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE:box2d>
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje>
)

add_custom_command(TARGET Vuurjongen_Watermeisje POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje>/resources
)
