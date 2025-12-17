//
// Created by jusra on 17-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_PLAYERASSIGNPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_PLAYERASSIGNPACKETHANDLER_HPP
#include "Network/Packet/Handler/IPacketHandler.hpp"

class PlayerAssignPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_PLAYERASSIGNPACKETHANDLER_HPP