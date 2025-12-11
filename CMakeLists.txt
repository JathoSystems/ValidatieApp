cmake_minimum_required(VERSION 3.16)
project(Vuurjongen_Watermeisje_Server LANGUAGES CXX)
project(Vuurjongen_Watermeisje_Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# cmp0077
cmake_policy(SET CMP0077 NEW)

add_executable(Vuurjongen_Watermeisje_Server src/Server.cpp
        includes/characters/Fireboy.hpp
        src/characters/Fireboy.cpp
        includes/characters/movement/MovementComponent.hpp
        src/characters/Watergirl.cpp
        includes/characters/Watergirl.hpp
        includes/characters/events/MoveEvent.hpp
        src/characters/events/MoveEvent.cpp
        src/characters/movement/MovementComponent.cpp
        includes/characters/input/MovementInputHandler.hpp
        src/characters/input/MovementInputHandler.cpp
        includes/characters/BaseCharacter.hpp
        src/characters/BaseCharacter.cpp
        includes/characters/events/JumpEvent.h
        src/characters/events/JumpEvent.cpp
)

add_executable(Vuurjongen_Watermeisje_Game src/Main.cpp
        includes/characters/Fireboy.hpp
        src/characters/Fireboy.cpp
        includes/characters/movement/MovementComponent.hpp
        src/characters/Watergirl.cpp
        includes/characters/Watergirl.hpp
        includes/characters/events/MoveEvent.hpp
        src/characters/events/MoveEvent.cpp
        src/characters/movement/MovementComponent.cpp
        includes/characters/input/MovementInputHandler.hpp
        src/characters/input/MovementInputHandler.cpp
        includes/characters/BaseCharacter.hpp
        src/characters/BaseCharacter.cpp
        includes/characters/events/JumpEvent.h
        src/characters/events/JumpEvent.cpp
)

include_directories("includes")

# Link gameengine library
add_subdirectory(external/GameEngine)

# Voor Game executable
target_link_libraries(Vuurjongen_Watermeisje_Game PRIVATE Engine)
target_include_directories(Vuurjongen_Watermeisje_Game PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/GameEngine/includes
        ${CMAKE_CURRENT_SOURCE_DIR}/external/GameEngine/src
)

# Voor Server executable
target_link_libraries(Vuurjongen_Watermeisje_Server PRIVATE Engine)
target_include_directories(Vuurjongen_Watermeisje_Server PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/GameEngine/includes
        ${CMAKE_CURRENT_SOURCE_DIR}/external/GameEngine/src
)

# Kopieer alle benodigde DLLs naar Game executable
add_custom_command(TARGET Vuurjongen_Watermeisje_Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE:box2d>
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje_Game>
)

# Kopieer alle benodigde DLLs naar Server executable
add_custom_command(TARGET Vuurjongen_Watermeisje_Server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:box2d>
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje_Server>
)

# Kopieer resources naar Game
add_custom_command(TARGET Vuurjongen_Watermeisje_Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje_Game>/resources
)