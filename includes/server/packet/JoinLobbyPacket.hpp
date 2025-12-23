//
// Created for lobby system
//

#ifndef VUURJONGEN_WATERMEISJE_JOINLOBBYPACKET_HPP
#define VUURJONGEN_WATERMEISJE_JOINLOBBYPACKET_HPP
#include "Network/Packet/Packet.h"

class JoinLobbyPacket : public Packet {
public:
    int lobbyId = 0;
    int levelId = 1;
    
    JoinLobbyPacket() { packetId = 104; lobbyId = 0; }
    
    explicit JoinLobbyPacket(int id, int level) : levelId(level), lobbyId(id) { packetId = 104; }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeInt(lobbyId);
        buffer.writeInt(levelId);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        lobbyId = buffer.readInt(offset);
        levelId = buffer.readInt(offset);
    }
};

#endif //VUURJONGEN_WATERMEISJE_JOINLOBBYPACKET_HPP
