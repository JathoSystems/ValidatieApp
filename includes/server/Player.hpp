#ifndef VUURJONGEN_WATERMEISJE_GAME_PLAYER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_PLAYER_HPP
#include <cstdint>
#include <string>

class Player {
private:
    int _roomId = 1;
    int32_t _clientId;
    std::string _role;

public:
    Player(int32_t clientId, const std::string &role) : _clientId(clientId), _role(role) {
    }

    int32_t getId() const {
        return _clientId;
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_PLAYER_HPP
