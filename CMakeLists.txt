cmake_minimum_required(VERSION 3.16)
project(Vuurjongen_Watermeisje_Server LANGUAGES CXX)
project(Vuurjongen_Watermeisje_Game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# cmp0077
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

add_executable(Vuurjongen_Watermeisje_Server src/server/Server.cpp
        includes/characters/Fireboy.hpp
        src/characters/Fireboy.cpp
        src/characters/Watergirl.cpp
        includes/characters/Watergirl.hpp
        includes/characters/events/MoveEvent.hpp
        src/characters/events/MoveEvent.cpp
        includes/characters/input/MovementInputHandler.hpp
        src/characters/input/MovementInputHandler.cpp
        includes/characters/BaseCharacter.hpp
        src/characters/BaseCharacter.cpp
        includes/characters/events/JumpEvent.h
        src/characters/events/JumpEvent.cpp
        includes/enums/Direction.hpp
        includes/characters/BaseCharacterController.hpp
        src/characters/BaseCharacterController.cpp
        src/server/Player.cpp
        src/server/PlayerManager.cpp
        includes/scenes/Lobby.hpp
        src/scenes/Lobby.cpp
        includes/scenes/Game.hpp
        src/scenes/Game.cpp
        src/server/packet/handler/PlayerAssignPacketHandler.cpp
        includes/server/packet/handler/PlayerAssignPacketHandler.hpp
        src/server/packet/handler/GameReadyPacketHandler.cpp
        includes/server/packet/handler/GameReadyPacketHandler.hpp
)

add_executable(Vuurjongen_Watermeisje_Game src/Main.cpp
        includes/characters/Fireboy.hpp
        src/characters/Fireboy.cpp
        src/characters/Watergirl.cpp
        includes/characters/Watergirl.hpp
        includes/characters/events/MoveEvent.hpp
        src/characters/events/MoveEvent.cpp
        includes/characters/input/MovementInputHandler.hpp
        src/characters/input/MovementInputHandler.cpp
        includes/characters/BaseCharacter.hpp
        src/characters/BaseCharacter.cpp
        includes/characters/events/JumpEvent.h
        src/characters/events/JumpEvent.cpp
        includes/enums/Direction.hpp
        includes/characters/BaseCharacterController.hpp
        src/characters/BaseCharacterController.cpp
        includes/server/Player.hpp
        includes/server/PlayerManager.hpp
        includes/scenes/Lobby.hpp
        src/scenes/Lobby.cpp
        includes/scenes/Game.hpp
        src/scenes/Game.cpp
        includes/server/packet/PlayerAssignPacket.hpp
        includes/server/packet/GameReady.hpp
        src/server/packet/handler/PlayerAssignPacketHandler.cpp
        includes/server/packet/handler/PlayerAssignPacketHandler.hpp
        src/server/packet/handler/GameReadyPacketHandler.cpp
        includes/server/packet/handler/GameReadyPacketHandler.hpp
        src/LevelSelector.cpp
        includes/LevelSelector.h
        src/LevelGrid.cpp
        src/LevelGrid.h
        src/GridCell.h
        src/GridRenderer.cpp
        src/GridRenderer.h
        src/GridManager.cpp
        src/GridManager.h
        includes/scenes/MainMenu.hpp
        src/scenes/MainMenu.cpp
        includes/scenes/LevelScene.hpp
        src/scenes/LevelScene.cpp
)

include_directories("includes")
include_directories("src")

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
        $<TARGET_FILE:box2d>
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE:box2d>
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje_Game>
)

# Kopieer alle benodigde DLLs naar Server executable
add_custom_command(TARGET Vuurjongen_Watermeisje_Server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Engine>
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE:box2d>
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje_Server>
)

# Kopieer resources naar Game
add_custom_command(TARGET Vuurjongen_Watermeisje_Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:Vuurjongen_Watermeisje_Game>/resources
)