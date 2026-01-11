//
// Created by jusra on 6-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELPACKET_HPP
#define VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELPACKET_HPP

#include "Network/Packet/Packet.h"
#include "Network/GameState.hpp"
#include <string>

class NextLevelPacket : public Packet {
private:
    int32_t lobbyId;
    int32_t nextLevelId;

public:
    NextLevelPacket() : lobbyId(0), nextLevelId(0) {
        packetId = 122;
        try {
            lobbyId = std::stoi(GameState::getInstance().get("lobby"));
        } catch (...) {
            lobbyId = 1;
        }
    }

    NextLevelPacket(int32_t nextLevel) : nextLevelId(nextLevel) {
        packetId = 122;
        try {
            lobbyId = std::stoi(GameState::getInstance().get("lobby"));
        } catch (...) {
            lobbyId = 1;
        }
    }

    NextLevelPacket(int32_t lobby, int32_t nextLevel)
        : lobbyId(lobby), nextLevelId(nextLevel) {
        packetId = 122;
    }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeInt(lobbyId);
        buffer.writeInt(nextLevelId);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        lobbyId = buffer.readInt(offset);
        nextLevelId = buffer.readInt(offset);
    }

    int32_t getLobby() const { return lobbyId; }
    int32_t getNextLevel() const { return nextLevelId; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELPACKET_HPP
