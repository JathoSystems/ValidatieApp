//
// Created by jusra on 7-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LEVELSWITCHER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LEVELSWITCHER_HPP
#include "Engine/GameEngine.h"
#include "Events/EventManager.h"
#include "Network/NetworkSystem.h"
#include "Scenes/SceneSystem.h"
#include <memory>

class LevelSwitcher {
private:
    std::shared_ptr<NetworkSystem> _network;
    SceneSystem *_sceneSystem;
    EventManager *_eventManager;

public:
    LevelSwitcher(std::shared_ptr<NetworkSystem> network, EventManager *eventManager) : _network(network),
        _sceneSystem(
            GameEngine::getInstance().getSystem<SceneSystem>()),
        _eventManager(eventManager) {
    }

    void openLevel(int level, bool online);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LEVELSWITCHER_HPP
