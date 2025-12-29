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

    // Set spawn position FIRST
    getTransform()->getPosition()->setX(200);
    getTransform()->getPosition()->setY(500);

    // Then send spawn event with EXACT position
    if (network && network->getMiddleware()) {
        float x = getTransform()->getPosition()->getX();
        float y = getTransform()->getPosition()->getY();

        std::cout << "[Fireboy] Sending spawn event: ID=" << getId()
                  << " Pos=(" << x << ", " << y << ")" << std::endl;

        network->getMiddleware()->sendEvent(
            std::make_shared<SpawnEvent>(getId(), "fireboy", x, y)
        );
    }
}

// Remote constructor - DON'T hardcode position, it comes from SpawnEvent
Fireboy::Fireboy(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
    bool active): BaseCharacter(parentId, network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/fireboy/dropping.png)");
    setJumpingSpritesheet(R"(resources/fireboy/jumping.png)");
    setIdleSpritesheet(R"(resources/fireboy/idle.png)");
    setMovingLeftSpritesheet(R"(resources/fireboy/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/fireboy/walk-right.png)");

    // NOTE: Position is set by SpawnEvent.spawn() BEFORE this object is added to scene
    // Do NOT set position here or it will override the network position!
    std::cout << "[Fireboy] Remote character created, position managed by SpawnEvent" << std::endl;
}