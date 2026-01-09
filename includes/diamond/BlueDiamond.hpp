//
// Created by jusra on 6-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_BLUEDIAMOND_HPP
#define VUURJONGEN_WATERMEISJE_GAME_BLUEDIAMOND_HPP
#include "grid/LevelGrid.h"
#include "characters/Fireboy.hpp"
#include "GameObjects/GameObject.h"

class BlueDiamond : public GameObject {
private:
    LevelGrid* _grid;
    int _x;
    int _y;
    bool _collected;

public:
    BlueDiamond(LevelGrid* grid, int x, int y);

    void checkCollisionWith(GameObject *other);

    void onCollisionEnter(const CollisionData &collision) override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_BLUEDIAMOND_HPP