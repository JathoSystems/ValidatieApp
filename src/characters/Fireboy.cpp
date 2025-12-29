#include "characters/Fireboy.hpp"

#include "SpawnEvent.hpp"
#include "GameObjects/ObjectRegistry.hpp"

// Standard Constructor (auto-generated ID)
Fireboy::Fireboy(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                 bool active) : BaseCharacter(network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");

    getTransform()->getPosition()->setX(200);
    getTransform()->getPosition()->setY(500);

    if (network && network->getMiddleware()) {
        float x = getTransform()->getPosition()->getX();
        float y = getTransform()->getPosition()->getY();
        network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "fireboy", x, y));
    }
}

// Fixed-ID Constructor (Uses ID 99 from LevelScene)
Fireboy::Fireboy(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
    bool active): BaseCharacter(parentId, network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");

    // FIX: Ensure both X and Y are set
    getTransform()->getPosition()->setX(200);
    getTransform()->getPosition()->setY(500);

    // FIX: Send SpawnEvent even if using fixed ID (99)
    // This confirms to the other player that this ID is active
    if (active && network && network->getMiddleware()) {
        float x = getTransform()->getPosition()->getX();
        float y = getTransform()->getPosition()->getY();
        network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "fireboy", x, y));
    }
}