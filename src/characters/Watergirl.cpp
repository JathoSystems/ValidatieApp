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

    // Set spawn position FIRST
    getTransform()->getPosition()->setX(600);
    getTransform()->getPosition()->setY(500);

    // Then send spawn event with EXACT position
    if (network && network->getMiddleware()) {
        float x = getTransform()->getPosition()->getX();
        float y = getTransform()->getPosition()->getY();

        std::cout << "[Watergirl] Sending spawn event: ID=" << getId()
                  << " Pos=(" << x << ", " << y << ")" << std::endl;

        network->getMiddleware()->sendEvent(
            std::make_shared<SpawnEvent>(getId(), "watergirl", x, y)
        );
    }
}

// Remote constructor - DON'T hardcode position, it comes from SpawnEvent
Watergirl::Watergirl(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager,
    GameEngine *engine, bool active)
    : BaseCharacter(parentId, network, eventManager, engine, active, getDefaultBindings()) {
    setFallingSpritesheet(R"(resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/watergirl/walk-right.png)");

    // NOTE: Position is set by SpawnEvent.spawn() BEFORE this object is added to scene
    // Do NOT set position here or it will override the network position!
    std::cout << "[Watergirl] Remote character created, position managed by SpawnEvent" << std::endl;
}