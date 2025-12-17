//
// Created by jusra on 16-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMEREADY_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMEREADY_HPP

#include "Network/Packet/Packet.h"

class GameReadyPacket : public Packet {
public:
    GameReadyPacket() { packetId = 102; }

    void serialize() override {
        buffer.writeInt(packetId);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
    }
};
#endif //VUURJONGEN_WATERMEISJE_GAME_GAMEREADY_HPP