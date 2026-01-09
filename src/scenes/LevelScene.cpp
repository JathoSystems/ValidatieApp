#include "scenes/LevelScene.hpp"
#include "grid/LevelGrid.h"
#include "grid/GridManager.h"
#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "grid/GridRenderer.h"
#include "cell/Lava.hpp"
#include "cell/Water.hpp"
#include "diamond/RedDiamond.hpp"
#include "diamond/BlueDiamond.hpp"
#include "door/Door.hpp"
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

LevelScene::LevelScene(int levelNumber, bool isOnline, std::shared_ptr<NetworkSystem> network,
                       EventManager *eventManager)
    : Scene("level_" + std::to_string(levelNumber) + (isOnline ? "_online" : "")),
      _levelNumber(levelNumber),
      _isOnline(isOnline),
      _network(network),
      _eventManager(eventManager),
      _isInitialized(false),
      _batCreated(false),
      _batCount(0),
      _fireboy(nullptr),
      _watergirl(nullptr),
      _fireboyDiamondText(nullptr),
      _watergirlDiamondText(nullptr),
      _peopleAtDoor(0) {
}

LevelScene::~LevelScene() {
    cleanup();
}

void LevelScene::onInitialRender() {
    std::cout << "[LevelScene] Initialize started for level " << _levelNumber << std::endl;

    GameEngine *gameEngine = &GameEngine::getInstance();
    SceneSystem *sceneSystem = gameEngine->getSystem<SceneSystem>();
    
    // Only initialize if this scene is currently active
    if (sceneSystem) {
        Scene *activeScene = sceneSystem->getActiveSceneObj();
        std::cout << "[LevelScene] Active scene: " << (activeScene ? activeScene->getName() : "nullptr") 
                  << ", This scene: " << getName() << std::endl;
        
        if (!activeScene || activeScene->getName() != getName()) {
            std::cout << "[LevelScene] Scene is not active, skipping initialization" << std::endl;
            // Reset _isInitialized so we can initialize when scene becomes active
            _isInitialized = false;
            return;
        }
        
        std::cout << "[LevelScene] Scene is active, proceeding with initialization" << std::endl;
    }

    // If this is a reinitialization, perform cleanup first
    if (_isInitialized) {
        std::cout << "[LevelScene] Performing cleanup before reinitialization..." << std::endl;
        cleanup();
    }

    // Mark pointers as invalid during initialization
    _fireboyDiamondText = nullptr;
    _watergirlDiamondText = nullptr;
    _fireboy = nullptr;
    _watergirl = nullptr;

    std::cout << "[LevelScene] Creating grid..." << std::endl;
    createBasicLevelGrid();

    std::cout << "[LevelScene] Setting up level..." << std::endl;
    setupLevel();

    std::cout << "[LevelScene] Setting up characters..." << std::endl;
    setupCharacters();

    std::cout << "[LevelScene] Creating bat..." << std::endl;
    createBat();


    std::cout << "[LevelScene] Setting up HUD..." << std::endl;
    setupHUD();

    _isInitialized = true;
    std::cout << "[LevelScene] Initialize completed" << std::endl;
}

void LevelScene::cleanup() {
    std::cout << "[LevelScene] Cleanup: Clearing all references..." << std::endl;

    // Clear all raw pointers
    _fireboyDiamondText = nullptr;
    _watergirlDiamondText = nullptr;
    _fireboy = nullptr;
    _watergirl = nullptr;
    
    // Clear door pointers (doors are owned by the scene's object list)
    _doors.clear();

    // Unregister grid from GridManager
    std::string sceneName = getName();
    GridManager::unregisterGrid(sceneName);

    std::cout << "[LevelScene] Cleanup: Complete" << std::endl;
}

