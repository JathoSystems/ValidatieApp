//
// Created by jusra on 16-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_GAME_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAME_HPP
#include "Events/EventManager.h"
#include "Network/NetworkSystem.h"
#include "Scenes/Scene.h"

class Game : public Scene {
private:
    std::shared_ptr<NetworkSystem> _network;
    EventManager *_eventManager;

public:
    Game(std::shared_ptr<NetworkSystem> network, EventManager *eventManager);

    void onInitialRender() override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_GAME_HPP