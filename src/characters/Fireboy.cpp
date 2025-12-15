//
// Created by jusra on 5-12-2025.
//

#include "characters/Fireboy.hpp"
#include "characters/events/MoveEvent.hpp"
#include "Events/EventRegistry.h"

Fireboy::Fireboy(EventManager* eventManager, GameEngine *engine, bool active)
    : BaseCharacter(eventManager, engine, active, ControlScheme::ARROWS) {
    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");
}