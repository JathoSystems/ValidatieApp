//
// Created by jusra on 17-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Network/NetworkSystem.h"
#include <memory>

class GameReadyPacketHandler : public IPacketHandler {
private:
    static std::shared_ptr<NetworkSystem> g_network;
public:
    void handle(const Packet &packet) override;
    
    static void setNetwork(const std::shared_ptr<NetworkSystem>& network) {
        g_network = network;
    }
};

inline std::shared_ptr<NetworkSystem> GameReadyPacketHandler::g_network = nullptr;

#endif //VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP