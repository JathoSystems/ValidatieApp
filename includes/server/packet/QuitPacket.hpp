//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#include "Network/Packet/Packet.h"

class QuitPacket : public Packet {
private:
    int _lobby = 1;
public:
    QuitPacket() {
        packetId = 120;
        _lobby = std::stoi(GameState::getInstance().get("lobby"));
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