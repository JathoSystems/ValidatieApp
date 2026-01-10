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
        std::cout << "[QuitLevelPacketHandler] QUIT PACKET RECEIVED" << std::endl;

        // Clear lobby state
        GameState::getInstance().remove("lobby");
        GameState::getInstance().remove("role");

        // Go to main menu
        GameEngine::getInstance().getSystem<SceneSystem>()->setScene("MainMenu");
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP