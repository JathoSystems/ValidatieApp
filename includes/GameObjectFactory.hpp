#ifndef VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP
#define VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP

#include <functional>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "bat/Bat.h"
#include "bat/BatAI.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "grid/GridManager.h"
#include "GameObjects/GameObject.h"
#include "Engine/GameEngine.h"
#include "Scenes/SceneSystem.h"
#include "Scenes/Scene.h"
#include "Network/GameState.hpp"
#include "GameObjects/ObjectRegistry.hpp"

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
            auto system = GameEngine::getInstance().getSystem<SceneSystem>();
            if (!system) {
                std::cerr << "[Factory] SceneSystem is null when creating bat!" << std::endl;
                return nullptr;
            }
            
            Scene* activeScene = system->getActiveSceneObj();
            if (!activeScene) {
                std::cerr << "[Factory] Active scene is null when creating bat!" << std::endl;
                return nullptr;
            }
            
            std::string sceneName = activeScene->getName();
            LevelGrid* grid = GridManager::getGrid(sceneName);
            if (!grid) {
                std::cerr << "[Factory] Grid not found for scene: " << sceneName << std::endl;
                return nullptr;
            }
            
            const int CELL_SIZE = grid->getCellSize();
            auto bat = std::make_unique<Bat>(grid, CELL_SIZE, 80.0f);
            
            const int BAT_SIZE = CELL_SIZE;
            bat->getTransform()->getSize()->setWidth(BAT_SIZE);
            bat->getTransform()->getSize()->setHeight(BAT_SIZE);
            
            auto batAnimator = std::make_unique<Animator>("resources/bat/flying.png", 1, 8);
            bat->addComponent(std::move(batAnimator));
            
            Bat* batPtr = bat.get();
            
            bool isNetworked = (_network != nullptr);
            bool isAuthoritative = false;
            if (isNetworked) {
                std::string role = GameState::getInstance().get("role");
                isAuthoritative = (role == "fireboy");
            } else {
                isAuthoritative = true; // Single player
            }
            
            auto batAI = std::make_unique<BatAI>(batPtr, grid, activeScene, CELL_SIZE, 80.0f, isNetworked, _manager, parentId, isAuthoritative);
            bat->addComponent(std::move(batAI));
            
            return bat;
        });
    }

    GameObjectFactory(const GameObjectFactory&) = delete;
    GameObjectFactory& operator=(const GameObjectFactory&) = delete;

    std::unordered_map<std::string, CreatorFunc> creators_;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_GAMEOBJECTFACTORY_HPP