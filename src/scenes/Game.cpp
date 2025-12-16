//
// Created by jusra on 16-12-2025.
//

#include "scenes/Game.hpp"

#include "characters/Fireboy.hpp"
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Input/InputSystem.h"
#include "Physics/Collider.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Scenes/Camera/FixedCamera.h"

Game::Game(std::shared_ptr<NetworkSystem> network, EventManager* eventManager) : Scene("Game") {
    _network = network;
    _eventManager = eventManager;
}

void Game::onInitialRender() {
    GameEngine* gameEngine = &GameEngine::getInstance();
    PhysicsSystem *physicsSystem = gameEngine->getSystem<PhysicsSystem>();
    InputSystem *inputSystem = gameEngine->getSystem<InputSystem>();

    if (!inputSystem)
        return;

    physicsSystem->setGravity(0.0f, 981.0f);

    auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
    setCamera(std::move(camera));

    auto ground = std::make_unique<GameObject>();
    ground->getTransform()->getPosition()->setX(640.0f);
    ground->getTransform()->getPosition()->setY(650.0f);
    ground->getTransform()->getSize()->setWidth(1280.0f);
    ground->getTransform()->getSize()->setHeight(100.0f);

    auto groundPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    groundPhysics->setBodyType(BodyType::STATIC);
    groundPhysics->setCollider(std::make_unique<BoxCollider>(1280.0f, 100.0f));
    groundPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    ground->addComponent(std::move(groundPhysics));

    auto groundRenderer = std::make_unique<SpriteRenderer>("../external/GameEngine/resources/square.png");
    groundRenderer->setParent(ground.get());
    ground->addComponent(std::move(groundRenderer));

    addObject(std::move(ground));

    // Platform
    auto platform = std::make_unique<GameObject>();
    platform->getTransform()->getPosition()->setX(400.0f);
    platform->getTransform()->getPosition()->setY(400.0f);
    platform->getTransform()->getSize()->setWidth(300.0f);
    platform->getTransform()->getSize()->setHeight(50.0f);

    auto platformPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    platformPhysics->setBodyType(BodyType::STATIC);
    platformPhysics->setCollider(std::make_unique<BoxCollider>(300.0f, 50.0f));
    platformPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    platform->addComponent(std::move(platformPhysics));

    auto platformRenderer = std::make_unique<SpriteRenderer>("../external/GameEngine/resources/square_blue.png");
    platformRenderer->setParent(platform.get());
    platform->addComponent(std::move(platformRenderer));

    addObject(std::move(platform));

    auto box = std::make_unique<GameObject>();
    box->getTransform()->getPosition()->setX(450.0f);
    box->getTransform()->getPosition()->setY(130.0f);
    box->getTransform()->getSize()->setWidth(60.0f);
    box->getTransform()->getSize()->setHeight(60.0f);

    auto boxPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    boxPhysics->setBodyType(BodyType::DYNAMIC);
    boxPhysics->setCollider(std::make_unique<BoxCollider>(60.0f, 60.0f));
    boxPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    boxPhysics->setGravityScale(1.0f);
    boxPhysics->setParent(box.get());
    box->addComponent(std::move(boxPhysics));

    auto boxRenderer = std::make_unique<SpriteRenderer>("../external/GameEngine/resources/square.png");
    boxRenderer->setParent(box.get());
    box->addComponent(std::move(boxRenderer));

    addObject(std::move(box));

    std::unique_ptr<Fireboy> fireboy = std::make_unique<Fireboy>(_network, _eventManager, gameEngine, true);
    addObject(std::move(fireboy));

    auto hud = std::make_unique<HUD>();

    auto fpsCounter = std::make_unique<FPSCounter>();
    fpsCounter->setPosition(5.0f, 5.0f);
    fpsCounter->setSize(80.0f, 30.0f);
    fpsCounter->setFontSize(20);

    hud->setFPSCounter(std::move(fpsCounter));
    setHUD(std::move(hud));
}
