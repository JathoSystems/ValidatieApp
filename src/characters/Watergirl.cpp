#include "characters/Watergirl.hpp"

#include "SpawnEvent.hpp"
#include "Engine/GameEngine.h"
#include "GameObjects/Spritesheet/Animator.h"

Watergirl::Watergirl(std::shared_ptr<NetworkSystem> network, EventManager* eventManager, GameEngine *engine, bool active)
    : BaseCharacter(network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/watergirl/walk-right.png)");

    // Set spawn position BEFORE sending spawn event
    getTransform()->getPosition()->setX(600);
    getTransform()->getPosition()->setY(500); // Add Y position too!

    if (network && network->getMiddleware()) {
        // Send spawn event with current position
        float x = getTransform()->getPosition()->getX();
        float y = getTransform()->getPosition()->getY();
        network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "watergirl", x, y));
    }
}

Watergirl::Watergirl(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager,
    GameEngine *engine, bool active)
    : BaseCharacter(parentId, network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/watergirl/walk-right.png)");
}