//
// Created by jusra on 5-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_WATERGIRL_HPP
#define VUURJONGEN_WATERMEISJE_WATERGIRL_HPP
#include "BaseCharacter.hpp"
#include "Engine/GameEngine.h"
#include "GameObjects/GameObject.h"

class Watergirl : public BaseCharacter {
public:
    Watergirl(GameEngine *engine, bool active = false);
};

#endif //VUURJONGEN_WATERMEISJE_WATERGIRL_HPP