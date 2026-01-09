//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#include "Network/Packet/Packet.h"

class QuitPacket : public Packet {
private:
    int _lobby;
public:
    QuitPacket(int lobby = 0) {
        packetId = 120;
        _lobby = lobby;
    }

    void serialize() override {
        buffer.writeInt(packetId);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
    }

    int getLobby() {
        return _lobby;
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP