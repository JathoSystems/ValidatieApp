#include <iostream>
#include <asio.hpp>

#include "SpawnEvent.hpp"
#include "../includes/characters/Fireboy.hpp"
#include "../includes/characters/events/JumpEvent.h"
#include "../includes/characters/events/MoveEvent.hpp"
#include "Engine/GameEngine.h"
#include "Events/EventManager.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ip/host_name.hpp"
#include "Network/NetworkSystem.h"
#include "Network/Packet/PacketRegistery.h"
#include "Network/Packet/Packets/NetworkEventPacket.h"
#include "Network/Sockets/TcpNetworkSocket.h"
#include "Physics/Collider.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"
#include "LevelCreator.h"
#include "Physics/PhysicsSystem.h"

#include "Scenes/Camera/FixedCamera.h"
#include "Network/Packet/Handler/PacketHandlerFactory.hpp"
#include "../includes/scenes/Game.hpp"
#include "../includes/scenes/Lobby.hpp"
#include "../includes/server/packet/GameReady.hpp"
#include "../includes/server/packet/PlayerAssignPacket.hpp"
#include "../includes/server/packet/handler/GameReadyPacketHandler.hpp"
#include "../includes/server/packet/handler/PlayerAssignPacketHandler.hpp"

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

    return "192.168.2.161";
}

// int main() {
//     try {
//         GameEngine *gameEngine = &GameEngine::getInstance();
//         gameEngine->init("Fireboy and watergirl revanced!", 1280, 720);
//
//         // Network mag pas na de init gedaan worden
//         auto network = std::make_shared<NetworkSystem>();
//         auto result = network->connect(getLocalIPAddress(), 7534);
//         EventManager manager(network->getMiddleware());
//
//         PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);
//
//         PacketRegistery::getInstance().registerPacket<PlayerAssignPacket>(110);
//         PacketHandlerFactory::getInstance().registerHandler(110, std::make_shared<PlayerAssignPacketHandler>());
//
//         PacketRegistery::getInstance().registerPacket<GameReadyPacket>(102);
//         PacketHandlerFactory::getInstance().registerHandler(102, std::make_shared<GameReadyPacketHandler>());
//
//
//         EventRegistry::getInstance()->registerEvent("jump", []() {
//             return std::make_shared<JumpEvent>();
//         });
//
//         EventRegistry::getInstance()->registerEvent("move", []() {
//             return std::make_shared<MoveEvent>(0, Direction::NONE, false);
//         });
//
//         EventRegistry::getInstance()->registerEvent("spawn", []() {
//             return std::make_shared<SpawnEvent>(0, "watergirl");
//         });
//
//         GameObjectFactory::getInstance().setNetworkSystem(network);
//         GameObjectFactory::getInstance().setEventManager(&manager);
//
//         network->getMiddleware()->setOnEventReceived([](int id, std::shared_ptr<IEvent> event) {
//             if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
//                 spawn->spawn();
//                 return;
//             }
//
//             GameObject *object = ObjectRegistry::getInstance().getObject(id);
//             if (!object) return;
//             event->apply(object);
//         });
//
//         manager.setEventCallback([](int id, std::shared_ptr<IEvent> event) {
//             if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
//                 spawn->spawn();
//                 return;
//             }
//
//             GameObject *object = ObjectRegistry::getInstance().getObject(id);
//
//             if (!object) return;
//
//             event->apply(object);
//         });
//
//         gameEngine->getSystem<SceneSystem>()->addScene(std::make_unique<Lobby>());
//         gameEngine->getSystem<SceneSystem>()->addScene(std::make_unique<Game>(network, &manager));
//         gameEngine->getSystem<SceneSystem>()->setScene("Lobby");
//
//
//         gameEngine->start();
//     } catch (const std::exception &e) {
//         std::cerr << "ERROR: " << e.what() << std::endl;
//         return 1;
//     }
//
//     return 0;
// }

int main() {
    try {
        std::cout << "Initializing Level Viewer" << std::endl;

        GameEngine *gameEngine = &GameEngine::getInstance();
        gameEngine->init("Fireboy & Watergirl - Level Viewer", 800, 600);

        auto physicsFacade = gameEngine->getSystem<PhysicsSystem>();

        std::cout << "Creating Scene " << std::endl;
        auto mainScene = std::make_unique<Scene>("MainScene");

        // Camera instellen
        auto viewport = std::make_unique<Viewport>(Size(800, 600), Position(0, 0));
        auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(0, 0));
        mainScene->setCamera(std::move(camera));

        std::cout << "Creating level..." << std::endl;

        int w,h;

        SDL_GetWindowSize(gameEngine->getWindow()->getWindow(), &w, &h);

        LevelCreator levelCreator(w,h);
        levelCreator.initLevel(mainScene.get(), physicsFacade ? physicsFacade->getBox2DFacade() : nullptr);

        levelCreator.createFromTextMap("Levels/level_1.txt");
        levelCreator.renderLevel();
        levelCreator.registerGrid("MainScene");

        std::cout << "Level rendered with " << mainScene->getObjects().size() << " objects" << std::endl;

        SceneSystem* sceneSystem = gameEngine->getSystem<SceneSystem>();
        if (sceneSystem) {
            sceneSystem->addScene(std::move(mainScene));
            sceneSystem->setScene("MainScene");
        }

        std::cout << "Starting Viewer" << std::endl;
        gameEngine->start();

    } catch (const std::exception &e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}