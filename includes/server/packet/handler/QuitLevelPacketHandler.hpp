//
// Created by jusra on 5-1-2026.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP
#include "Engine/GameEngine.h"
#include "Network/Packet/Handler/IPacketHandler.hpp"
#include "Scenes/SceneSystem.h"
#include <mutex>
#include <vector>
#include <functional>

extern std::mutex eventMutex;
extern std::vector<std::function<void()>> eventQueue;

class QuitLevelPacketHandler : public IPacketHandler {
public:
    void handle(const Packet &packet) override {
        std::cout << "QUIT PACKET RECEIVED" << std::endl;
        std::lock_guard<std::mutex> lock(eventMutex);
        eventQueue.push_back([]() {
            GameEngine::getInstance().getSystem<SceneSystem>()->setScene("MainMenu");
        });
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_QUITLEVELPACKETHANDLER_HPP