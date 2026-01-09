//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#include "Engine/GameEngine.h"
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Scenes/SceneSystem.h"

class QuitLevelPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override {
        std::cout << "QUIT PACKET RECEIVED" << std::endl;
        auto prevName = GameEngine::getInstance().getSystem<SceneSystem>()->getActiveSceneObj()->getName();
        GameEngine::getInstance().getSystem<SceneSystem>()->setScene("MainMenu");
        GameEngine::getInstance().getSystem<SceneSystem>()->removeScene(prevName);
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP