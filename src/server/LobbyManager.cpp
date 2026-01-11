#include "server/LobbyManager.hpp"
#include <algorithm>
#include <iostream>

int LobbyManager::createLobby(int levelId, int32_t hostId) {
    int lobbyId = _nextLobbyId++;
    Lobby lobby(lobbyId, levelId);
    lobby.players.push_back(hostId);
    auto result = _lobbies.insert(std::make_pair(lobbyId, std::move(lobby)));
    return lobbyId;
}

bool LobbyManager::joinLobby(int lobbyId, int32_t clientId, int levelId) {
    auto it = _lobbies.find(lobbyId);
    if (it == _lobbies.end()) {
        std::cerr << "Lobby " << lobbyId << " does not exist" << std::endl;
        return false;
    }

    if (it->second.isFull()) {
        std::cerr << "Lobby " << lobbyId << " is full" << std::endl;
        return false;
    }

    std::cerr << "Attempting to join lobby " << lobbyId << " for level " << levelId << ", level of lobby is: " << it->
            second.levelId << std::endl;
    if (levelId != it->second.levelId) {
        std::cerr << "Level ID mismatch when joining lobby: " << levelId << " != " << it->second.levelId << std::endl;
        return false;
    }

    auto &players = it->second.players;
    if (std::find(players.begin(), players.end(), clientId) != players.end()) {
        std::cerr << "Player " << clientId << " is already in lobby " << lobbyId << std::endl;
        return false;
    }

    it->second.players.push_back(clientId);
    std::cerr << "Player " << clientId << " joined lobby " << lobbyId << " successfully" << std::endl;
    return true;
}

void LobbyManager::leaveLobby(int lobbyId, int32_t clientId) {
    auto it = _lobbies.find(lobbyId);
    if (it != _lobbies.end()) {
        auto &players = it->second.players;
        players.erase(
            std::remove(players.begin(), players.end(), clientId),
            players.end()
        );

        if (players.empty()) {
            _lobbies.erase(it);
        }
    }
}

Lobby *LobbyManager::getLobby(int lobbyId) {
    auto it = _lobbies.find(lobbyId);
    if (it != _lobbies.end()) {
        return &it->second;
    }
    return nullptr;
}

int LobbyManager::getLobbyIdForPlayer(int32_t clientId) {
    for (auto &pair: _lobbies) {
        for (int32_t playerId: pair.second.players) {
            if (playerId == clientId) {
                return pair.first;
            }
        }
    }
    return -1;
}

void LobbyManager::removeLobby(int lobbyId) {
    _lobbies.erase(lobbyId);
}

bool LobbyManager::lobbyExists(int lobbyId) {
    return _lobbies.find(lobbyId) != _lobbies.end();
}
