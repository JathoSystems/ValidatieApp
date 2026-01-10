#ifndef VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#define VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#include <iostream>
#include <cstring>
#include <unordered_map>
#include <vector>

#include "GameObjectFactory.hpp"
#include "Engine/GameEngine.h"
#include "Events/IEvent.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "Scenes/SceneSystem.h"
#include "bat/BatAI.h"
#include "grid/GridManager.h"
#include "server/GlobalFlags.h"

class SpawnEvent : public IEvent {
private:
    static std::unordered_map<int, int> idMapping;
    int registryId = 0;
    std::string objectName = "fireboy";
    float spawnX = 0.0f;
    float spawnY = 0.0f;

public:
    static std::vector<SpawnEvent> _pendingEvents;

    SpawnEvent(int id = 0, const std::string &name = "fireboy", float x = 0.0f, float y = 0.0f)
        : registryId(id), objectName(name), spawnX(x), spawnY(y) {
    }

    std::string getName() const override {
        return "spawn";
    }

    static void processPending() {
        if (_pendingEvents.empty()) return;

        std::cout << "[SpawnEvent] Processing " << _pendingEvents.size() << " pending spawns..." << std::endl;
        std::vector<SpawnEvent> events = _pendingEvents;
        _pendingEvents.clear();

        for (auto& event : events) {
            event.spawn();
        }
    }

    Package serialize() const override {
        Package p;

        const uint8_t* idBytes = reinterpret_cast<const uint8_t*>(&registryId);
        for (int i = 0; i < sizeof(int); ++i) {
            p.push_back(idBytes[i]);
        }

        uint8_t* xBytes = (uint8_t*)&spawnX;
        for (int i = 0; i < 4; i++) p.push_back(xBytes[i]);

        uint8_t* yBytes = (uint8_t*)&spawnY;
        for (int i = 0; i < 4; i++) p.push_back(yBytes[i]);

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
        if (package.size() < 13) return data;

        std::memcpy(&registryId, &package[0], sizeof(int));
        memcpy(&spawnX, &package[4], 4);
        memcpy(&spawnY, &package[8], 4);

        std::string name;
        for (size_t i = 12; i < package.size() && i < package.size(); ++i) {
            if (package[i] == 0) break;
            name += static_cast<char>(package[i]);
        }
        objectName = name;

        for (size_t i = 0; i < package.size(); ++i) {
            data.push_back(package[i]);
        }
        return data;
    }

    void apply(GameObject * gameObject) override {
        if (GlobalFlags::isLevelCleaning) {
            return;
        }
        spawn();
    }

    void spawn() {
        if (GlobalFlags::isLevelCleaning) {
            return;
        }

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
        if (!system) return;

        Scene *scene = system->getActiveSceneObj();
        if (!scene) return;

        bool gridReady = (GridManager::getGrid(scene->getName()) != nullptr);

        if (!gridReady) {
            std::cout << "[SpawnEvent] Grid not ready for scene: " << scene->getName()
                      << " - BUFFERING spawn of " << objectName << std::endl;
            _pendingEvents.push_back(*this); // <--- IN DE WACHTRIJ
            return;
        }

        std::unique_ptr<GameObject> object;
        object = GameObjectFactory::getInstance().create(registryId, objectName);
        if (!object) {
            std::cout << "[SpawnEvent] ERROR: Failed to create object: " << objectName << std::endl;
            return;
        }

        Transform* transform = object->getTransform();
        if (transform && transform->getPosition()) {
            transform->getPosition()->setX(spawnX);
            transform->getPosition()->setY(spawnY);
        }

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