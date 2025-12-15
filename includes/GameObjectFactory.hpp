//
// Created by jusra on 15-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP

#include <functional>
#include <memory>

#include "characters/Fireboy.hpp"
#include "GameObjects/GameObject.h"

class GameObjectFactory {
public:
    using CreatorFunc = std::function<std::unique_ptr<GameObject>()>;

    static GameObjectFactory& getInstance() {
        static GameObjectFactory instance;
        return instance;
    }

    void registerType(const std::string& name, CreatorFunc func) {
        creators_[name] = func;
    }

    std::unique_ptr<GameObject> create(const std::string& name) {
        auto it = creators_.find(name);
        if (it != creators_.end()) {
            return it->second(); // Roept de geregistreerde functie aan
        }
        return nullptr;
    }

private:
    GameObjectFactory() {
        registerType("fireboy", []() {
            return std::make_unique<Fireboy>(nullptr, nullptr, false);
        });
    }
    GameObjectFactory(const GameObjectFactory&) = delete;
    GameObjectFactory& operator=(const GameObjectFactory&) = delete;

    std::unordered_map<std::string, CreatorFunc> creators_;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP