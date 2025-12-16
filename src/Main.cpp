#include <iostream>

#include "SpawnEvent.hpp"
#include "characters/Fireboy.hpp"
#include "characters/events/JumpEvent.h"
#include "characters/events/MoveEvent.hpp"
#include "Engine/GameEngine.h"
#include "Events/EventManager.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "GameObjects/Component/KeyInputComponent.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Input/InputSystem.h"
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


int main() {
    try {
        GameEngine *gameEngine = &GameEngine::getInstance();
        gameEngine->init("Fireboy and watergirl revanced!", 1280, 720);

        // Network mag pas na de init gedaan worden
        auto network = std::make_shared<NetworkSystem>();
        auto result = network->connect("192.168.68.58", 7534);
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
