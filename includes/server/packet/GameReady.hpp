#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMEREADY_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMEREADY_HPP

#include "Network/Packet/Packet.h"

class GameReadyPacket : public Packet {
public:
    int levelId = 0;

    GameReadyPacket() {
        packetId = 102;
        levelId = 0;
    }

    GameReadyPacket(int level) : levelId(level) { packetId = 102; }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeInt(levelId);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        levelId = buffer.readInt(offset);
    }
};
#endif //VUURJONGEN_WATERMEISJE_GAME_GAMEREADY_HPP