void LevelScene::setupHUD() {
    auto hud = std::make_unique<HUD>();

    // FPS Counter
    auto fpsCounter = std::make_unique<FPSCounter>();
    fpsCounter->setPosition(5.0f, 5.0f);
    fpsCounter->setSize(80.0f, 30.0f);
    fpsCounter->setFontSize(20);
    hud->setFPSCounter(std::move(fpsCounter));

    // Fireboy Diamond Counter
    auto fireboyDiamondText = std::make_unique<Text>("Fireboy: 0");
    fireboyDiamondText->setColor(std::make_unique<Color>(255, 100, 100)); // Rood
    fireboyDiamondText->setFontSize(24);
    auto fireboyDiamondObj = std::make_unique<GameObject>();
    _fireboyDiamondText = fireboyDiamondText.get(); // Bewaar pointer voor updates
    fireboyDiamondObj->addComponent(std::move(fireboyDiamondText));
    fireboyDiamondObj->getTransform()->getPosition()->setX(1050);
    fireboyDiamondObj->getTransform()->getPosition()->setY(20);
    fireboyDiamondObj->getTransform()->getSize()->setWidth(200);
    fireboyDiamondObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(fireboyDiamondObj));

    // Watergirl Diamond Counter
    auto watergirlDiamondText = std::make_unique<Text>("Watergirl: 0");
    watergirlDiamondText->setColor(std::make_unique<Color>(100, 100, 255)); // Blauw
    watergirlDiamondText->setFontSize(24);
    auto watergirlDiamondObj = std::make_unique<GameObject>();
    _watergirlDiamondText = watergirlDiamondText.get(); // Bewaar pointer voor updates
    watergirlDiamondObj->addComponent(std::move(watergirlDiamondText));
    watergirlDiamondObj->getTransform()->getPosition()->setX(1050);
    watergirlDiamondObj->getTransform()->getPosition()->setY(60);
    watergirlDiamondObj->getTransform()->getSize()->setWidth(200);
    watergirlDiamondObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(watergirlDiamondObj));

    setHUD(std::move(hud));
}

void LevelScene::onUpdate(float deltaTime) {
    // Only update if fully initialized
    if (!_isInitialized) {
        return;
    }

    updateDiamondCounters();
    checkDiamondCollisions();
    checkDoorCollisions();
}

Fireboy *getFireboy(Scene *scene) {
    auto &objects = scene->getObjects();
    for (auto &obj: objects) {
        if (!obj) {
            continue;
        }
        GameObject* objPtr = obj.get();
        if (!objPtr) {
            continue;
        }
        if (Fireboy *fireboy = dynamic_cast<Fireboy *>(objPtr)) {
            return fireboy;
        }
    }
    return nullptr;
}

Watergirl *getWatergirl(Scene *scene) {
    auto &objects = scene->getObjects();
    for (auto &obj: objects) {
        if (!obj) {
            continue;
        }
        GameObject* objPtr = obj.get();
        if (!objPtr) {
            continue;
        }
        if (Watergirl *watergirl = dynamic_cast<Watergirl *>(objPtr)) {
            return watergirl;
        }
    }
    return nullptr;
}

void LevelScene::checkDiamondCollisions() {
    // Null-check before dereferencing to prevent crashes during reinitialization
    if (!_fireboy && !_watergirl) {
        return; // Characters not yet initialized
    }

    // Get all game objects from scene
    auto &objects = getObjects();

    for (auto &obj: objects) {
        if (!obj) {
            continue; // Skip null objects
        }
        
        GameObject* objPtr = obj.get();
        if (!objPtr) {
            continue; // Skip if get() returns null
        }
        
        // Check if object is a diamond
        if (RedDiamond *redDiamond = dynamic_cast<RedDiamond *>(objPtr)) {
            redDiamond->checkCollisionWith(getFireboy(this));
        } else if (BlueDiamond *blueDiamond = dynamic_cast<BlueDiamond *>(objPtr)) {
            blueDiamond->checkCollisionWith(getWatergirl(this));
        }
    }
}

void LevelScene::updateDiamondCounters() {
    Fireboy *fire = _fireboy;
    if (!fire) fire = getFireboy(this);

    if (_fireboyDiamondText && fire) {
        int diamonds = fire->getDiamonds();
        _fireboyDiamondText->setText("Fireboy: " + std::to_string(diamonds));
    }

    Watergirl *water = _watergirl;
    if (!water) water = getWatergirl(this);
    if (_watergirlDiamondText && water) {
        int diamonds = water->getDiamonds();
        _watergirlDiamondText->setText("Watergirl: " + std::to_string(diamonds));
    }
}

void LevelScene::createBasicLevelGrid() {
    auto levelGrid = std::make_unique<LevelGrid>(32, 18, 40);
    
    for (int x = 0; x < 32; x++) {
        levelGrid->setCellType(x, 17, CellType::Ground);
    }
    levelGrid->setCellType(15, 16, CellType::RedDiamond);
    levelGrid->setCellType(18, 16, CellType::BlueDiamond);
    levelGrid->setCellType(3, 15, CellType::RedDoor);
    levelGrid->setCellType(22, 15, CellType::BlueDoor);

    for (int y = 0; y < 18; y++) {
        levelGrid->setCellType(0, y, CellType::Ground);
        levelGrid->setCellType(31, y, CellType::Ground);
    }

    for (int x = 5; x < 12; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
    }

    for (int x = 20; x < 27; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
        levelGrid->setCellType(x, 12, CellType::Lava);
    }

    for (int x = 12; x < 20; x++) {
        levelGrid->setCellType(x, 8, CellType::Ground);
    }

    // Register grid in GridManager (it will own the grid)
    std::string sceneName = getName();
    GridManager::registerGrid(sceneName, std::move(levelGrid));
}

void LevelScene::checkDoorCollisions() {
    // Only check door collisions if scene is fully initialized
    if (!_isInitialized) {
        return;
    }
    
    // Re-fetch character pointers each frame to ensure they're still valid
    // This avoids using stale pointers if characters are destroyed/recreated
    Fireboy* fire = getFireboy(this);
    Watergirl* water = getWatergirl(this);
    
    // Update cached pointers if we found valid characters
    if (fire) _fireboy = fire;
    if (water) _watergirl = water;
    
    // If cached pointers don't match, clear them
    if (_fireboy && _fireboy != fire) _fireboy = nullptr;
    if (_watergirl && _watergirl != water) _watergirl = nullptr;
    
    if (!fire && !water)
        return;

    // Use stored Door pointers and call type-specific methods to avoid dynamic_cast crashes
    for (Door* door : _doors) {
        if (!door) {
            continue; // Skip null pointers
        }
        
        // Only check collision if we have valid characters
        // Re-fetch character pointers right before use to ensure they're still valid
        if (fire && door->getColor() == "red") {
            // Verify fire is still in the scene before using it
            Fireboy* currentFire = getFireboy(this);
            if (currentFire == fire) {
                door->checkCollisionWithFireboy(fire);
            }
        }
        
        if (water && door->getColor() == "blue") {
            // Verify water is still in the scene before using it
            Watergirl* currentWater = getWatergirl(this);
            if (currentWater == water) {
                door->checkCollisionWithWatergirl(water);
            }
        }
    }
}

