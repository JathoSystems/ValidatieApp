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
        auto network = std::make_shared<NetworkSystem>();
        auto result = network->connect("192.168.2.161", 7534);
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

        GameEngine *gameEngine = &GameEngine::getInstance();
        gameEngine->init("Fireboy and watergirl revanced!", 1280, 720);

        // PhysicsSystem *physicsSystem = gameEngine->getSystem<PhysicsSystem>();
        // InputSystem *inputSystem = gameEngine->getSystem<InputSystem>();
        //
        // if (!inputSystem) {
        //     return 1;
        // }
        //
        // physicsSystem->setGravity(0.0f, 981.0f);
        //
        // auto scene = std::make_unique<Scene>("MainScene");
        // auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
        // auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
        // scene->setCamera(std::move(camera));
        //
        // // Ground
        // auto ground = std::make_unique<GameObject>();
        // ground->getTransform()->getPosition()->setX(640.0f);
        // ground->getTransform()->getPosition()->setY(650.0f);
        // ground->getTransform()->getSize()->setWidth(1280.0f);
        // ground->getTransform()->getSize()->setHeight(100.0f);
        //
        // auto groundPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
        // groundPhysics->setBodyType(BodyType::STATIC);
        // groundPhysics->setCollider(std::make_unique<BoxCollider>(1280.0f, 100.0f));
        // groundPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
        // ground->addComponent(std::move(groundPhysics));
        //
        // auto groundRenderer = std::make_unique<SpriteRenderer>("../external/GameEngine/resources/square.png");
        // groundRenderer->setParent(ground.get());
        // ground->addComponent(std::move(groundRenderer));
        //
        // scene->addObject(std::move(ground));
        //
        // // Platform
        // auto platform = std::make_unique<GameObject>();
        // platform->getTransform()->getPosition()->setX(400.0f);
        // platform->getTransform()->getPosition()->setY(400.0f);
        // platform->getTransform()->getSize()->setWidth(300.0f);
        // platform->getTransform()->getSize()->setHeight(50.0f);
        //
        // auto platformPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
        // platformPhysics->setBodyType(BodyType::STATIC);
        // platformPhysics->setCollider(std::make_unique<BoxCollider>(300.0f, 50.0f));
        // platformPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
        // platform->addComponent(std::move(platformPhysics));
        //
        // auto platformRenderer = std::make_unique<SpriteRenderer>("../external/GameEngine/resources/square_blue.png");
        // platformRenderer->setParent(platform.get());
        // platform->addComponent(std::move(platformRenderer));
        //
        // scene->addObject(std::move(platform));
        //
        // auto box = std::make_unique<GameObject>();
        // box->getTransform()->getPosition()->setX(450.0f);
        // box->getTransform()->getPosition()->setY(130.0f);
        // box->getTransform()->getSize()->setWidth(60.0f);
        // box->getTransform()->getSize()->setHeight(60.0f);
        //
        // auto boxPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
        // boxPhysics->setBodyType(BodyType::DYNAMIC);
        // boxPhysics->setCollider(std::make_unique<BoxCollider>(60.0f, 60.0f));
        // boxPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
        // boxPhysics->setGravityScale(1.0f);
        // boxPhysics->setParent(box.get());
        // box->addComponent(std::move(boxPhysics));
        //
        // auto boxRenderer = std::make_unique<SpriteRenderer>("../external/GameEngine/resources/square.png");
        // boxRenderer->setParent(box.get());
        // box->addComponent(std::move(boxRenderer));
        //
        // scene->addObject(std::move(box));
        //
        // std::unique_ptr<Fireboy> fireboy = std::make_unique<Fireboy>(network, &manager, gameEngine, true);
        // scene->addObject(std::move(fireboy));
        //
        // auto hud = std::make_unique<HUD>();
        //
        // auto fpsCounter = std::make_unique<FPSCounter>();
        // fpsCounter->setPosition(5.0f, 5.0f);
        // fpsCounter->setSize(80.0f, 30.0f);
        // fpsCounter->setFontSize(20);
        //
        // hud->setFPSCounter(std::move(fpsCounter));
        // scene->setHUD(std::move(hud));

        gameEngine->getSystem<SceneSystem>()->addScene(std::make_unique<Lobby>());
        gameEngine->getSystem<SceneSystem>()->addScene(std::make_unique<Game>());
        gameEngine->getSystem<SceneSystem>()->setScene("Lobby");
        gameEngine->start();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
