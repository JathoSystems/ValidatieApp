cmake_minimum_required(VERSION 3.16)
project(Vuurjongen_Watermeisje LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(Vuurjongen_Watermeisje src/Main.cpp
        includes/characters/Fireboy.hpp
        src/characters/Fireboy.cpp
        includes/characters/movement/MovementComponent.hpp
        src/characters/Watergirl.cpp
        includes/characters/Watergirl.hpp
        includes/characters/events/MoveEvent.hpp
        src/characters/events/MoveEvent.cpp
)

include_directories("includes")

# Link gameengine library
add_subdirectory(external/GameEngine)
target_link_libraries(Vuurjongen_Watermeisje PRIVATE Engine)
target_include_directories(Vuurjongen_Watermeisje PUBLIC External/GameEngine/src
        External/GameEngine/includes)

add_custom_command(TARGET Vuurjongen_Watermeisje POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje>
)

add_custom_command(TARGET Vuurjongen_Watermeisje POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje>/resources
)
# cmp0077
cmake_policy( SET CMP0077 NEW )