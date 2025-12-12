#pragma once
#include "GameObjects/GameObject.h"


class LevelBuilder {
private:
    int gridBlockW = 20; // width for 1 block in grid
    int gridBlockH = 20; // height for 1 block in grid

    int windowW;
    int windowH;

    Window* _window;

public:
    std::unique_ptr<GameObject> grid;

    LevelBuilder(Window* window);

    void renderBlock(Position* position);
};
