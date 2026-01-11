#include "server/packet/handler/LobbyInfoPacketHandler.hpp"
#include "server/packet/LobbyInfoPacket.hpp"
#include "scenes/Lobby.hpp"
#include "scenes/LevelScene.hpp"
#include "Scenes/SceneSystem.h"
#include "Engine/GameEngine.h"
#include "Network/NetworkSystem.h"
#include "Network/GameState.hpp"
#include "Events/EventManager.h"
#include "GameObjects/ObjectRegistry.hpp"

static std::shared_ptr<NetworkSystem> g_network = nullptr;
static EventManager *g_eventManager = nullptr;

extern std::map<int, std::function<std::unique_ptr<Scene>()> > g_levels;

void LobbyInfoPacketHandler::setNetworkAndEventManager(std::shared_ptr<NetworkSystem> network,
                                                       EventManager *eventManager) {
    g_network = network;
    g_eventManager = eventManager;
}

void LobbyInfoPacketHandler::handle(const Packet &packet) {
    LobbyInfoPacket lobbyInfo;
    lobbyInfo.getBuffer().setData(packet.getBuffer().getData());
    lobbyInfo.deserialize();

    auto gameEngine = &GameEngine::getInstance();
    auto sceneSystem = gameEngine->getSystem<SceneSystem>();

    if (sceneSystem) {
        int lobbyId = lobbyInfo.lobbyId;
        int levelId = lobbyInfo.levelId;
        int playerCount = lobbyInfo.playerCount;
        std::string status = lobbyInfo.status;

        GameState::getInstance().set("lobby", std::to_string(lobbyId));

        std::string currentSceneName;
        Scene *currentScene = sceneSystem->getActiveSceneObj();
        if (currentScene) {
            currentSceneName = currentScene->getName();
        }

        sceneSystem->setScene("Lobby");

        if (!currentSceneName.empty() && currentSceneName.find("room_selection_") == 0) {
            sceneSystem->removeScene(currentSceneName);
        }

        Scene *activeScene = sceneSystem->getActiveSceneObj();
        if (activeScene) {
            Lobby *lobby = dynamic_cast<Lobby *>(activeScene);
            if (lobby) {
                lobby->setLobbyInfo(lobbyId, levelId, playerCount);
                lobby->updateStatus(status);
            }
        }

        if (g_network && g_eventManager && g_levels.find(levelId) != g_levels.end()) {
            auto scene = g_levels[levelId]();
            if (LevelScene *lvl = dynamic_cast<LevelScene *>(scene.get())) {
                lvl->toggleOnline(g_network, g_eventManager);
            }
            sceneSystem->addScene(std::move(scene));
        }
    }
}
