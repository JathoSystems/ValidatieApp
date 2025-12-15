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
    SpawnEvent(int id = 0, const std::string& name = "fireboy")
        : registryId(id), objectName(name) {}

    std::string getName() const override {
        return "spawn";
    }

    // Zet registryId en objectName om naar een Package
    Package serialize() const override {
        Package p;
        p.push_back(registryId);

        // Zet string om naar bytes
        for (char c : objectName) {
            p.push_back(static_cast<int8_t>(c));
        }
        p.push_back(0); // null-terminator
        return p;
    }

    // Lees registryId en objectName uit Package
    Data deserialize(const Package &package) override {
        Data data;

        if (package.size() >= 2) {
            registryId = package[0];

            // String reconstrueren
            std::string name;
            for (size_t i = 1; i < package.size(); ++i) {
                if (package[i] == 0) break;
                name += static_cast<char>(package[i]);
            }
            objectName = name;

            data.push_back(registryId);
            for (char c : objectName) data.push_back(static_cast<int8_t>(c));
        }

        return data;
    }

    void apply(GameObject *gameObject) override {
        // std::cout << objectName << " houruh" << std::endl;
        //
        // Scene* scene = GameEngine::getInstance().getSystem<SceneSystem>()->getActiveSceneObj();
        //
        // if (!scene) {
        //     std::cout << "Scene is null" << std::endl;
        //     return;
        // }
        //
        // std::unique_ptr<GameObject> object = GameObjectFactory::getInstance().create("fireboy");
        //
        // if (!object) {
        //     std::cout << "Object is null" << std::endl;
        //     return;
        // }
        //
        // scene->addObject(std::move(object));
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_SPAWNEVENT_HPP