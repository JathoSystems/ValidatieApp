//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP
#include "Network/Packet/Packet.h"
#include "Network/GameState.hpp"

class QuitPacket : public Packet {
private:
    int _lobby = 0;
public:
    QuitPacket() {
        packetId = 120;
        _lobby = 0;

        try {
            std::string lobbyStr = GameState::getInstance().get("lobby");
            std::cout << "[QuitPacket] Raw lobby string from GameState: '" << lobbyStr << "'" << std::endl;

            if (!lobbyStr.empty()) {
                // Try to convert, but catch any exceptions
                try {
                    _lobby = std::stoi(lobbyStr);
                } catch (const std::invalid_argument& e) {
                    std::cout << "[QuitPacket] Invalid lobby ID format: " << lobbyStr << std::endl;
                    _lobby = 0;
                } catch (const std::out_of_range& e) {
                    std::cout << "[QuitPacket] Lobby ID out of range: " << lobbyStr << std::endl;
                    _lobby = 0;
                }
            } else {
                std::cout << "[QuitPacket] Lobby string is empty, defaulting to 0" << std::endl;
            }
        } catch (...) {
            std::cout << "[QuitPacket] Unexpected error reading lobby ID, defaulting to 0" << std::endl;
            _lobby = 0;
        }

        std::cout << "[QuitPacket] Sending Quit for Lobby ID: " << _lobby << std::endl;
    }

    void serialize() override {
        buffer.writeInt(packetId);
        buffer.writeInt(_lobby);
    }

    void deserialize() override {
        size_t offset = 0;
        packetId = buffer.readInt(offset);
        _lobby = buffer.readInt(offset);
        std::cout << "[QuitPacket] Deserialized Lobby ID: " << _lobby << std::endl;
    }

    int getLobby() const {
        return _lobby;
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITPACKET_HPP