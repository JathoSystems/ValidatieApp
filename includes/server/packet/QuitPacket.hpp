#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#include "Network/Packet/Packet.h"
#include "Network/GameState.hpp"

class QuitPacket : public Packet {
private:
    int _lobby;

public:
    QuitPacket() {
        packetId = 120;
        std::string lobbyStr = GameState::getInstance().get("lobby", "");
        if (!lobbyStr.empty()) {
            try {
                _lobby = std::stoi(lobbyStr);
            } catch (...) {
                _lobby = -1;
            }
        } else {
            _lobby = -1;
        }
    }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeInt(_lobby);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        _lobby = buffer.readInt(offset);
    }

    int getLobby() const {
        return _lobby;
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
