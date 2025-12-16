//
// Created by jusra on 16-12-2025.
//

#include "server/PlayerManager.hpp"

#include <algorithm>

void PlayerManager::join(int32_t clientId, std::string role) {
    _players.emplace_back(clientId, role);
}

void PlayerManager::leave(int32_t clientId) {
    _players.erase(
        std::remove_if(_players.begin(), _players.end(),
            [clientId](const Player& player) {
                return player.getId() == clientId;
            }),
        _players.end()
    );
}
