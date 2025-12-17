//
// Created by jusra on 15-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#define VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP

#include "GameObjectFactory.hpp"
#include "Engine/GameEngine.h"
#include "Events/IEvent.h"
#include "Scenes/SceneSystem.h"
#include "characters/BaseCharacter.hpp"
#include "bat/Bat.h"
#include "bat/BatAI.h"
#include "grid/GridManager.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "Events/EventManager.h"
#include <unordered_map>

class SpawnEvent : public IEvent {
private:
    int registryId = 0;
    std::string objectName = "fireboy";
    static std::unordered_map<int, int> idMapping;

public:
    SpawnEvent(int id = 0, const std::string &name = "fireboy")
        : registryId(id), objectName(name) {
    }

    std::string getName() const override {
        return "spawn";
    }

    Package serialize() const override {
        Package p;
        p.push_back(registryId);

        for (char c: objectName) {
            p.push_back(static_cast<int8_t>(c));
        }
        p.push_back(0);
        return p;
    }

    Data deserialize(const Package &package) override {
        Data data;

        if (package.size() >= 2) {
            registryId = package[0];

            std::string name;
            for (size_t i = 1; i < package.size(); ++i) {
                if (package[i] == 0) break;
                name += static_cast<char>(package[i]);
            }
            objectName = name;

            data.push_back(registryId);
            for (char c: objectName) data.push_back(static_cast<int8_t>(c));
        }

        return data;
    }

    void apply(GameObject * gameObject) override {
        spawn();
    }

    void spawn() {
        auto system = GameEngine::getInstance().getSystem<SceneSystem>();
        if (!system) return;

        Scene *scene = system->getActiveSceneObj();
        if (!scene) return;

        if (scene->getName() == "Lobby") return;

        std::unique_ptr<GameObject> object;
        object = GameObjectFactory::getInstance().create(registryId, objectName);
        if (!object) return;

        GameObject* objPtr = object.get();
        int registeredId = ObjectRegistry::getInstance().registerObject(objPtr);
        
        if (objectName == "fireboy" || objectName == "watergirl") {
            BaseCharacter* character = dynamic_cast<BaseCharacter*>(objPtr);
            if (character) {
                BaseCharacterController* controller = character->getController();
                if (controller) {
                    controller->setParentId(registeredId);
                }
            }
        }
        
        if (registeredId != registryId) {
            idMapping[registryId] = registeredId;
        }

        if (objectName == "fireboy" || objectName == "watergirl") {
            auto* transform = objPtr->getTransform();
            if (transform) {
                auto* pos = transform->getPosition();
                if (pos) {
                    pos->setX(objectName == "fireboy" ? 200 : 600);
                    pos->setY(500);
                }
            }

            std::string idleSprite = objectName == "fireboy" 
                ? "resources/fireboy/idle.png" 
                : "resources/watergirl/idle.png";
            
            BaseCharacter* character = dynamic_cast<BaseCharacter*>(objPtr);
            if (character) {
                std::string charIdle = character->getIdleSpritesheet();
                if (!charIdle.empty()) {
                    idleSprite = charIdle;
                }
            }
            
            auto animator = std::make_unique<Animator>(idleSprite, 1, 5);
            objPtr->addComponent(std::move(animator));
        } else if (objectName == "bat") {
            LevelGrid* grid = GridManager::getGrid("Game");
            if (grid) {
                Bat* bat = dynamic_cast<Bat*>(objPtr);
                if (bat) {
                    const int CELL_SIZE = 10;
                    const int GRID_WIDTH = 1280 / CELL_SIZE;
                    const int GRID_HEIGHT = 720 / CELL_SIZE;
                    
                    int startGridX = GRID_WIDTH / 2;
                    int startGridY = GRID_HEIGHT / 2;
                    bool foundStart = false;
                    
                    for (int radius = 0; radius < std::min(GRID_WIDTH, GRID_HEIGHT) / 2 && !foundStart; ++radius) {
                        for (int y = startGridY - radius; y <= startGridY + radius && !foundStart; ++y) {
                            for (int x = startGridX - radius; x <= startGridX + radius && !foundStart; ++x) {
                                if (x > startGridX - radius && x < startGridX + radius &&
                                    y > startGridY - radius && y < startGridY + radius) {
                                    continue;
                                }
                                
                                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                                    if (grid->isWalkable(x, y)) {
                                        startGridX = x;
                                        startGridY = y;
                                        foundStart = true;
                                    }
                                }
                            }
                        }
                    }
                    
                    if (foundStart) {
                        float worldX, worldY;
                        grid->gridToWorld(startGridX, startGridY, worldX, worldY);
                        worldX += CELL_SIZE / 2.0f;
                        worldY += CELL_SIZE / 2.0f;
                        
                        auto* transform = bat->getTransform();
                        if (transform) {
                            transform->getPosition()->setX(static_cast<int>(worldX));
                            transform->getPosition()->setY(static_cast<int>(worldY));
                        }
                        
                        EventManager* eventManager = GameObjectFactory::getInstance().getEventManager();
                        bool isNetworked = (eventManager != nullptr);
                        int batId = registeredId;
                        bool isAuthoritative = true;
                        
                        auto batAI = std::make_unique<BatAI>(bat, grid, scene, CELL_SIZE, 80.0f, isNetworked, eventManager, batId, isAuthoritative);
                        bat->addComponent(std::move(batAI));
                    }
                }
            }
        }

        try {
            scene->addObject(std::move(object));
        } catch (const std::exception &e) {
        }
    }
    
    static int getMappedId(int originalId);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
