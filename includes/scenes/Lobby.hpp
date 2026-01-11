//
// Created by jusra on 16-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LOBBY_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LOBBY_HPP
#include "Scenes/Scene.h"
#include "Network/NetworkSystem.h"
#include <string>
#include <memory>

class Lobby : public Scene {
private:
    int _lobbyId;
    int _levelId;
    int _playerCount;
    GameObject* _lobbyIdTextObj;
    GameObject* _statusTextObj;
    GameObject* _levelTextObj;
    std::shared_ptr<NetworkSystem> _network;

public:
    Lobby(std::shared_ptr<NetworkSystem> network);
    void setLobbyInfo(int lobbyId, int levelId, int playerCount);
    void updateStatus(const std::string& status);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LOBBY_HPP