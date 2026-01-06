//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_REDDIAMOND_HPP
#define VUURJONGEN_WATERMEISJE_GAME_REDDIAMOND_HPP
#include "LevelGrid.h"
#include "characters/Fireboy.hpp"
#include "GameObjects/GameObject.h"

class RedDiamond : public GameObject {
private:
    LevelGrid* _grid;
    int _x;
    int _y;

public:
    RedDiamond(LevelGrid* grid, int x, int y);

    void checkCollisionWith(GameObject *other);

    void onCollisionEnter(const CollisionData &collision) override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_REDDIAMOND_HPP