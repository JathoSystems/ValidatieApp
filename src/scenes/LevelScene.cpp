#include "scenes/LevelScene.hpp"
#include "grid/LevelGrid.h"
#include "grid/GridManager.h"
#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "grid/GridRenderer.h"
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
#include "bat/Bat.h"
#include "bat/BatAI.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "SpawnEvent.hpp"

LevelScene::LevelScene(int levelNumber, bool isOnline, std::shared_ptr<NetworkSystem> network, EventManager* eventManager)
    : Scene("level_" + std::to_string(levelNumber) + (isOnline ? "_online" : "")),
      _levelNumber(levelNumber),
      _isOnline(isOnline),
      _network(network),
      _eventManager(eventManager),
      _batCreated(false) {
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
    
    std::cout << "[LevelScene] Creating bat..." << std::endl;
    createBat();
    
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
    auto levelGrid = std::make_unique<LevelGrid>(32, 18, 40);
    
    for (int x = 0; x < 32; x++) {
        levelGrid->setCellType(x, 17, CellType::Ground);
    }
    
    for (int y = 0; y < 18; y++) {
        levelGrid->setCellType(0, y, CellType::Ground);
        levelGrid->setCellType(31, y, CellType::Ground);
    }
    
    for (int x = 5; x < 12; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
    }
    
    for (int x = 20; x < 27; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
    }
    
    for (int x = 12; x < 20; x++) {
        levelGrid->setCellType(x, 8, CellType::Ground);
    }
    
    // Register grid in GridManager
    std::string sceneName = getName();
    GridManager::registerGrid(sceneName, std::move(levelGrid));
}

void LevelScene::setupLevel() {
    GameEngine* gameEngine = &GameEngine::getInstance();
    PhysicsSystem* physicsSystem = gameEngine->getSystem<PhysicsSystem>();

    physicsSystem->setGravity(0.0f, 981.0f);

    auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
    setCamera(std::move(camera));

    auto levelText = std::make_unique<Text>("Level " + std::to_string(_levelNumber) + (_isOnline ? " (Online)" : ""));
    levelText->setColor(std::make_unique<Color>(255, 255, 255));
    auto levelTextObj = std::make_unique<GameObject>();
    levelTextObj->addComponent(std::move(levelText));
    levelTextObj->getTransform()->getPosition()->setX(580);
    levelTextObj->getTransform()->getPosition()->setY(10);
    levelTextObj->getTransform()->getSize()->setWidth(200);
    levelTextObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(levelTextObj));

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

    std::string sceneName = getName();
    LevelGrid* grid = GridManager::getGrid(sceneName);
    if (!grid) {
        std::cout << "[LevelScene] Grid not found for scene: " << sceneName << std::endl;
        return;
    }
    
    int cellSize = grid->getCellSize();


    // Create individual blocks for each ground cell
    int blockCount = 0;
    for (int x = 0; x < grid->getWidth(); ++x) {
        for (int y = 0; y < grid->getHeight(); ++y) {
            if (grid->getCellType(x, y) == CellType::Ground) {
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
}

void LevelScene::setupCharacters() {
    GameEngine *gameEngine = &GameEngine::getInstance();

    // Define Fixed IDs so both clients agree
    const int FIREBOY_ID = 99;
    const int WATERGIRL_ID = 100;

    if (_isOnline) {
        std::string role = GameState::getInstance().get("role");

        if (role == "fireboy") {
            auto fireboy = std::make_unique<Fireboy>(FIREBOY_ID, _network, _eventManager, gameEngine, true);
            addObject(std::move(fireboy));

            auto watergirl = std::make_unique<Watergirl>(WATERGIRL_ID, _network, _eventManager, gameEngine, false);
            // Optional: Set initial off-screen position until sync packet arrives
            watergirl->getTransform()->getPosition()->setX(600);
            watergirl->getTransform()->getPosition()->setY(500);
            addObject(std::move(watergirl));
        } else {
            auto fireboy = std::make_unique<Fireboy>(FIREBOY_ID, _network, _eventManager, gameEngine, false);
            fireboy->getTransform()->getPosition()->setX(200);
            fireboy->getTransform()->getPosition()->setY(500);
            addObject(std::move(fireboy));

            auto watergirl = std::make_unique<Watergirl>(WATERGIRL_ID, _network, _eventManager, gameEngine, true);
            addObject(std::move(watergirl));
        }
    } else {
        auto fireboy = std::make_unique<Fireboy>(nullptr, _eventManager, gameEngine, true);
        fireboy->getTransform()->getPosition()->setX(200);
        fireboy->getTransform()->getPosition()->setY(500);
        addObject(std::move(fireboy));

        auto watergirl = std::make_unique<Watergirl>(nullptr, _eventManager, gameEngine, true);
        watergirl->getTransform()->getPosition()->setX(400);
        watergirl->getTransform()->getPosition()->setY(500);
        addObject(std::move(watergirl));
    }
}

void LevelScene::createBat() {
    if (_batCreated) return;

    std::string sceneName = getName();
    LevelGrid* grid = GridManager::getGrid(sceneName);
    if (!grid) {
        std::cout << "[LevelScene] Grid not found for scene: " << sceneName << std::endl;
        return;
    }

    const int CELL_SIZE = grid->getCellSize();
    const int GRID_WIDTH = grid->getWidth();
    const int GRID_HEIGHT = grid->getHeight();

    int startGridX = GRID_WIDTH / 2;
    int startGridY = GRID_HEIGHT / 2;
    bool foundStart = false;

    // Find a walkable position starting from center
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

    if (!foundStart) {
        std::cout << "[LevelScene] Could not find walkable position for bat" << std::endl;
        return;
    }

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
