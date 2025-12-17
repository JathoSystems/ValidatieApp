//
// Created by jusra on 16-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_PLAYERASSIGNPACKET_HPP
#define VUURJONGEN_WATERMEISJE_GAME_PLAYERASSIGNPACKET_HPP
#include "Network/Packet/Packet.h"

class PlayerAssignPacket : public Packet {
private:
    std::string _role;

public:
    PlayerAssignPacket() { packetId = 110; }

    explicit PlayerAssignPacket(const std::string& role)
        : _role(role) {
        packetId = 110;
    }

    void serialize() override;

    void deserialize() override;

    const std::string &getRole() const { return _role; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_PLAYERASSIGNPACKET_HPP
