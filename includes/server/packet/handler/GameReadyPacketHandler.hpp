//
// Created by jusra on 17-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP
#include "Network/Packet/Handler/IPacketHandler.hpp"

class GameReadyPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_GAMEREADYPACKETHANDLER_HPP