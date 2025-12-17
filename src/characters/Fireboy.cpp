//
// Created by jusra on 5-12-2025.
//

#include "characters/Fireboy.hpp"

#include "SpawnEvent.hpp"
#include "GameObjects/ObjectRegistry.hpp"

Fireboy::Fireboy(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                 bool active) : BaseCharacter(Position(160.0f, 0.0f), network, eventManager, engine, active) {
    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");

    network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "fireboy"));
}

Fireboy::Fireboy(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
    bool active): BaseCharacter(parentId, Position(160.0f, 0.0f), network, eventManager, engine, active) {
    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");
}
