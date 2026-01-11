#ifndef VUURJONGEN_WATERMEISJE_GAME_LIQUID_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LIQUID_HPP
#include <string>
#include "GameObjects/GameObject.h"

class LevelGrid;

class Liquid : public GameObject {
private:
    std::string _leftSprite;
    std::string _rightSprite;
    std::string _middleSprite;

public:
    Liquid(LevelGrid *grid, int x, int y, std::string left, std::string right, std::string middle);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LIQUID_HPP
