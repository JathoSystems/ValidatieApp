//
// Created for lobby system
//

#ifndef VUURJONGEN_WATERMEISJE_CREATELOBBYPACKET_HPP
#define VUURJONGEN_WATERMEISJE_CREATELOBBYPACKET_HPP
#include "Network/Packet/Packet.h"

class CreateLobbyPacket : public Packet {
public:
    int levelId = 0;
    
    CreateLobbyPacket() { packetId = 103; levelId = 0; }
    
    explicit CreateLobbyPacket(int level) : levelId(level) { packetId = 103; }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeInt(levelId);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        levelId = buffer.readInt(offset);
    }
};

#endif //VUURJONGEN_WATERMEISJE_CREATELOBBYPACKET_HPP
