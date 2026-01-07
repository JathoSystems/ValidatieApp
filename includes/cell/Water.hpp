//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_WATER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_WATER_HPP
#include "Liquid.hpp"

class Water : public Liquid {
public:
    Water(LevelGrid* grid, int x, int y);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_WATER_HPP