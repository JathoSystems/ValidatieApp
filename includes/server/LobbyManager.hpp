//
// Created for lobby system
//

#ifndef VUURJONGEN_WATERMEISJE_LOBBYMANAGER_HPP
#define VUURJONGEN_WATERMEISJE_LOBBYMANAGER_HPP

#include <map>
#include <vector>
#include <cstdint>

struct Lobby {
    int lobbyId;
    int levelId;
    std::vector<int32_t> players; // client IDs
    
    Lobby(int id, int level) : lobbyId(id), levelId(level) {}
    
    int getPlayerCount() const { return static_cast<int>(players.size()); }
    bool isFull() const { return players.size() >= 2; }
};

class LobbyManager {
private:
    std::map<int, Lobby> _lobbies;
    int _nextLobbyId = 1;

public:
    int createLobby(int levelId, int32_t hostId);
    bool joinLobby(int lobbyId, int32_t clientId);
    void leaveLobby(int lobbyId, int32_t clientId);
    Lobby* getLobby(int lobbyId);
    int getLobbyIdForPlayer(int32_t clientId);
    void removeLobby(int lobbyId);
    bool lobbyExists(int lobbyId);
};

#endif //VUURJONGEN_WATERMEISJE_LOBBYMANAGER_HPP
