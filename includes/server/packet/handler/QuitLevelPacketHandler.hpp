//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#include "Engine/GameEngine.h"
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Network/NetworkSystem.h"
#include "Scenes/SceneSystem.h"
#include "GameObjects/ObjectRegistry.hpp"

class QuitLevelPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override {
        std::cout << "QUIT PACKET RECEIVED" << std::endl;
        
        // Clear packet queue and object registry before scene change
        NetworkSystem* networkSystem = GameEngine::getInstance().getSystem<NetworkSystem>();
        if (networkSystem && networkSystem->getMiddleware()) {
            networkSystem->getMiddleware()->clearPacketQueue();
        }
        ObjectRegistry::getInstance().clear();
        
        GameEngine::getInstance().getSystem<SceneSystem>()->setScene("MainMenu");
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP