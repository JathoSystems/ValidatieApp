#include <iostream>
#include <asio.hpp>

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
#include "../external/GameEngine/includes/Network/Packet/Packets/PlayerAssignPacket.hpp"
#include "Network/Packet/Packets/GameReady.hpp"
#include "scenes/Game.hpp"
#include "scenes/Lobby.hpp"

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

int main() {
    try {
        GameEngine *gameEngine = &GameEngine::getInstance();
        gameEngine->init("Fireboy and watergirl revanced!", 1280, 720);

        // Network mag pas na de init gedaan worden
        auto network = std::make_shared<NetworkSystem>();
        auto result = network->connect(getLocalIPAddress(), 7534);
        EventManager manager(network->getMiddleware());

        PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);
        PacketRegistery::getInstance().registerPacket<PlayerAssignPacket>(110);
        PacketRegistery::getInstance().registerPacket<GameReadyPacket>(102);

        EventRegistry::getInstance()->registerEvent("jump", []() {
            return std::make_shared<JumpEvent>();
        });

        EventRegistry::getInstance()->registerEvent("move", []() {
            return std::make_shared<MoveEvent>(0, Direction::NONE, false);
        });

        EventRegistry::getInstance()->registerEvent("spawn", []() {
            return std::make_shared<SpawnEvent>(0, "watergirl");
        });

        GameObjectFactory::getInstance().setNetworkSystem(network);
        GameObjectFactory::getInstance().setEventManager(&manager);

        network->getMiddleware()->setOnEventReceived([](int id, std::shared_ptr<IEvent> event) {
            if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
                spawn->spawn();
                return;
            }

            GameObject *object = ObjectRegistry::getInstance().getObject(id);
            if (!object) return;
            event->apply(object);
        });

        manager.setEventCallback([](int id, std::shared_ptr<IEvent> event) {
            if (SpawnEvent *spawn = dynamic_cast<SpawnEvent *>(event.get())) {
                spawn->spawn();
                return;
            }

            GameObject *object = ObjectRegistry::getInstance().getObject(id);

            if (!object) return;

            event->apply(object);
        });

        gameEngine->getSystem<SceneSystem>()->addScene(std::make_unique<Lobby>());
        gameEngine->getSystem<SceneSystem>()->addScene(std::make_unique<Game>(network, &manager));
        gameEngine->getSystem<SceneSystem>()->setScene("Lobby");


        gameEngine->start();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
