
//
// Created by jusra on 5-12-2025.
//

#include "characters/Watergirl.hpp"

#include "Engine/GameEngine.h"
#include "GameObjects/Spritesheet/Animator.h"

Watergirl::Watergirl(std::shared_ptr<NetworkSystem> network, EventManager* eventManager, GameEngine *engine, bool active): BaseCharacter(network, eventManager, engine, active){
    setFallingSpritesheet(R"(resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/watergirl/walk-right.png)");

    getTransform()->getPosition()->setY(100);
}
