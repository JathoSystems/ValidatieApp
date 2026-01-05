//
// Created by jusra on 16-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_LOBBY_HPP
#define VUURJONGEN_WATERMEISJE_GAME_LOBBY_HPP
#include "Scenes/Scene.h"
#include <string>

class Lobby : public Scene {
private:
    int _lobbyId;
    int _levelId;
    int _playerCount;
    GameObject* _lobbyIdTextObj;
    GameObject* _statusTextObj;
    GameObject* _levelTextObj;

public:
    Lobby();
    void setLobbyInfo(int lobbyId, int levelId, int playerCount);
    void updateStatus(const std::string& status);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_LOBBY_HPP