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

        // Packet structure: 4 (id as int) + 4 (x) + 4 (y) + 1 (name min) = 13 bytes minimum
        if (package.size() >= 13) {
            std::memcpy(&registryId, &package[0], sizeof(int));

            // Deserialize X position
            memcpy(&spawnX, &package[4], 4);

            // Deserialize Y position
            memcpy(&spawnY, &package[8], 4);

            // Deserialize object name
            std::string name;
            for (size_t i = 12; i < package.size(); ++i) {
                if (package[i] == 0) break;
                name += static_cast<char>(package[i]);
            }
            objectName = name;

            for (size_t i = 0; i < package.size(); ++i) {
                data.push_back(package[i]);
            }
        }

        return data;
    }

    void apply(GameObject * gameObject) override {
        // Now called directly on main thread - safe to access game objects
        spawn();
    }

    void spawn() {
        GameObject* existingObj = ObjectRegistry::getInstance().getObject(registryId);
        if (existingObj) {
            auto transform = existingObj->getTransform();
            if (transform) {
                auto position = transform->getPosition();
                if (position) {
                    position->setX(spawnX);
                    position->setY(spawnY);
                }
            }
            return;
        }

        auto system = GameEngine::getInstance().getSystem<SceneSystem>();
        if (!system) {
            return;
        }

        Scene *scene = system->getActiveSceneObj();
        if (!scene) {
            return;
        }

        std::unique_ptr<GameObject> object;
        object = GameObjectFactory::getInstance().create(registryId, objectName);
        if (!object) {
            return;
        }

        object->getTransform()->getPosition()->setX(spawnX);
        object->getTransform()->getPosition()->setY(spawnY);

        GameObject* objectPtr = object.get();
        
        // Note: Objects with Broadcastable (like Bat, BaseCharacter) register themselves
        // in the constructor, so we only need to insert non-Broadcastable objects here
        if (objectName != "bat" && objectName != "fireboy" && objectName != "watergirl") {
            ObjectRegistry::getInstance().insert(objectPtr, registryId);
        }
        
        if (objectName == "bat") {
            BatAI* batAI = objectPtr->getComponent<BatAI>();
            if (batAI) {
                batAI->setNetworkPosition(spawnX, spawnY);
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