//
// Created by jusra on 15-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#define VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#include <iostream>
#include <cstring>

#include "GameObjectFactory.hpp"
#include "Engine/GameEngine.h"
#include "Events/IEvent.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "Scenes/SceneSystem.h"

class SpawnEvent : public IEvent {
private:
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
        p.push_back(registryId);

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

        // Packet structure: 1 (id) + 4 (x) + 4 (y) + 1 (name min)
        if (package.size() >= 10) {
            registryId = package[0];

            // Deserialize X position
            memcpy(&spawnX, &package[1], 4);

            // Deserialize Y position
            memcpy(&spawnY, &package[5], 4);

            // Deserialize object name
            std::string name;
            for (size_t i = 9; i < package.size(); ++i) {
                if (package[i] == 0) break;
                name += static_cast<char>(package[i]);
            }
            objectName = name;

            data.push_back(registryId);
        }

        return data;
    }

    void apply(GameObject * gameObject) override {
        spawn();
    }

    void spawn() {
        std::cout << "SPAWNING " << objectName << " WITH ID " << registryId << " AT (" << spawnX << ", " << spawnY << ")" << std::endl;

        GameObject* existingObj = ObjectRegistry::getInstance().getObject(registryId);
        if (existingObj) {
            Transform* transform = existingObj->getTransform();
            if (transform && transform->getPosition()) {
                transform->getPosition()->setX(spawnX);
                transform->getPosition()->setY(spawnY);
            }
            return;
        }

        auto system = GameEngine::getInstance().getSystem<SceneSystem>();
        if (!system) {
            std::cout << "[SpawnEvent] SceneSystem is null!" << std::endl;
            return;
        }

        Scene *scene = system->getActiveSceneObj();
        if (!scene) {
            std::cout << "[SpawnEvent] Active scene is null!" << std::endl;
            return;
        }

        std::unique_ptr<GameObject> object;
        object = GameObjectFactory::getInstance().create(registryId, objectName);
        if (!object) {
            std::cout << "Factory returned nullptr" << std::endl;
            return;
        }

        Transform* transform = object->getTransform();
        if (transform && transform->getPosition()) {
            transform->getPosition()->setX(spawnX);
            transform->getPosition()->setY(spawnY);
        }

        try {
            scene->addObject(std::move(object));
        } catch (const std::exception &e) {
            std::cout << "[SpawnEvent] Exception adding object to scene: " << e.what() << std::endl;
        }
    }

    float getSpawnX() const { return spawnX; }
    float getSpawnY() const { return spawnY; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP