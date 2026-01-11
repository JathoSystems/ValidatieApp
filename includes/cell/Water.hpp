#ifndef VUURJONGEN_WATERMEISJE_GAME_WATER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_WATER_HPP
#include "Liquid.hpp"

class Water : public Liquid {
public:
    Water(LevelGrid *grid, int x, int y);

    void onCollisionEnter(const CollisionData &collision) override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_WATER_HPP
