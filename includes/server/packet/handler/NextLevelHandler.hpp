#ifndef VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP

#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "server/packet/NextLevelPacket.hpp"
#include "Scenes/SceneSystem.h"
#include "Engine/GameEngine.h"
#include "scenes/LevelScene.hpp"
#include <iostream>
#include <memory>

class NextLevelPacketHandler : public IPacketHandler {
private:
    static std::shared_ptr<NetworkSystem> g_network;
    static EventManager* g_eventManager;

public:
    void handle(const Packet &packet) override {
        NextLevelPacket nextLevel;
        nextLevel.getBuffer().setData(packet.getBuffer().getData());
        nextLevel.deserialize();

        std::cout << "Loading next level: " << nextLevel.getNextLevel()
                << " for lobby: " << nextLevel.getLobby() << "\n";

        GameEngine *engine = &GameEngine::getInstance();
        SceneSystem* sceneSystem = engine->getSystem<SceneSystem>();
        
        if (!sceneSystem) {
            std::cerr << "SceneSystem not found!\n";
            return;
        }

        if (!g_network || !g_eventManager) {
            std::cerr << "Network or EventManager not initialized!\n";
            return;
        }

        std::string levelSceneName = "level_" + std::to_string(nextLevel.getNextLevel()) + "_online";
        
        // Create and add the new level scene
        auto newLevelScene = std::make_unique<LevelScene>(
            nextLevel.getNextLevel(), 
            true, 
            g_network, 
            g_eventManager
        );

        sceneSystem->addScene(std::move(newLevelScene));
        sceneSystem->setScene(levelSceneName);
        
        std::cout << "Switched to level scene: " << levelSceneName << "\n";
    }

    static void setNetworkAndEventManager(const std::shared_ptr<NetworkSystem> &network, EventManager *eventManager) {
        g_network = network;
        g_eventManager = eventManager;
    }
};

// Static member initialization (put this in the .cpp file if you have one)
inline std::shared_ptr<NetworkSystem> NextLevelPacketHandler::g_network = nullptr;
inline EventManager* NextLevelPacketHandler::g_eventManager = nullptr;

#endif //VUURJONGEN_WATERMEISJE_GAME_NEXTLEVELHANDLER_HPP