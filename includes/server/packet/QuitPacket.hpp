//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#include "Network/Packet/Packet.h"

class QuitPacket : public Packet {
public:
    QuitPacket() {
        packetId = 120;
    }

    void serialize() override {
        buffer.writeInt(packetId);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP