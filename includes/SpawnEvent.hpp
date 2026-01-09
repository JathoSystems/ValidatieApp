//
// Created by jusra on 15-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#define VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#include <iostream>
#include <cstring>
#include <unordered_map>

#include "GameObjectFactory.hpp"
#include "Engine/GameEngine.h"
#include "Events/IEvent.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "Scenes/SceneSystem.h"
#include "bat/BatAI.h"
#include "grid/GridManager.h" // Include GridManager to check if grid exists

class SpawnEvent : public IEvent {
private:
    static std::unordered_map<int, int> idMapping;
    int registryId = 0;
    std::string objectName = "fireboy";
    float spawnX = 0.0f;
    float spawnY = 0.0f;

public:
    SpawnEvent(int id = 0, const std::string &name = "fireboy", float x = 0.0f, float y = 0.0f)
        : registryId(id), objectName(name), spawnX(x), spawnY(y) {
    }

    std::string getName() const override {
        return "spawn";
    }

    Package serialize() const override {
        Package p;

        const uint8_t* idBytes = reinterpret_cast<const uint8_t*>(&registryId);
        for (int i = 0; i < sizeof(int); ++i) {
            p.push_back(idBytes[i]);
        }

        // Serialize X position (4 bytes)
        uint8_t* xBytes = (uint8_t*)&spawnX;
        for (int i = 0; i < 4; i++) {
            p.push_back(xBytes[i]);
        }

        // Serialize Y position (4 bytes)
        uint8_t* yBytes = (uint8_t*)&spawnY;
        for (int i = 0; i < 4; i++) {
            p.push_back(yBytes[i]);
        }

        // Serialize object name
        for (char c: objectName) {
            p.push_back(static_cast<int8_t>(c));
        }
        p.push_back(0);

        std::cout << "SENDING SPAWN EVENT " << objectName
                  << " WITH ID " << registryId
                  << " AT (" << spawnX << ", " << spawnY << ")" << std::endl;
        return p;
    }

    Data deserialize(const Package &package) override {
        Data data;

        std::cout << "[SpawnEvent] Deserializing packet of size: " << package.size() << std::endl;

        // Packet structure: 4 (id as int) + 4 (x) + 4 (y) + 1 (name min) = 13 bytes minimum
        if (package.size() < 13) {
            std::cout << "[SpawnEvent] ERROR: Packet too small: " << package.size() << " bytes" << std::endl;
            return data;
        }

        std::memcpy(&registryId, &package[0], sizeof(int));
        std::cout << "[SpawnEvent] Registry ID: " << registryId << std::endl;

        // Deserialize X position
        memcpy(&spawnX, &package[4], 4);
        std::cout << "[SpawnEvent] X: " << spawnX << std::endl;

        // Deserialize Y position
        memcpy(&spawnY, &package[8], 4);
        std::cout << "[SpawnEvent] Y: " << spawnY << std::endl;

        // Deserialize object name - ADD BOUNDS CHECK
        std::string name;
        for (size_t i = 12; i < package.size() && i < package.size(); ++i) {
            if (package[i] == 0) break;
            name += static_cast<char>(package[i]);
        }
        objectName = name;
        std::cout << "[SpawnEvent] Object name: " << objectName << std::endl;

        // Copy data
        for (size_t i = 0; i < package.size(); ++i) {
            data.push_back(package[i]);
        }

        std::cout << "[SpawnEvent] Deserialization complete" << std::endl;
        return data;
    }

    void apply(GameObject * gameObject) override {
        spawn();
    }

    void spawn() {
        GameObject* existingObj = ObjectRegistry::getInstance().getObject(registryId);
        if (existingObj) {
            Transform* transform = existingObj->getTransform();
            if (!transform || !transform->getPosition()) {
                std::cout << "[SpawnEvent] ERROR: Existing object has no transform!" << std::endl;
                return;
            }
            transform->getPosition()->setX(spawnX);
            transform->getPosition()->setY(spawnY);
            return;
        }

        auto system = GameEngine::getInstance().getSystem<SceneSystem>();
        if (!system) {
            std::cout << "[SpawnEvent] ERROR: SceneSystem is null!" << std::endl;
            return;
        }

        Scene *scene = system->getActiveSceneObj();
        if (!scene) {
            std::cout << "[SpawnEvent] ERROR: Active scene is null!" << std::endl;
            return;
        }

        // Check if grid exists for this scene before attempting to create bat
        // This prevents the factory from crashing when it tries to access the grid
        if (GridManager::getGrid(scene->getName()) == nullptr) {
            std::cout << "[SpawnEvent] Grid not ready for scene: " << scene->getName()
                      << " - Skipping spawn of " << objectName << std::endl;
            return;
        }

        std::unique_ptr<GameObject> object;
        object = GameObjectFactory::getInstance().create(registryId, objectName);
        if (!object) {
            std::cout << "[SpawnEvent] ERROR: Failed to create object: " << objectName << std::endl;
            return;
        }

        // Add null check for transform BEFORE using it
        Transform* transform = object->getTransform();
        if (!transform || !transform->getPosition()) {
            std::cout << "[SpawnEvent] ERROR: Created object has no transform!" << std::endl;
            return;
        }

        transform->getPosition()->setX(spawnX);
        transform->getPosition()->setY(spawnY);

        GameObject* objectPtr = object.get();
        ObjectRegistry::getInstance().insert(objectPtr, registryId);

        if (objectName == "bat") {
            BatAI* batAI = objectPtr->getComponent<BatAI>();
            if (batAI) {
                batAI->setNetworkPosition(spawnX, spawnY);
                batAI->setScene(scene);
            }
        }

        try {
            scene->addObject(std::move(object));
        } catch (const std::exception &e) {
            std::cout << "[SpawnEvent] Exception adding object to scene: " << e.what() << std::endl;
        }
    }

    float getSpawnX() const { return spawnX; }
    float getSpawnY() const { return spawnY; }
    
    static int getMappedId(int originalId);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP