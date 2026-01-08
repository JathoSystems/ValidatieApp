//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LAVA_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LAVA_HPP
#include "Liquid.hpp"

class Lava : public Liquid {
public:
    Lava(LevelGrid* grid, int x, int y);

    void onCollisionEnter(const CollisionData &collision) override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LAVA_HPP