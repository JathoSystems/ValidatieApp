//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_RESTARTPACKET_HPP
#define VUURJONGEN_WATERMEISJE_GAME_RESTARTPACKET_HPP
#include "Network/GameState.hpp"
#include "Network/Packet/Packet.h"

class RestartPacket : public Packet {
private:
    std::string _level = "MainMenu";
    int lobby;

public:
    RestartPacket() {
        packetId = 121;
        try {
            lobby = std::stoi(GameState::getInstance().get("lobby"));
        } catch (...) {
            lobby = 1;
        }
    }

    RestartPacket(std::string level) {
        packetId = 121;
        _level = level;
        try {
            lobby = std::stoi(GameState::getInstance().get("lobby"));
        } catch (...) {
            lobby = 1;
        }
    }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeString(_level);
        buffer.writeInt(lobby);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        _level = buffer.readString(offset);
        lobby = buffer.readInt(offset);
    }

    std::string getLevel() const {
        return _level;
    }

    int getLobby() const {
        return lobby;
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_RESTARTPACKET_HPP