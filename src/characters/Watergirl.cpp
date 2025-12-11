
//
// Created by jusra on 5-12-2025.
//

#include "characters/Watergirl.hpp"

#include "characters/movement/MovementComponent.hpp"
#include "Engine/GameEngine.h"
#include "GameObjects/Spritesheet/Animator.h"

Watergirl::Watergirl(GameEngine *engine, bool active): BaseCharacter(engine, active){
    setFallingSpritesheet(R"(../resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(../resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(../resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(../resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(../resources/watergirl/walk-right.png)");

    getTransform()->getPosition()->setY(100);
}
