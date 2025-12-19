#include "characters/Watergirl.hpp"

#include "SpawnEvent.hpp"
#include "Engine/GameEngine.h"
#include "GameObjects/Spritesheet/Animator.h"

Watergirl::Watergirl(std::shared_ptr<NetworkSystem> network, EventManager* eventManager, GameEngine *engine, bool active)
    : BaseCharacter(network, eventManager, engine, active, getDefaultBindings()) {
    std::cout << "WATERGIRL IS " << (active ? "ACTIVE" : "INACTIVE") << std::endl;
    setFallingSpritesheet(R"(resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/watergirl/walk-right.png)");

    if (network && network->getMiddleware()) {
        network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(getId(), "watergirl"));
    }

    getTransform()->getPosition()->setX(600);
}

Watergirl::Watergirl(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager,
    GameEngine *engine, bool active)
    : BaseCharacter(parentId, network, eventManager, engine, active, getDefaultBindings()) {

    std::cout << "WATERGIRL IS " << (active ? "ACTIVE" : "INACTIVE") << std::endl;

    setFallingSpritesheet(R"(resources/watergirl/dropping.png)");
    setJumpingSpritesheet(R"(resources/watergirl/jumping.png)");
    setIdleSpritesheet(R"(resources/watergirl/idle.png)");
    setMovingLeftSpritesheet(R"(resources/watergirl/walk-left.png)");
    setMovingRightSpritesheet(R"(resources/watergirl/walk-right.png)");

    getTransform()->getPosition()->setX(600);
}
