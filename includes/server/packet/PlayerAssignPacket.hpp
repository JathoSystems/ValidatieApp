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

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeString(_role);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        _role = buffer.readString(offset);
    }

    const std::string& getRole() const { return _role; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_PLAYERASSIGNPACKET_HPP
