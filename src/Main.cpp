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
#include "server/packet/GameReady.hpp"
#include "server/packet/PlayerAssignPacket.hpp"
#include "server/packet/handler/GameReadyPacketHandler.hpp"
#include "server/packet/handler/PlayerAssignPacketHandler.hpp"
#include "server/packet/handler/LobbyInfoPacketHandler.hpp"
#include "server/packet/LobbyInfoPacket.hpp"
#include "server/packet/CreateLobbyPacket.hpp"
#include "server/packet/JoinLobbyPacket.hpp"

std::string getLocalIPAddress() {
    try {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        asio::ip::tcp::resolver::query query(asio::ip::host_name(), "");
        asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
        asio::ip::tcp::resolver::iterator end;

        while(it != end) {
            asio::ip::tcp::endpoint endpoint = *it++;
            asio::ip::address addr = endpoint.address();

            if(addr.is_v4() && !addr.is_loopback()) {
                return addr.to_string();
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return "127.0.0.1";
}

// REMOVED: Don't declare these here - use the ones from GameEngine
// std::mutex eventMutex;
// std::vector<std::function<void()>> eventQueue;

// Declare as extern to use the ones from GameEngine.cpp
extern std::mutex eventMutex;
extern std::vector<std::function<void()>> eventQueue;

int main() {
    try {
        GameEngine *gameEngine = &GameEngine::getInstance();
        gameEngine->init("Fireboy and watergirl revanced!", 1280, 720);

        // Network mag pas na de init gedaan worden
        auto network = std::make_shared<NetworkSystem>();
        network->connect(getLocalIPAddress(), 7534);
        network->getMiddleware()->setOnEventReceived([](int id, std::shared_ptr<IEvent> event) {
            std::lock_guard<std::mutex> lock(eventMutex);
            eventQueue.push_back([id, event]() {
                if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
                    spawn->spawn();
                    return;
                }
                GameObject *object = ObjectRegistry::getInstance().getObject(id);
                if (!object) return;
                event->apply(object);
            });
        });

        EventManager manager(network->getMiddleware());
        manager.setEventCallback([](int id, std::shared_ptr<IEvent> event) {
            // std::lock_guard<std::mutex> lock(eventMutex);
            // eventQueue.push_back([id, event]() {
            //     if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
            //         spawn->spawn();
            //         return;
            //     }
            //     GameObject *object = ObjectRegistry::getInstance().getObject(id);
            //     if (!object) return;
            //     event->apply(object);
            // });
        });

        PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);

        PacketRegistery::getInstance().registerPacket<PlayerAssignPacket>(110);
        PacketHandlerFactory::getInstance().registerHandler(110, std::make_shared<PlayerAssignPacketHandler>());

        PacketRegistery::getInstance().registerPacket<GameReadyPacket>(102);
        PacketHandlerFactory::getInstance().registerHandler(102, std::make_shared<GameReadyPacketHandler>());
        
        PacketRegistery::getInstance().registerPacket<CreateLobbyPacket>(103);
        PacketRegistery::getInstance().registerPacket<JoinLobbyPacket>(104);
        PacketRegistery::getInstance().registerPacket<LobbyInfoPacket>(105);

        auto lobbyInfoHandler = std::make_shared<LobbyInfoPacketHandler>();
        LobbyInfoPacketHandler::setNetworkAndEventManager(network, &manager);
        PacketHandlerFactory::getInstance().registerHandler(105, lobbyInfoHandler);

        EventRegistry::getInstance()->registerEvent("jump", []() {
            return std::make_shared<JumpEvent>();
        });

        EventRegistry::getInstance()->registerEvent("move", []() {
            return std::make_shared<MoveEvent>(0, Direction::NONE, false);
        });

        EventRegistry::getInstance()->registerEvent("spawn", []() {
            return std::make_shared<SpawnEvent>(0, "watergirl", 0.0f, 0.0f);
        });

        GameObjectFactory::getInstance().setNetworkSystem(network);
        GameObjectFactory::getInstance().setEventManager(&manager);

        SceneSystem* sceneSystem = gameEngine->getSystem<SceneSystem>();
        
        sceneSystem->addScene(std::make_unique<MainMenu>());
        
        LevelSelector levelSelector(sceneSystem, network, &manager);
        levelSelector.createLevelSelectorScene();
        
        sceneSystem->addScene(std::make_unique<Lobby>());
        sceneSystem->addScene(std::make_unique<Game>(network, &manager));
        
        sceneSystem->setScene("MainMenu");

        gameEngine->start();
    } catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}