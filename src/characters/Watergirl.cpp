#include "characters/Watergirl.hpp"

#include "SpawnEvent.hpp"
#include "Engine/GameEngine.h"
#include "GameObjects/Spritesheet/Animator.h"

// Standard Constructor (auto-generated ID)
Watergirl::Watergirl(std::shared_ptr<NetworkSystem> network, EventManager* eventManager, GameEngine *engine, bool active)
    : BaseCharacter(network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/watergirl/walk-right.png)");

    getTransform()->getPosition()->setX(600);
    getTransform()->getPosition()->setY(500);

    if (network && network->getMiddleware()) {
        float x = getTransform()->getPosition()->getX();
        float y = getTransform()->getPosition()->getY();
        network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "watergirl", x, y));
    }
}

// Fixed-ID Constructor (Uses ID 100 from LevelScene)
Watergirl::Watergirl(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager,
    GameEngine *engine, bool active)
    : BaseCharacter(parentId, network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/watergirl/walk-right.png)");

    // FIX: Added starting position (she was spawning at 0,0 before)
    getTransform()->getPosition()->setX(600);
    getTransform()->getPosition()->setY(500);

    // FIX: Send SpawnEvent so the other player knows ID 100 is connected
    if (active && network && network->getMiddleware()) {
        float x = getTransform()->getPosition()->getX();
        float y = getTransform()->getPosition()->getY();
        network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "watergirl", x, y));
    }
}