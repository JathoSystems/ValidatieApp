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
#include "GameObjects/GameObject.h"
#include "Scenes/SceneSystem.h"

class SpawnEvent : public IEvent {
private:
    int objectId;
    std::string objectType;
    float spawnX;
    float spawnY;

public:
    SpawnEvent(int id, std::string type, float x, float y)
        : objectId(id), objectType(std::move(type)), spawnX(x), spawnY(y) {}

    std::string getName() const override {
        return "spawn";
    }

    Package serialize() const override {
        Package data;

        // Serialize object ID (4 bytes)
        data.push_back((objectId >> 24) & 0xFF);
        data.push_back((objectId >> 16) & 0xFF);
        data.push_back((objectId >> 8) & 0xFF);
        data.push_back(objectId & 0xFF);

        // Serialize object type length (4 bytes)
        uint32_t typeLength = objectType.length();
        data.push_back((typeLength >> 24) & 0xFF);
        data.push_back((typeLength >> 16) & 0xFF);
        data.push_back((typeLength >> 8) & 0xFF);
        data.push_back(typeLength & 0xFF);

        // Serialize object type string
        data.insert(data.end(), objectType.begin(), objectType.end());

        // Serialize spawn X position (4 bytes)
        uint32_t xBits;
        std::memcpy(&xBits, &spawnX, sizeof(float));
        data.push_back((xBits >> 24) & 0xFF);
        data.push_back((xBits >> 16) & 0xFF);
        data.push_back((xBits >> 8) & 0xFF);
        data.push_back(xBits & 0xFF);

        // Serialize spawn Y position (4 bytes)
        uint32_t yBits;
        std::memcpy(&yBits, &spawnY, sizeof(float));
        data.push_back((yBits >> 24) & 0xFF);
        data.push_back((yBits >> 16) & 0xFF);
        data.push_back((yBits >> 8) & 0xFF);
        data.push_back(yBits & 0xFF);

        return data;
    }

    Data deserialize(const Package &package) override {
        if (package.size() < 8) {
            std::cerr << "Invalid spawn event data" << std::endl;
            return {};
        }

        size_t offset = 0;

        // Deserialize object ID (4 bytes)
        objectId = (package[offset] << 24) | (package[offset + 1] << 16) |
                   (package[offset + 2] << 8) | package[offset + 3];
        offset += 4;

        // Deserialize type length (4 bytes)
        uint32_t typeLength = (package[offset] << 24) | (package[offset + 1] << 16) |
                              (package[offset + 2] << 8) | package[offset + 3];
        offset += 4;

        if (offset + typeLength + 8 > package.size()) {
            std::cerr << "Invalid spawn event: insufficient data" << std::endl;
            return {};
        }

        // Deserialize object type string
        objectType = std::string(package.begin() + offset, package.begin() + offset + typeLength);
        offset += typeLength;

        // Deserialize spawn X position (4 bytes)
        uint32_t xBits = (package[offset] << 24) | (package[offset + 1] << 16) |
                         (package[offset + 2] << 8) | package[offset + 3];
        std::memcpy(&spawnX, &xBits, sizeof(float));
        offset += 4;

        // Deserialize spawn Y position (4 bytes)
        uint32_t yBits = (package[offset] << 24) | (package[offset + 1] << 16) |
                         (package[offset + 2] << 8) | package[offset + 3];
        std::memcpy(&spawnY, &yBits, sizeof(float));
        offset += 4;

        std::cout << "[SpawnEvent] Deserialized: ID=" << objectId
                  << " Type=" << objectType
                  << " Pos=(" << spawnX << ", " << spawnY << ")" << std::endl;

        return {};
    }

    void apply(GameObject *object) override {
        // Not used - spawn() handles creation
    }

    void spawn() {
        std::cout << "[SpawnEvent] Spawning " << objectType
                  << " at (" << spawnX << ", " << spawnY << ")" << std::endl;

        GameEngine *engine = &GameEngine::getInstance();
        SceneSystem *sceneSystem = engine->getSystem<SceneSystem>();

        if (!sceneSystem) {
            std::cerr << "Scene system not found!" << std::endl;
            return;
        }

        Scene *currentScene = sceneSystem->getActiveSceneObj();
        if (!currentScene) {
            std::cerr << "No current scene!" << std::endl;
            return;
        }

        // Create remote character (active=false)
        std::unique_ptr<GameObject> character = GameObjectFactory::getInstance().create(
            objectId, objectType);

        if (character) {
            // Set spawn position BEFORE adding to scene
            character->getTransform()->getPosition()->setX(spawnX);
            character->getTransform()->getPosition()->setY(spawnY);

            std::cout << "[SpawnEvent] Character positioned at ("
                      << spawnX << ", " << spawnY << ")" << std::endl;

            currentScene->addObject(std::move(character));
        } else {
            std::cerr << "Failed to create character: " << objectType << std::endl;
        }
    }

    int getObjectId() const { return objectId; }
    std::string getObjectType() const { return objectType; }
    float getSpawnX() const { return spawnX; }
    float getSpawnY() const { return spawnY; }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP