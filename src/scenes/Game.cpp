//
// Created by jusra on 16-12-2025.
//

#include "scenes/Game.hpp"

#include "../../external/GameEngine/includes/Network/GameState.hpp"
#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Input/InputSystem.h"
#include "Physics/Collider.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Scenes/Camera/FixedCamera.h"
#include "grid/LevelGrid.h"
#include "grid/GridManager.h"
#include "bat/Bat.h"
#include "bat/BatAI.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "GameObjects/Spritesheet/Animator.h"
#include "SpawnEvent.hpp"
#include "GameObjectFactory.hpp"
#include "UI/Text.h"
#include "Scenes/SceneSystem.h"

Game::Game(std::shared_ptr<NetworkSystem> network, EventManager *eventManager) : Scene("Game") {
    _network = network;
    _eventManager = eventManager;
    _characterCreated = false;
    _batCreated = false;
    _isInitialized = false;
}

void Game::onInitialRender() {
    std::cout << "[Game] onInitialRender() called" << std::endl;
    
    GameEngine *gameEngine = &GameEngine::getInstance();
    PhysicsSystem *physicsSystem = gameEngine->getSystem<PhysicsSystem>();
    InputSystem *inputSystem = gameEngine->getSystem<InputSystem>();
    SceneSystem *sceneSystem = gameEngine->getSystem<SceneSystem>();

    if (!inputSystem || !physicsSystem || !sceneSystem) {
        std::cout << "[Game] Systems not available, returning" << std::endl;
        return;
    }
    
    Scene *activeScene = sceneSystem->getActiveSceneObj();
    std::cout << "[Game] Active scene: " << (activeScene ? activeScene->getName() : "nullptr") 
              << ", This scene: " << getName() << std::endl;
    
    if (!activeScene || activeScene->getName() != getName()) {
        std::cout << "[Game] Scene is not active, skipping initialization" << std::endl;
        _isInitialized = false;
        return;
    }
    
    std::cout << "[Game] Scene is active, proceeding with initialization" << std::endl;
    
    if (_isInitialized) {
        return;
    }

    physicsSystem->setGravity(0.0f, 981.0f);

    auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
    setCamera(std::move(camera));

    auto ground = std::make_unique<GameObject>();
    ground->getTransform()->getPosition()->setX(640.0f);
    ground->getTransform()->getPosition()->setY(670.0f);
    ground->getTransform()->getSize()->setWidth(1280.0f);
    ground->getTransform()->getSize()->setHeight(100.0f);

    auto groundPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    groundPhysics->setBodyType(BodyType::STATIC);
    groundPhysics->setCollider(std::make_unique<BoxCollider>());
    groundPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent *groundPhysicsPtr = groundPhysics.get();
    ground->addComponent(std::move(groundPhysics));
    physicsSystem->registerComponent(groundPhysicsPtr);

    auto groundRenderer = std::make_unique<SpriteRenderer>("resources/square.png");
    groundRenderer->setParent(ground.get());
    ground->addComponent(std::move(groundRenderer));

    addObject(std::move(ground));

    auto platform = std::make_unique<GameObject>();
    platform->getTransform()->getPosition()->setX(400.0f);
    platform->getTransform()->getPosition()->setY(400.0f);
    platform->getTransform()->getSize()->setWidth(300.0f);
    platform->getTransform()->getSize()->setHeight(50.0f);

    auto platformPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    platformPhysics->setBodyType(BodyType::STATIC);
    platformPhysics->setCollider(std::make_unique<BoxCollider>());
    platformPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent *platformPhysicsPtr = platformPhysics.get();
    platform->addComponent(std::move(platformPhysics));
    physicsSystem->registerComponent(platformPhysicsPtr);

    auto platformRenderer = std::make_unique<SpriteRenderer>("resources/square_blue.png");
    platformRenderer->setParent(platform.get());
    platform->addComponent(std::move(platformRenderer));

    addObject(std::move(platform));

    auto box = std::make_unique<GameObject>();
    box->getTransform()->getPosition()->setX(450.0f);
    box->getTransform()->getPosition()->setY(345.0f);
    box->getTransform()->getSize()->setWidth(60.0f);
    box->getTransform()->getSize()->setHeight(60.0f);

    auto boxPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    boxPhysics->setBodyType(BodyType::DYNAMIC);
    boxPhysics->setCollider(std::make_unique<BoxCollider>());
    boxPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    boxPhysics->setGravityScale(1.0f);
    PhysicsComponent *boxPhysicsPtr = boxPhysics.get();
    box->addComponent(std::move(boxPhysics));
    physicsSystem->registerComponent(boxPhysicsPtr);

    auto boxRenderer = std::make_unique<SpriteRenderer>("resources/square.png");
    boxRenderer->setParent(box.get());
    box->addComponent(std::move(boxRenderer));

    addObject(std::move(box));

    auto leftWall = std::make_unique<GameObject>();
    leftWall->getTransform()->getPosition()->setX(25.0f);
    leftWall->getTransform()->getPosition()->setY(360.0f);
    leftWall->getTransform()->getSize()->setWidth(50.0f);
    leftWall->getTransform()->getSize()->setHeight(720.0f);

    auto leftWallPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    leftWallPhysics->setBodyType(BodyType::STATIC);
    leftWallPhysics->setCollider(std::make_unique<BoxCollider>());
    leftWallPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent *leftWallPhysicsPtr = leftWallPhysics.get();
    leftWall->addComponent(std::move(leftWallPhysics));
    physicsSystem->registerComponent(leftWallPhysicsPtr);

    auto leftWallRenderer = std::make_unique<SpriteRenderer>("resources/square.png");
    leftWallRenderer->setParent(leftWall.get());
    leftWall->addComponent(std::move(leftWallRenderer));

    addObject(std::move(leftWall));

    auto rightWall = std::make_unique<GameObject>();
    rightWall->getTransform()->getPosition()->setX(1255.0f);
    rightWall->getTransform()->getPosition()->setY(360.0f);
    rightWall->getTransform()->getSize()->setWidth(50.0f);
    rightWall->getTransform()->getSize()->setHeight(720.0f);

    auto rightWallPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    rightWallPhysics->setBodyType(BodyType::STATIC);
    rightWallPhysics->setCollider(std::make_unique<BoxCollider>());
    rightWallPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent *rightWallPhysicsPtr = rightWallPhysics.get();
    rightWall->addComponent(std::move(rightWallPhysics));
    physicsSystem->registerComponent(rightWallPhysicsPtr);

    auto rightWallRenderer = std::make_unique<SpriteRenderer>("resources/square.png");
    rightWallRenderer->setParent(rightWall.get());
    rightWall->addComponent(std::move(rightWallRenderer));

    addObject(std::move(rightWall));

    auto roof = std::make_unique<GameObject>();
    roof->getTransform()->getPosition()->setX(640.0f);
    roof->getTransform()->getPosition()->setY(50.0f);
    roof->getTransform()->getSize()->setWidth(1280.0f);
    roof->getTransform()->getSize()->setHeight(100.0f);

    auto roofPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    roofPhysics->setBodyType(BodyType::STATIC);
    roofPhysics->setCollider(std::make_unique<BoxCollider>());
    roofPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent *roofPhysicsPtr = roofPhysics.get();
    roof->addComponent(std::move(roofPhysics));
    physicsSystem->registerComponent(roofPhysicsPtr);

    auto roofRenderer = std::make_unique<SpriteRenderer>("resources/square.png");
    roofRenderer->setParent(roof.get());
    roof->addComponent(std::move(roofRenderer));

    addObject(std::move(roof));

    setupGrid();
    createBat();
    createCharacter();

    auto hud = std::make_unique<HUD>();
    auto fpsCounter = std::make_unique<FPSCounter>();
    fpsCounter->setPosition(5.0f, 5.0f);
    fpsCounter->setSize(80.0f, 30.0f);
    fpsCounter->setFontSize(20);

    Fireboy *fireboy = nullptr;
    for (const std::unique_ptr<GameObject>& gameObject : getObjects()) {
        if (Fireboy *temp = dynamic_cast<Fireboy *>(gameObject.get())) {
            fireboy = temp;
        }
    }

    if (fireboy) {
        std::cout << "Adding!";
        auto gameObject = std::make_unique<GameObject>();
        auto red = std::make_unique<Text>(
            "Red: " + std::to_string(fireboy->getDiamonds())
        );

        gameObject->getTransform()->getPosition()->setX(5);
        gameObject->getTransform()->getPosition()->setY(40);
        gameObject->getTransform()->getSize()->setWidth(150);
        gameObject->getTransform()->getSize()->setHeight(30);
        red->setFontSize(20);

        hud->addObject(std::move(gameObject));
    }

    hud->setFPSCounter(std::move(fpsCounter));
    setHUD(std::move(hud));
    
    _isInitialized = true;
}

void Game::setupGrid() {
    const int CELL_SIZE = 10;
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE;
    const int GRID_HEIGHT = WINDOW_HEIGHT / CELL_SIZE;

    auto levelGrid = std::make_unique<LevelGrid>(GRID_WIDTH, GRID_HEIGHT, CELL_SIZE);
    LevelGrid* gridPtr = levelGrid.get();

    auto markCellsAsGround = [gridPtr, CELL_SIZE](float posX, float posY, float width, float height) {
        float left = posX - width / 2.0f;
        float top = posY - height / 2.0f;
        float right = posX + width / 2.0f;
        float bottom = posY + height / 2.0f;

        float epsilon = 0.01f;
        int gridLeft = static_cast<int>(std::floor(left / CELL_SIZE));
        int gridTop = static_cast<int>(std::floor(top / CELL_SIZE));
        int gridRight = static_cast<int>(std::floor((right - epsilon) / CELL_SIZE));
        int gridBottom = static_cast<int>(std::floor((bottom - epsilon) / CELL_SIZE));

        gridLeft = std::max(0, std::min(gridLeft, gridPtr->getWidth() - 1));
        gridTop = std::max(0, std::min(gridTop, gridPtr->getHeight() - 1));
        gridRight = std::max(0, std::min(gridRight, gridPtr->getWidth() - 1));
        gridBottom = std::max(0, std::min(gridBottom, gridPtr->getHeight() - 1));

        for (int x = gridLeft; x <= gridRight; ++x) {
            for (int y = gridTop; y <= gridBottom; ++y) {
                gridPtr->setCellType(x, y, CellType::Ground);
            }
        }
    };

    markCellsAsGround(640.0f, 670.0f, 1280.0f, 100.0f);
    markCellsAsGround(400.0f, 400.0f, 300.0f, 50.0f);
    markCellsAsGround(25.0f, 360.0f, 50.0f, 720.0f);
    markCellsAsGround(1255.0f, 360.0f, 50.0f, 720.0f);
    markCellsAsGround(640.0f, 50.0f, 1280.0f, 100.0f);

    GridManager::registerGrid("Game", std::move(levelGrid));
}

void Game::createBat() {
    if (_batCreated) return;

    LevelGrid* grid = GridManager::getGrid("Game");
    if (!grid) return;

    const int CELL_SIZE = 10;
    const int GRID_WIDTH = 1280 / CELL_SIZE;
    const int GRID_HEIGHT = 720 / CELL_SIZE;

    int startGridX = GRID_WIDTH / 2;
    int startGridY = GRID_HEIGHT / 2;
    bool foundStart = false;

    for (int radius = 0; radius < std::min(GRID_WIDTH, GRID_HEIGHT) / 2 && !foundStart; ++radius) {
        for (int y = startGridY - radius; y <= startGridY + radius && !foundStart; ++y) {
            for (int x = startGridX - radius; x <= startGridX + radius && !foundStart; ++x) {
                if (x > startGridX - radius && x < startGridX + radius &&
                    y > startGridY - radius && y < startGridY + radius) {
                    continue;
                }

                if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
                    if (grid->isWalkable(x, y)) {
                        startGridX = x;
                        startGridY = y;
                        foundStart = true;
                    }
                }
            }
        }
    }

    if (!foundStart) return;

    auto bat = std::make_unique<Bat>(grid, CELL_SIZE, 80.0f); // Faster speed

    float worldX, worldY;
    grid->gridToWorld(startGridX, startGridY, worldX, worldY);
    worldX += CELL_SIZE / 2.0f;
    worldY += CELL_SIZE / 2.0f;

    bat->getTransform()->getPosition()->setX(static_cast<int>(worldX));
    bat->getTransform()->getPosition()->setY(static_cast<int>(worldY));

    const int BAT_SIZE = CELL_SIZE;
    bat->getTransform()->getSize()->setWidth(BAT_SIZE);
    bat->getTransform()->getSize()->setHeight(BAT_SIZE);

    int batId = ObjectRegistry::getInstance().registerObject(bat.get());

    auto batAnimator = std::make_unique<Animator>("resources/bat/flying.png", 1, 8);
    bat->addComponent(std::move(batAnimator));

    bool isNetworked = (_network != nullptr);
    bool isAuthoritative = true;
    auto batAI = std::make_unique<BatAI>(bat.get(), grid, this, CELL_SIZE, 80.0f, isNetworked, _eventManager, batId, isAuthoritative);
    bat->addComponent(std::move(batAI));

    addObject(std::move(bat));
    _batCreated = true;

    if (_network && _eventManager) {
        _network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(batId, "bat"));
    }
}

void Game::createCharacter() {
    if (_characterCreated) return;

    GameEngine* gameEngine = &GameEngine::getInstance();
    std::string characterState = GameState::getInstance().get("role");

    if (characterState.empty()) {
        return;
    }

    std::unique_ptr<BaseCharacter> character = nullptr;

    if (characterState == "fireboy") {
        character = std::make_unique<Fireboy>(_network, _eventManager, gameEngine, true);
    } else if (characterState == "watergirl") {
        character = std::make_unique<Watergirl>(_network, _eventManager, gameEngine, true);
    } else {
        return;
    }

    if (character) {
        int characterId = ObjectRegistry::getInstance().registerObject(character.get());

        BaseCharacterController* controller = character->getController();
        if (controller) {
            controller->setParentId(characterId);
        }

        auto* pos = character->getTransform()->getPosition();
        if (pos) {
            pos->setX(characterState == "fireboy" ? 200 : 600);
            pos->setY(500);
        }

        auto* size = character->getTransform()->getSize();
        if (size) {
            size->setWidth(50);
            size->setHeight(100);
        }

        std::string idleSprite = characterState == "fireboy"
            ? "resources/fireboy/idle.png"
            : "resources/watergirl/idle.png";

        auto animator = std::make_unique<Animator>(idleSprite, 1, 5);
        character->addComponent(std::move(animator));

        if (_network && _eventManager) {
            _network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(characterId, characterState));
        }

        addObject(std::move(character));
        _characterCreated = true;
    }
}

void Game::update(float deltaTime) {
    if (!_characterCreated) {
        createCharacter();
    }
}
