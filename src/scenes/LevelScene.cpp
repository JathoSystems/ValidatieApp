#include "scenes/LevelScene.hpp"
#include "LevelGrid.h"
#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "GridRenderer.h"
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Input/InputSystem.h"
#include "Physics/Collider.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"
#include "Scenes/Camera/FixedCamera.h"
#include "Scenes/SceneSystem.h"
#include "UI/Button.h"
#include "UI/Text.h"
#include "UI/FPSCounter.h"
#include "Network/GameState.hpp"

LevelScene::LevelScene(int levelNumber, bool isOnline, std::shared_ptr<NetworkSystem> network, EventManager* eventManager)
    : Scene("level_" + std::to_string(levelNumber) + (isOnline ? "_online" : "")),
      _levelNumber(levelNumber),
      _isOnline(isOnline),
      _network(network),
      _eventManager(eventManager) {
}

LevelScene::~LevelScene() = default;

void LevelScene::initialize() {
    std::cout << "[LevelScene] Initialize started for level " << _levelNumber << std::endl;
    
    std::cout << "[LevelScene] Creating grid..." << std::endl;
    createBasicLevelGrid();
    
    std::cout << "[LevelScene] Setting up level..." << std::endl;
    setupLevel();
    
    std::cout << "[LevelScene] Setting up characters..." << std::endl;
    setupCharacters();
    
    std::cout << "[LevelScene] Setting up HUD..." << std::endl;
    auto hud = std::make_unique<HUD>();
    auto fpsCounter = std::make_unique<FPSCounter>();
    fpsCounter->setPosition(5.0f, 5.0f);
    fpsCounter->setSize(80.0f, 30.0f);
    fpsCounter->setFontSize(20);
    hud->setFPSCounter(std::move(fpsCounter));
    setHUD(std::move(hud));
    
    std::cout << "[LevelScene] Initialize completed" << std::endl;
}

void LevelScene::createBasicLevelGrid() {
    _levelGrid = std::make_unique<LevelGrid>(32, 18, 40);
    
    for (int x = 0; x < 32; x++) {
        _levelGrid->setCellType(x, 17, CellType::Ground);
    }
    
    for (int y = 0; y < 18; y++) {
        _levelGrid->setCellType(0, y, CellType::Ground);
        _levelGrid->setCellType(31, y, CellType::Ground);
    }
    
    for (int x = 5; x < 12; x++) {
        _levelGrid->setCellType(x, 12, CellType::Ground);
    }
    
    for (int x = 20; x < 27; x++) {
        _levelGrid->setCellType(x, 12, CellType::Ground);
    }
    
    for (int x = 12; x < 20; x++) {
        _levelGrid->setCellType(x, 8, CellType::Ground);
    }
}

