//
// Created by jusra on 17-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Network/NetworkSystem.h"
#include "Events/EventManager.h"
#include <memory>

class GameReadyPacketHandler : public IPacketHandler {
public:
    static void setNetworkAndEventManager(std::shared_ptr<NetworkSystem> network, EventManager* eventManager);
    void handle(const Packet &packet) override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP