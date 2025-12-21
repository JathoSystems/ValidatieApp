//
// Created by jusra on 15-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#define VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
#include <iostream>

#include "GameObjectFactory.hpp"
#include "Engine/GameEngine.h"
#include "Events/IEvent.h"
#include "Scenes/SceneSystem.h"

class SpawnEvent : public IEvent {
private:
    int registryId = 0;
    std::string objectName = "fireboy";

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

        std::cout << "SENDING SPAWN EVENT " << objectName << " WITH ID " << registryId << std::endl;
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
        // When apply is called (from NetworkEventPacketHandler), spawn the object
        spawn();
    }

    void spawn() {
        std::cout << "SPAWNING " << objectName << " WITH ID " << registryId << std::endl;
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

        try {
            scene->addObject(std::move(object));
        } catch (const std::exception &e) {
            std::cout << "[SpawnEvent] Exception adding object to scene: " << e.what() << std::endl;
        }
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP
