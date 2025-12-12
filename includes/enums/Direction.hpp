//
// Created by jusra on 12-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_DIRECTION_HPP
#define VUURJONGEN_WATERMEISJE_GAME_DIRECTION_HPP

#include <string>

enum class Direction {
    NORTH, EAST, SOUTH, WEST, NONE
};

inline std::string DirectionToString(Direction dir) {
    switch (dir) {
        case Direction::NORTH: return "NORTH";
        case Direction::EAST:  return "EAST";
        case Direction::SOUTH: return "SOUTH";
        case Direction::WEST:  return "WEST";
        case Direction::NONE:  return "NONE";
        default:               return "UNKNOWN";
    }
}

inline int DirectionToInt(Direction dir) {
    return static_cast<int>(dir);
}

inline Direction IntToDirection(int value) {
    if (value >= 0 && value <= 4) {
        return static_cast<Direction>(value);
    }
    return Direction::NONE;
}

#endif //VUURJONGEN_WATERMEISJE_GAME_DIRECTION_HPP