void LevelScene::setupLevel() {
    GameEngine *gameEngine = &GameEngine::getInstance();
    PhysicsSystem *physicsSystem = gameEngine->getSystem<PhysicsSystem>();

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

    // Create individual blocks for each cell type
    for (int x = 0; x < grid->getWidth(); ++x) {
        for (int y = 0; y < grid->getHeight(); ++y) {
            CellType type = grid->getCellType(x, y);
            
            if (type == CellType::Ground) {
                auto block = std::make_unique<GameObject>();
                block->getTransform()->getPosition()->setX(x * cellSize + cellSize / 2.0f);
                block->getTransform()->getPosition()->setY(y * cellSize + cellSize / 2.0f);
                block->getTransform()->getSize()->setWidth(cellSize);
                block->getTransform()->getSize()->setHeight(cellSize);
                block->setLayer(0);

                auto physics = std::make_unique<PhysicsComponent>(physicsSystem->getBox2DFacade());
                physics->setBodyType(BodyType::STATIC);
                physics->setCollider(std::make_unique<BoxCollider>(cellSize, cellSize));
                physics->setMaterial(Material(1.0f, 0.8f, 0.0f));

                PhysicsComponent *physicsPtr = physics.get();
                block->addComponent(std::move(physics));
                physicsSystem->registerComponent(physicsPtr);

                auto sprite = std::make_unique<SpriteRenderer>("resources/square.png");
                block->addComponent(std::move(sprite));

                addObject(std::move(block));
            }

            if (type == CellType::Water) {
                addObject(std::make_unique<Water>(grid, x, y));
            }

            if (type == CellType::Lava) {
                addObject(std::make_unique<Lava>(grid, x, y));
            }

            if (type == CellType::RedDiamond) {
                addObject(std::make_unique<RedDiamond>(grid, x, y));
            }

            if (type == CellType::BlueDiamond) {
                addObject(std::make_unique<BlueDiamond>(grid, x, y));
            }

            if (type == CellType::RedDoor) {
                auto door = std::make_unique<Door>(this, grid->getCellSize(), x, y);
                Door* doorPtr = door.get();
                _doors.push_back(doorPtr);
                addObject(std::move(door));
            }

            if (type == CellType::BlueDoor) {
                auto door = std::make_unique<Door>(this, grid->getCellSize(), x, y, "blue");
                Door* doorPtr = door.get();
                _doors.push_back(doorPtr);
                addObject(std::move(door));
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
            // Create the active fireboy (the one this client controls)
            auto fireboy = std::make_unique<Fireboy>(_network, _eventManager, gameEngine, true);
            _fireboy = fireboy.get(); // Store pointer to the active fireboy
            addObject(std::move(fireboy));
            
            // Create the networked fireboy with fixed ID
            auto fire = std::make_unique<Fireboy>(FIREBOY_ID, _network, _eventManager, gameEngine, true);
            addObject(std::move(fire));

            auto water = std::make_unique<Watergirl>(WATERGIRL_ID, _network, _eventManager, gameEngine, false);
            // Optional: Set initial off-screen position until sync packet arrives
            water->getTransform()->getPosition()->setX(600);
            water->getTransform()->getPosition()->setY(500);
            addObject(std::move(water));
        } else {
            // Create the active watergirl (the one this client controls)
            auto watergirl = std::make_unique<Watergirl>(_network, _eventManager, gameEngine, true);
            _watergirl = watergirl.get(); // Store pointer to the active watergirl
            addObject(std::move(watergirl));
            
            auto fire = std::make_unique<Fireboy>(FIREBOY_ID, _network, _eventManager, gameEngine, false);
            fire->getTransform()->getPosition()->setX(200);
            fire->getTransform()->getPosition()->setY(500);
            addObject(std::move(fire));

            auto water = std::make_unique<Watergirl>(WATERGIRL_ID, _network, _eventManager, gameEngine, true);
            addObject(std::move(water));
        }
    } else {
        auto fireboy = std::make_unique<Fireboy>(nullptr, _eventManager, gameEngine, true);
        fireboy->getTransform()->getPosition()->setX(200);
        fireboy->getTransform()->getPosition()->setY(500);
        _fireboy = fireboy.get(); // Bewaar pointer
        addObject(std::move(fireboy));

        auto watergirl = std::make_unique<Watergirl>(nullptr, _eventManager, gameEngine, true);
        watergirl->getTransform()->getPosition()->setX(400);
        watergirl->getTransform()->getPosition()->setY(500);
        _watergirl = watergirl.get(); // Bewaar pointer
        addObject(std::move(watergirl));
    }
}

void LevelScene::createBat() {
    // In multiplayer mode, only the authoritative client (fireboy) creates bats locally
    // Non-authoritative clients create bats from SpawnEvents
    if (_isOnline) {
        std::string role = GameState::getInstance().get("role");
        if (role != "fireboy") {
            // Non-authoritative client - don't create bat locally, wait for SpawnEvent
            return;
        }
    }

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

    _batCount++;
    int batId = _batCount;

    ObjectRegistry::getInstance().insert(bat.get(), batId);

    auto batAnimator = std::make_unique<Animator>("resources/bat/flying.png", 1, 8);
    bat->addComponent(std::move(batAnimator));

    bool isNetworked = (_network != nullptr);
    bool isAuthoritative = false;
    if (_isOnline) {
        std::string role = GameState::getInstance().get("role");
        isAuthoritative = (role == "fireboy");
    } else {
        isAuthoritative = true;
    }

    float batX = static_cast<float>(bat->getTransform()->getPosition()->getX());
    float batY = static_cast<float>(bat->getTransform()->getPosition()->getY());

    auto batAI = std::make_unique<BatAI>(bat.get(), grid, this, CELL_SIZE, 80.0f, isNetworked, _eventManager, batId, isAuthoritative);
    bat->addComponent(std::move(batAI));

    addObject(std::move(bat));

    if (_network && _eventManager) {
        _network->getMiddleware()->sendEvent(std::make_shared<SpawnEvent>(batId, "bat", batX, batY));
    }
}
