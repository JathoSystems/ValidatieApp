#ifndef VUURJONGEN_WATERMEISJE_GAME_PLAYERMANAGER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_PLAYERMANAGER_HPP
#include <cstdint>
#include <string>
#include <vector>

#include "Player.hpp"

class PlayerManager {
private:
    std::vector<Player> _players;
    std::vector<std::string> _roles;

public:
    PlayerManager() {
        _roles.emplace_back("watergirl");
        _roles.emplace_back("fireboy");
    }

    int getAmountOfPlayers() const {
        return static_cast<int>(_players.size());
    }

    int getAmountOfRoles() const {
        return static_cast<int>(_roles.size());
    }

    std::string getNextRole() {
        // First player gets fireboy, second gets watergirl
        return _players.empty() ? _roles[1] : _roles[0]; // roles[1] is fireboy, roles[0] is watergirl
    }

    void join(int32_t clientId, std::string role);

    void leave(int32_t clientId);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_PLAYERMANAGER_HPP
