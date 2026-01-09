#include <iostream>
#include <asio.hpp>

#include "LevelSelector.h"
#include "SpawnEvent.hpp"
#include "characters/Fireboy.hpp"
#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "Engine/GameEngine.h"
#include "Events/EventManager.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ip/host_name.hpp"
#include "Events/EventRegistry.h"
#include "Network/NetworkSystem.h"
#include "Network/Packet/PacketRegistery.h"
#include "Network/Packet/Packets/NetworkEventPacket.h"
#include "Network/Sockets/TcpNetworkSocket.h"
#include "Physics/Collider.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"
#include "Scenes/Camera/FixedCamera.h"
#include "Network/Packet/Handler/PacketHandlerFactory.hpp"
#include "scenes/MainMenu.hpp"
#include "scenes/Game.hpp"
#include "scenes/Lobby.hpp"
#include "scenes/RestartScene.hpp"
#include "server/packet/GameReady.hpp"
#include "server/packet/PlayerAssignPacket.hpp"
#include "server/packet/handler/GameReadyPacketHandler.hpp"
#include "bat/events/BatMoveEvent.hpp"
#include "server/packet/handler/PlayerAssignPacketHandler.hpp"
#include "server/packet/handler/LobbyInfoPacketHandler.hpp"
#include "server/packet/LobbyInfoPacket.hpp"
#include "server/packet/CreateLobbyPacket.hpp"
#include "server/packet/JoinLobbyPacket.hpp"
#include "server/packet/NextLevelPacket.hpp"
#include "server/packet/QuitPacket.hpp"
#include "server/packet/RestartPacket.hpp"
#include "server/packet/handler/NextLevelHandler.hpp"
#include "server/packet/handler/QuitLevelPacketHandler.hpp"
#include "server/packet/handler/RestartLevelPacketHandler.hpp"
#include "GameObjectFactory.hpp"

std::string getLocalIPAddress() {
    try {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        asio::ip::tcp::resolver::query query(asio::ip::host_name(), "");
        asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
        asio::ip::tcp::resolver::iterator end;

        while (it != end) {
            asio::ip::tcp::endpoint endpoint = *it++;
            asio::ip::address addr = endpoint.address();

            if (addr.is_v4() && !addr.is_loopback()) {
                return addr.to_string();
            }
        }
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return "192.168.2.161";
}

extern std::mutex eventMutex;
extern std::vector<std::function<void()>> eventQueue;

int main() {
    try {
        GameEngine *gameEngine = &GameEngine::getInstance();
        gameEngine->init("Fireboy and watergirl revanced!", 1280, 720);

        auto sceneSystem = gameEngine->getSystem<SceneSystem>();
        if (!sceneSystem) {
            std::cerr << "[Main] ERROR: SceneSystem is null!" << std::endl;
            return 1;
        }

        // Network mag pas na de init gedaan worden
        auto network = std::make_shared<NetworkSystem>();
        network->connect(getLocalIPAddress(), 7534);

        EventManager manager(network->getMiddleware());

        // Register packets
        PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);
        PacketRegistery::getInstance().registerPacket<PlayerAssignPacket>(110);
        PacketHandlerFactory::getInstance().registerHandler(110, std::make_shared<PlayerAssignPacketHandler>());
        PacketRegistery::getInstance().registerPacket<GameReadyPacket>(102);
        PacketHandlerFactory::getInstance().registerHandler(102, std::make_shared<GameReadyPacketHandler>());

        PacketRegistery::getInstance().registerPacket<CreateLobbyPacket>(103);
        PacketRegistery::getInstance().registerPacket<JoinLobbyPacket>(104);
        PacketRegistery::getInstance().registerPacket<LobbyInfoPacket>(105);
        PacketRegistery::getInstance().registerPacket<QuitPacket>(120);
        PacketHandlerFactory::getInstance().registerHandler(120, std::make_shared<QuitLevelPacketHandler>());

        PacketRegistery::getInstance().registerPacket<RestartPacket>(121);
        PacketHandlerFactory::getInstance().registerHandler(121, std::make_shared<RestartLevelPacketHandler>());
        PacketRegistery::getInstance().registerPacket<NextLevelPacket>(122);
        PacketHandlerFactory::getInstance().registerHandler(122, std::make_shared<NextLevelPacketHandler>());

        auto lobbyInfoHandler = std::make_shared<LobbyInfoPacketHandler>();
        LobbyInfoPacketHandler::setNetworkAndEventManager(network, &manager);
        NextLevelPacketHandler::setNetworkAndEventManager(network, &manager);
        PacketHandlerFactory::getInstance().registerHandler(105, lobbyInfoHandler);

        // Register events
        EventRegistry::getInstance()->registerEvent("jump", []() {
            return std::make_shared<JumpEvent>();
        });

        EventRegistry::getInstance()->registerEvent("move", []() {
            return std::make_shared<MoveEvent>(0, Direction::NONE, false);
        });

        EventRegistry::getInstance()->registerEvent("spawn", []() {
            return std::make_shared<SpawnEvent>(0, "watergirl", 0.0f, 0.0f);
        });

        EventRegistry::getInstance()->registerEvent("batmove", []() {
            return std::make_shared<BatMoveEvent>(0, 0.0f, 0.0f);
        });

        // Set up event handlers
        network->getMiddleware()->setOnEventReceived([](int id, std::shared_ptr<IEvent> event) {
            std::lock_guard<std::mutex> lock(eventMutex);
            eventQueue.push_back([id, event]() {
                if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
                    spawn->spawn();
                    return;
                }

                GameObject *object = ObjectRegistry::getInstance().getObject(id);
                if (!object) {
                    int mappedId = SpawnEvent::getMappedId(id);
                    if (mappedId != id) {
                        object = ObjectRegistry::getInstance().getObject(mappedId);
                    }
                }
                if (!object) {
                    return;
                }
                event->apply(object);
            });
        });

        manager.setEventCallback([](int id, std::shared_ptr<IEvent> event) {
            if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
                spawn->spawn();
                return;
            }

            GameObject *object = ObjectRegistry::getInstance().getObject(id);
            if (!object) {
                int mappedId = SpawnEvent::getMappedId(id);
                if (mappedId != id) {
                    object = ObjectRegistry::getInstance().getObject(mappedId);
                }
            }
            if (!object) {
                return;
            }

            event->apply(object);
        });

        // Set up GameObjectFactory
        GameObjectFactory::getInstance().setNetworkSystem(network);
        GameObjectFactory::getInstance().setEventManager(&manager);

        // Add scenes
        sceneSystem->addScene(std::make_unique<MainMenu>());
        sceneSystem->addScene(std::make_unique<Lobby>());
        sceneSystem->addScene(std::make_unique<Game>(network, &manager));
        sceneSystem->addScene(std::make_unique<RestartScene>(network));

        // Create level selector scene
        LevelSelector levelSelector(sceneSystem, network, &manager);
        levelSelector.createLevelSelectorScene();

        // Set initial scene before starting
        sceneSystem->setScene("MainMenu");

        gameEngine->start();
    } catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}