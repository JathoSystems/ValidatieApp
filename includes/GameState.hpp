//
// Created by jusra on 16-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMESTATE_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMESTATE_HPP

#include <unordered_map>
#include <string>
#include <mutex>

class GameState {
private:
    std::unordered_map<std::string, std::string> _data;
    std::mutex _mutex;

    GameState() = default;
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;

public:
    static GameState& getInstance() {
        static GameState instance;
        return instance;
    }

    void set(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(_mutex);
        _data[key] = value;
    }

    std::string get(const std::string& key, const std::string& defaultValue = "") {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _data.find(key);
        if (it != _data.end()) return it->second;
        return defaultValue;
    }

    void remove(const std::string& key) {
        std::lock_guard<std::mutex> lock(_mutex);
        _data.erase(key);
    }
};


#endif //VUURJONGEN_WATERMEISJE_GAME_GAMESTATE_HPP