void LevelScene::setupLevel() {
    std::cout << "[LevelScene] Getting GameEngine..." << std::endl;
    GameEngine* gameEngine = &GameEngine::getInstance();
    std::cout << "[LevelScene] Getting PhysicsSystem..." << std::endl;
    PhysicsSystem* physicsSystem = gameEngine->getSystem<PhysicsSystem>();

    std::cout << "[LevelScene] Setting gravity..." << std::endl;
    physicsSystem->setGravity(0.0f, 981.0f);

    std::cout << "[LevelScene] Creating camera..." << std::endl;
    auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
    setCamera(std::move(camera));

    std::cout << "[LevelScene] Adding level text..." << std::endl;
    auto levelText = std::make_unique<Text>("Level " + std::to_string(_levelNumber) + (_isOnline ? " (Online)" : ""));
    levelText->setColor(std::make_unique<Color>(255, 255, 255));
    auto levelTextObj = std::make_unique<GameObject>();
    levelTextObj->addComponent(std::move(levelText));
    levelTextObj->getTransform()->getPosition()->setX(580);
    levelTextObj->getTransform()->getPosition()->setY(10);
    levelTextObj->getTransform()->getSize()->setWidth(200);
    levelTextObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(levelTextObj));

    std::cout << "[LevelScene] Adding back button..." << std::endl;
    auto backButton = std::make_unique<Button>("Back", std::make_unique<Color>(255, 100, 100));
    backButton->setOnClick([]() {
        GameEngine::getInstance().getSystem<SceneSystem>()->setScene("level_selector");
    });
    auto backButtonObj = std::make_unique<GameObject>();
    backButtonObj->addComponent(std::move(backButton));
    backButtonObj->getTransform()->getPosition()->setX(20);
    backButtonObj->getTransform()->getPosition()->setY(20);
    backButtonObj->getTransform()->getSize()->setWidth(80);
    backButtonObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(backButtonObj));

    std::cout << "[LevelScene] Creating physics blocks..." << std::endl;
    int cellSize = _levelGrid->getCellSize();
    
    // Use combined colliders for level geometry instead of individual blocks
    // This avoids the 70+ object creation issue
    
    // Bottom ground (full width)
    auto groundBlock = std::make_unique<GameObject>();
    groundBlock->getTransform()->getPosition()->setX(640);  // Center of screen
    groundBlock->getTransform()->getPosition()->setY(17 * cellSize + cellSize / 2.0f);
    groundBlock->getTransform()->getSize()->setWidth(32 * cellSize);
    groundBlock->getTransform()->getSize()->setHeight(cellSize);
    
    auto groundPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    groundPhysics->setBodyType(BodyType::STATIC);
    groundPhysics->setCollider(std::make_unique<BoxCollider>(32 * cellSize, cellSize));
    groundPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent* groundPhysicsPtr = groundPhysics.get();
    groundBlock->addComponent(std::move(groundPhysics));
    physicsSystem->registerComponent(groundPhysicsPtr);
    addObject(std::move(groundBlock));
    
    // Left wall
    auto leftWall = std::make_unique<GameObject>();
    leftWall->getTransform()->getPosition()->setX(cellSize / 2.0f);
    leftWall->getTransform()->getPosition()->setY(360);
    leftWall->getTransform()->getSize()->setWidth(cellSize);
    leftWall->getTransform()->getSize()->setHeight(18 * cellSize);
    
    auto leftPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    leftPhysics->setBodyType(BodyType::STATIC);
    leftPhysics->setCollider(std::make_unique<BoxCollider>(cellSize, 18 * cellSize));
    leftPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent* leftPhysicsPtr = leftPhysics.get();
    leftWall->addComponent(std::move(leftPhysics));
    physicsSystem->registerComponent(leftPhysicsPtr);
    addObject(std::move(leftWall));
    
    // Right wall
    auto rightWall = std::make_unique<GameObject>();
    rightWall->getTransform()->getPosition()->setX(31 * cellSize + cellSize / 2.0f);
    rightWall->getTransform()->getPosition()->setY(360);
    rightWall->getTransform()->getSize()->setWidth(cellSize);
    rightWall->getTransform()->getSize()->setHeight(18 * cellSize);
    
    auto rightPhysics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    rightPhysics->setBodyType(BodyType::STATIC);
    rightPhysics->setCollider(std::make_unique<BoxCollider>(cellSize, 18 * cellSize));
    rightPhysics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent* rightPhysicsPtr = rightPhysics.get();
    rightWall->addComponent(std::move(rightPhysics));
    physicsSystem->registerComponent(rightPhysicsPtr);
    addObject(std::move(rightWall));
    
    // Platform 1 (left, y=12)
    auto platform1 = std::make_unique<GameObject>();
    platform1->getTransform()->getPosition()->setX(8.5f * cellSize);
    platform1->getTransform()->getPosition()->setY(12 * cellSize + cellSize / 2.0f);
    platform1->getTransform()->getSize()->setWidth(7 * cellSize);
    platform1->getTransform()->getSize()->setHeight(cellSize);
    
    auto plat1Physics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    plat1Physics->setBodyType(BodyType::STATIC);
    plat1Physics->setCollider(std::make_unique<BoxCollider>(7 * cellSize, cellSize));
    plat1Physics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent* plat1PhysicsPtr = plat1Physics.get();
    platform1->addComponent(std::move(plat1Physics));
    physicsSystem->registerComponent(plat1PhysicsPtr);
    addObject(std::move(platform1));
    
    // Platform 2 (right, y=12)
    auto platform2 = std::make_unique<GameObject>();
    platform2->getTransform()->getPosition()->setX(23.5f * cellSize);
    platform2->getTransform()->getPosition()->setY(12 * cellSize + cellSize / 2.0f);
    platform2->getTransform()->getSize()->setWidth(7 * cellSize);
    platform2->getTransform()->getSize()->setHeight(cellSize);
    
    auto plat2Physics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    plat2Physics->setBodyType(BodyType::STATIC);
    plat2Physics->setCollider(std::make_unique<BoxCollider>(7 * cellSize, cellSize));
    plat2Physics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent* plat2PhysicsPtr = plat2Physics.get();
    platform2->addComponent(std::move(plat2Physics));
    physicsSystem->registerComponent(plat2PhysicsPtr);
    addObject(std::move(platform2));
    
    // Platform 3 (center, y=8)
    auto platform3 = std::make_unique<GameObject>();
    platform3->getTransform()->getPosition()->setX(16 * cellSize);
    platform3->getTransform()->getPosition()->setY(8 * cellSize + cellSize / 2.0f);
    platform3->getTransform()->getSize()->setWidth(8 * cellSize);
    platform3->getTransform()->getSize()->setHeight(cellSize);
    
    auto plat3Physics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
    plat3Physics->setBodyType(BodyType::STATIC);
    plat3Physics->setCollider(std::make_unique<BoxCollider>(8 * cellSize, cellSize));
    plat3Physics->setMaterial(Material(1.0f, 0.8f, 0.0f));
    PhysicsComponent* plat3PhysicsPtr = plat3Physics.get();
    platform3->addComponent(std::move(plat3Physics));
    physicsSystem->registerComponent(plat3PhysicsPtr);
    addObject(std::move(platform3));
    
    std::cout << "[LevelScene] Created 6 combined colliders" << std::endl;
    
    // Add grid renderer to visualize the level (renders all ground cells)
    std::cout << "[LevelScene] Adding grid renderer..." << std::endl;
    auto gridRendererObj = std::make_unique<GameObject>();
    gridRendererObj->setLayer(0); // Render behind other objects
    gridRendererObj->addComponent(std::make_unique<GridRendererComponent>(_levelGrid.get()));
    addObject(std::move(gridRendererObj));
    
    std::cout << "[LevelScene] Level setup complete" << std::endl;
}

void LevelScene::setupCharacters() {
    GameEngine* gameEngine = &GameEngine::getInstance();

    if (_isOnline) {
        std::string role = GameState::getInstance().get("role");
        if (role == "fireboy") {
            auto fireboy = std::make_unique<Fireboy>(_network, _eventManager, gameEngine, true);
            addObject(std::move(fireboy));
        } else {
            auto watergirl = std::make_unique<Watergirl>(_network, _eventManager, gameEngine, true);
            addObject(std::move(watergirl));
        }
    } else {
        auto fireboy = std::make_unique<Fireboy>(_network, _eventManager, gameEngine, true);
        fireboy->getTransform()->getPosition()->setX(200);
        fireboy->getTransform()->getPosition()->setY(500);
        addObject(std::move(fireboy));

        auto watergirl = std::make_unique<Watergirl>(_network, _eventManager, gameEngine, true);
        watergirl->getTransform()->getPosition()->setX(400);
        watergirl->getTransform()->getPosition()->setY(500);
        addObject(std::move(watergirl));
    }
}
