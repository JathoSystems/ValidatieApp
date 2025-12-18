//
// Created for lobby system
//

#ifndef VUURJONGEN_WATERMEISJE_JOINLOBBYPACKET_HPP
#define VUURJONGEN_WATERMEISJE_JOINLOBBYPACKET_HPP
#include "Network/Packet/Packet.h"

class JoinLobbyPacket : public Packet {
public:
    int lobbyId = 0;
    
    JoinLobbyPacket() { packetId = 104; lobbyId = 0; }
    
    explicit JoinLobbyPacket(int id) : lobbyId(id) { packetId = 104; }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeInt(lobbyId);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        lobbyId = buffer.readInt(offset);
    }
};

#endif //VUURJONGEN_WATERMEISJE_JOINLOBBYPACKET_HPP
