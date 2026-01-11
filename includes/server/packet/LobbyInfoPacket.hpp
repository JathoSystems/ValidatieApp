#ifndef VUURJONGEN_WATERMEISJE_LOBBYINFOPACKET_HPP
#define VUURJONGEN_WATERMEISJE_LOBBYINFOPACKET_HPP
#include "Network/Packet/Packet.h"

class LobbyInfoPacket : public Packet {
public:
    int lobbyId = 0;
    int levelId = 0;
    int playerCount = 0;
    std::string status;

    LobbyInfoPacket() {
        packetId = 105;
        lobbyId = 0;
        levelId = 0;
        playerCount = 0;
        status = "";
    }

    LobbyInfoPacket(int lobby, int level, int players, const std::string &stat)
        : lobbyId(lobby), levelId(level), playerCount(players), status(stat) {
        packetId = 105;
    }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeInt(lobbyId);
        buffer.writeInt(levelId);
        buffer.writeInt(playerCount);
        buffer.writeString(status);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        lobbyId = buffer.readInt(offset);
        levelId = buffer.readInt(offset);
        playerCount = buffer.readInt(offset);
        status = buffer.readString(offset);
    }
};

#endif //VUURJONGEN_WATERMEISJE_LOBBYINFOPACKET_HPP
