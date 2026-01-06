#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP

#include <functional>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "bat/Bat.h"
#include "bat/BatSpriteRenderer.h"
#include "grid/GridManager.h"
#include "GameObjects/GameObject.h"

class GameObjectFactory {
public:
    using CreatorFunc = std::function<std::unique_ptr<GameObject>(int parentId)>;

    static GameObjectFactory& getInstance() {
        static GameObjectFactory instance;
        return instance;
    }

    void registerType(const std::string& name, CreatorFunc func) {
        creators_[name] = func;
    }

    std::unique_ptr<GameObject> create(int parentId, const std::string& name) {
        auto it = creators_.find(name);
        if (it != creators_.end()) {
            return it->second(parentId);
        }
        return nullptr;
    }

    void setNetworkSystem(std::shared_ptr<NetworkSystem> network) {
        _network = network;
    }

    void setEventManager(EventManager * manager) {
        _manager = manager;
    }

    EventManager* getEventManager() const {
        return _manager;
    }

private:
    std::shared_ptr<NetworkSystem> _network;
    EventManager * _manager = nullptr;

    GameObjectFactory() {
        // Capture 'this' to access member variables at runtime
        registerType("fireboy", [this](int parentId) -> std::unique_ptr<GameObject> {
            if (!_network || !_manager) {
                std::cerr << "[Factory] Network or EventManager not set!" << std::endl;
                return nullptr;
            }
            return std::make_unique<Fireboy>(
                parentId,
                _network,
                _manager,
                &GameEngine::getInstance(),
                false
            );
        });

        registerType("watergirl", [this](int parentId) -> std::unique_ptr<GameObject> {
            if (!_network || !_manager) {
                std::cerr << "[Factory] Network or EventManager not set!" << std::endl;
                return nullptr;
            }
            return std::make_unique<Watergirl>(
                parentId,
                _network,
                _manager,
                &GameEngine::getInstance(),
                false
            );
        });
        
        registerType("bat", [this](int parentId) -> std::unique_ptr<GameObject> {
            LevelGrid* grid = GridManager::getGrid("Game");
            if (!grid) {
                return nullptr;
            }
            const int CELL_SIZE = 10;
            auto bat = std::make_unique<Bat>(grid, CELL_SIZE, 80.0f);
            
            const int BAT_SIZE = CELL_SIZE * 4;
            bat->getTransform()->getSize()->setWidth(BAT_SIZE);
            bat->getTransform()->getSize()->setHeight(BAT_SIZE);
            
            auto batRenderer = std::make_unique<BatSpriteRenderer>("resources/sprite2.png");
            bat->addComponent(std::move(batRenderer));
            
            return bat;
        });
    }

    GameObjectFactory(const GameObjectFactory&) = delete;
    GameObjectFactory& operator=(const GameObjectFactory&) = delete;

    std::unordered_map<std::string, CreatorFunc> creators_;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP