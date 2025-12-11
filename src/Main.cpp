#include <iostream>

#include "characters/Fireboy.hpp"
#include "characters/events/JumpEvent.h"
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
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

auto network = std::make_shared<NetworkSystem>();
auto result = network->connect("127.0.0.1", 7534);
EventManager manager(network->getMiddleware());

class PlayerController : public IKeyListener {
private:
    GameObject *object;
    PhysicsComponent *_physics = nullptr;
    float _moveSpeed = 300.0f;
    float _jumpForce = 5000.0f;
    int _groundContactCount = 0;
    bool _movingLeft = false;
    bool _movingRight = false;

public:
    void setObject(GameObject *object) { this->object = object; }

    void setPhysicsComponent(PhysicsComponent *physics) {
        _physics = physics;
    }

    void addGroundContact() {
        _groundContactCount++;
    }

    void removeGroundContact() {
        _groundContactCount = std::max(0, _groundContactCount - 1);
    }

    bool isGrounded() const {
        return _groundContactCount > 0;
    }

    void onKeyPress(Key key) override {
        if (!_physics) {
            std::cout << "Physics is null!" << std::endl;
            return;
        }

        switch (key) {
            case Key::A:
            case Key::LEFT:
                _movingLeft = true;
                break;
            case Key::D:
            case Key::RIGHT:
                _movingRight = true;
                break;
            case Key::SPACE:
            case Key::W:
            case Key::UP:
                GameObject *parent = object;

                if (network->isConnected()) {
                    std::cout << "Network is connected!\n";
                }

                manager.setEventCallback([parent](std::shared_ptr<IEvent> event) {
                    std::cout << "Mis deze\n";
                    event->apply(parent);
                });

                network->getMiddleware()->setOnEventReceived([parent](std::shared_ptr<IEvent> event) {
                    std::cout << "On event received!\n";
                    event->apply(parent);
                });

                manager.broadcast(std::make_shared<JumpEvent>());
                break;
        }
    }

    void onKeyRelease(Key key) override {
        switch (key) {
            case Key::A:
            case Key::LEFT:
                _movingLeft = false;
                break;
            case Key::D:
            case Key::RIGHT:
                _movingRight = false;
                break;
            default:
                break;
        }
    }

    void update() {
        if (!_physics || !_physics->isInitialized()) return;

        float vx = 0.0f;
        if (_movingLeft) {
            vx = -_moveSpeed;
        } else if (_movingRight) {
            vx = _moveSpeed;
        }

        float currentVx, currentVy;
        _physics->getVelocity(currentVx, currentVy);

        _physics->setVelocity(vx, currentVy);
    }
};

class Player : public GameObject {
private:
    PlayerController _controller;

public:
    void setup(PhysicsComponent *physics, InputSystem *inputSystem, GameObject *object) {
        _controller.setPhysicsComponent(physics);
        _controller.setObject(object);

        auto keyInput = std::make_unique<KeyInputComponent>(this);
        keyInput->setListener(&_controller);
        inputSystem->registerKeyComponent(keyInput.get());
        addComponent(std::move(keyInput));
    }

    void update(float deltaTime) {
        GameObject::update(deltaTime);
        _controller.update();
    }

    void onCollisionEnter(const CollisionData &collision) override {
        GameObject::onCollisionEnter(collision);

        if (collision.normalY > 0.2f) {
            _controller.addGroundContact();
        }
    }

    void onCollisionExit(const CollisionData &collision) override {
        _controller.removeGroundContact();
    }
};

int main() {
    try {
        PacketRegistery::getInstance().registerPacket<NetworkEventPacket>(100);

        EventRegistry::getInstance()->registerEvent("jump", []() {
            return std::make_shared<JumpEvent>();
        });

        std::unique_ptr<GameEngine> gameEngine = std::make_unique<GameEngine>();
        gameEngine->init("Physics Platformer Demo", 1280, 720);

        PhysicsSystem *physicsSystem = gameEngine->getSystem<PhysicsSystem>();
        InputSystem *inputSystem = gameEngine->getSystem<InputSystem>();

        if (!inputSystem) {
            return 1;
        }

        physicsSystem->setGravity(0.0f, 981.0f);

        auto scene = std::make_unique<Scene>("MainScene");
        auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
        auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
        scene->setCamera(std::move(camera));

        // Ground
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

        scene->addObject(std::move(ground));

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

        scene->addObject(std::move(platform));

        auto box = std::make_unique<GameObject>();
        box->getTransform()->getPosition()->setX(450.0f);
        box->getTransform()->getPosition()->setY(330.0f);
        box->getTransform()->getSize()->setWidth(60.0f);
        box->getTransform()->getSize()->setHeight(60.0f);

        auto boxPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
        boxPhysics->setBodyType(BodyType::DYNAMIC);
        boxPhysics->setCollider(std::make_unique<BoxCollider>(60.0f, 60.0f));
        boxPhysics->setMaterial(Material(50.0f, 0.8f, 0.0f));
        boxPhysics->setGravityScale(1.0f);
        boxPhysics->setParent(box.get());
        box->addComponent(std::move(boxPhysics));

        auto boxRenderer = std::make_unique<SpriteRenderer>("../external/GameEngine/resources/square.png");
        boxRenderer->setParent(box.get());
        box->addComponent(std::move(boxRenderer));

        scene->addObject(std::move(box));

        auto player = std::make_unique<Player>();
        player->getTransform()->getPosition()->setX(400.0f);
        player->getTransform()->getPosition()->setY(200.0f);
        player->getTransform()->getSize()->setWidth(50.0f);
        player->getTransform()->getSize()->setHeight(50.0f);

        auto playerPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
        playerPhysics->setBodyType(BodyType::DYNAMIC);
        playerPhysics->setCollider(std::make_unique<BoxCollider>(50.0f, 50.0f));
        playerPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
        playerPhysics->setGravityScale(1.0f);
        playerPhysics->setParent(player.get());

        auto *physicsPtr = playerPhysics.get();
        player->addComponent(std::move(playerPhysics));

        auto playerRenderer = std::make_unique<SpriteRenderer>("../external/GameEngine/resources/square_lime.png");
        playerRenderer->setParent(player.get());
        player->addComponent(std::move(playerRenderer));

        player->setup(physicsPtr, inputSystem, player.get());
        scene->addObject(std::move(player));

        auto hud = std::make_unique<HUD>();

        auto fpsCounter = std::make_unique<FPSCounter>();
        fpsCounter->setPosition(5.0f, 5.0f);
        fpsCounter->setSize(80.0f, 30.0f);
        fpsCounter->setFontSize(20);

        hud->setFPSCounter(std::move(fpsCounter));
        scene->setHUD(std::move(hud));

        gameEngine->getSystem<SceneSystem>()->addScene(std::move(scene));
        gameEngine->getSystem<SceneSystem>()->setScene("MainScene");
        gameEngine->start();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
