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

void LevelScene::onInitialRender() {
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


    // Create individual blocks for each ground cell
    int blockCount = 0;
    for (int x = 0; x < _levelGrid->getWidth(); ++x) {
        for (int y = 0; y < _levelGrid->getHeight(); ++y) {
            if (_levelGrid->getCellType(x, y) == CellType::Ground) {
                blockCount++;
                
                auto block = std::make_unique<GameObject>();
                block->getTransform()->getPosition()->setX(x * cellSize + cellSize / 2.0f);
                block->getTransform()->getPosition()->setY(y * cellSize + cellSize / 2.0f);
                block->getTransform()->getSize()->setWidth(cellSize);
                block->getTransform()->getSize()->setHeight(cellSize);
                block->setLayer(0); // Background layer

                auto physics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
                physics->setBodyType(BodyType::STATIC);
                physics->setCollider(std::make_unique<BoxCollider>(cellSize, cellSize));
                physics->setMaterial(Material(1.0f, 0.8f, 0.0f));

                PhysicsComponent* physicsPtr = physics.get();
                block->addComponent(std::move(physics));
                physicsSystem->registerComponent(physicsPtr);

                auto sprite = std::make_unique<SpriteRenderer>("resources/square.png");
                block->addComponent(std::move(sprite));

                addObject(std::move(block));
            }
        }
    }
    
    std::cout << "[LevelScene] Created " << blockCount << " individual blocks" << std::endl;
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
