#include "characters/Fireboy.hpp"

#include "SpawnEvent.hpp"
#include "GameObjects/ObjectRegistry.hpp"

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

Fireboy::Fireboy(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
    bool active): BaseCharacter(parentId, network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");

    getTransform()->getPosition()->setX(200);
    getTransform()->getPosition()->setY(500);

    if (active && network && network->getMiddleware()) {
        float x = getTransform()->getPosition()->getX();
        float y = getTransform()->getPosition()->getY();
        network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "fireboy", x, y));
    }
}