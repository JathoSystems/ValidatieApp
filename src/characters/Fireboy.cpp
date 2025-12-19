#include "characters/Fireboy.hpp"

#include "SpawnEvent.hpp"
#include "GameObjects/ObjectRegistry.hpp"

Fireboy::Fireboy(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                 bool active) : BaseCharacter(network, eventManager, engine, active, getDefaultBindings()) {
    std::cout << "FIREBOY IS " << (active ? "ACTIVE" : "INACTIVE") << std::endl;

    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");

    if (network && network->getMiddleware()) {
        network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "fireboy"));
    }
    getTransform()->getPosition()->setX(200);
}

Fireboy::Fireboy(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
    bool active): BaseCharacter(parentId, network, eventManager, engine, active, getDefaultBindings()) {
    std::cout << "FIREBOY IS " << (active ? "ACTIVE" : "INACTIVE") << std::endl;

    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");
    getTransform()->getPosition()->setX(200);
}
