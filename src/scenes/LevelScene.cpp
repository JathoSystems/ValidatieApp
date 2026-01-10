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
#include "server/GlobalFlags.h"

// FIX 1: Definieer de statische lijst (anders krijg je een linker error)
std::vector<SpawnEvent> SpawnEvent::_pendingEvents;

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

    if (sceneSystem) {
        Scene *activeScene = sceneSystem->getActiveSceneObj();
        std::cout << "[LevelScene] Active scene: " << (activeScene ? activeScene->getName() : "nullptr")
                  << ", This scene: " << getName() << std::endl;

        if (!activeScene || activeScene->getName() != getName()) {
            std::cout << "[LevelScene] Scene is not active, skipping initialization" << std::endl;
            _isInitialized = false;
            return;
        }

        std::cout << "[LevelScene] Scene is active, proceeding with initialization" << std::endl;
    }

    if (_isInitialized) {
        std::cout << "[LevelScene] Already initialized, skipping reinitialization" << std::endl;
        return;  // ← Don't cleanup and reinitialize!
    }

    _fireboyDiamondText = nullptr;
    _watergirlDiamondText = nullptr;
    _fireboy = nullptr;
    _watergirl = nullptr;
    _peopleAtDoor = 0;

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

    GlobalFlags::isLevelCleaning = false;
    SpawnEvent::processPending();

    std::cout << "[LevelScene] Initialize completed" << std::endl;
}


void LevelScene::cleanup() {
    if (!_isInitialized) return;

    std::cout << "[LevelScene] Cleanup: Clearing all references..." << std::endl;
    GlobalFlags::isLevelCleaning = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ObjectRegistry::getInstance().removeObject(99);
    ObjectRegistry::getInstance().removeObject(100);
    for(int i = 1; i <= _batCount; i++) {
        ObjectRegistry::getInstance().removeObject(i);
    }
    _batCount = 0;

    if (_hud) {
        _hud->clear();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    _objects.clear();

    // 4. Events wissen
    SpawnEvent::_pendingEvents.clear();

    // 5. Grid verwijderen
    std::string sceneName = getName();
    GridManager::unregisterGrid(sceneName);

    _fireboyDiamondText = nullptr;
    _watergirlDiamondText = nullptr;
    _fireboy = nullptr;
    _watergirl = nullptr;
    _doors.clear();
    _peopleAtDoor = 0;

    _isInitialized = false;
    std::cout << "[LevelScene] Cleanup: Complete" << std::endl;
}

void LevelScene::setupHUD() {
    auto hud = std::make_unique<HUD>();
    auto fpsCounter = std::make_unique<FPSCounter>();
    fpsCounter->setPosition(5.0f, 5.0f);
    fpsCounter->setSize(80.0f, 30.0f);
    fpsCounter->setFontSize(20);
    hud->setFPSCounter(std::move(fpsCounter));

    auto fireboyDiamondText = std::make_unique<Text>("Fireboy: 0");
    fireboyDiamondText->setColor(std::make_unique<Color>(255, 100, 100));
    fireboyDiamondText->setFontSize(24);
    auto fireboyDiamondObj = std::make_unique<GameObject>();
    _fireboyDiamondText = fireboyDiamondText.get();
    fireboyDiamondObj->addComponent(std::move(fireboyDiamondText));
    fireboyDiamondObj->getTransform()->getPosition()->setX(1050);
    fireboyDiamondObj->getTransform()->getPosition()->setY(20);
    fireboyDiamondObj->getTransform()->getSize()->setWidth(200);
    fireboyDiamondObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(fireboyDiamondObj));

    auto watergirlDiamondText = std::make_unique<Text>("Watergirl: 0");
    watergirlDiamondText->setColor(std::make_unique<Color>(100, 100, 255));
    watergirlDiamondText->setFontSize(24);
    auto watergirlDiamondObj = std::make_unique<GameObject>();
    _watergirlDiamondText = watergirlDiamondText.get();
    watergirlDiamondObj->addComponent(std::move(watergirlDiamondText));
    watergirlDiamondObj->getTransform()->getPosition()->setX(1050);
    watergirlDiamondObj->getTransform()->getPosition()->setY(60);
    watergirlDiamondObj->getTransform()->getSize()->setWidth(200);
    watergirlDiamondObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(watergirlDiamondObj));

    setHUD(std::move(hud));
}

void LevelScene::onUpdate(float deltaTime) {
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
        if (!obj) continue;
        if (Fireboy *fireboy = dynamic_cast<Fireboy *>(obj.get())) {
            return fireboy;
        }
    }
    return nullptr;
}

Watergirl *getWatergirl(Scene *scene) {
    auto &objects = scene->getObjects();
    for (auto &obj: objects) {
        if (!obj) continue;
        if (Watergirl *watergirl = dynamic_cast<Watergirl *>(obj.get())) {
            return watergirl;
        }
    }
    return nullptr;
}

void LevelScene::checkDiamondCollisions() {
    if (!_fireboy && !_watergirl) return;
    auto &objects = getObjects();
    for (size_t i = 0; i < objects.size(); i++) {
        if (!objects[i]) continue;
        if (RedDiamond *redDiamond = dynamic_cast<RedDiamond *>(objects[i].get())) {
            redDiamond->checkCollisionWith(getFireboy(this));
        } else if (BlueDiamond *blueDiamond = dynamic_cast<BlueDiamond *>(objects[i].get())) {
            blueDiamond->checkCollisionWith(getWatergirl(this));
        }
    }
}

void LevelScene::resetLevel() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "[LevelScene] SOFT RESET STARTING" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // 1. Reset flags
    GlobalFlags::isLevelCleaning = false;
    _levelFinished = false;
    _peopleAtDoor = 0;
    std::cout << "[RESET] Flags reset" << std::endl;

    // 2. Find ALL characters in the scene (both local and remote controlled)
    std::vector<Fireboy*> fireboys;
    std::vector<Watergirl*> watergirls;

    auto& objects = getObjects();
    for (auto& obj : objects) {
        if (!obj) continue;

        if (Fireboy* fire = dynamic_cast<Fireboy*>(obj.get())) {
            fireboys.push_back(fire);
        }
        else if (Watergirl* water = dynamic_cast<Watergirl*>(obj.get())) {
            watergirls.push_back(water);
        }
    }

    std::cout << "[RESET] Found " << fireboys.size() << " Fireboy(s) and "
              << watergirls.size() << " Watergirl(s)" << std::endl;

    // 3. Reset ALL Fireboys to their start position
    for (Fireboy* fire : fireboys) {
        if (!fire) continue;

        fire->getTransform()->getPosition()->setX(200);
        fire->getTransform()->getPosition()->setY(500);
        fire->resetDiamonds();

        if (auto* physics = fire->getComponent<PhysicsComponent>()) {
            physics->setVelocity(0.0f, 0.0f);
        }

        _fireboy = fire; // Update pointer
        std::cout << "[RESET] Reset Fireboy to (200, 500)" << std::endl;
    }

    // 4. Reset ALL Watergirls to their start position
    for (Watergirl* water : watergirls) {
        if (!water) continue;

        water->getTransform()->getPosition()->setX(400);
        water->getTransform()->getPosition()->setY(500);
        water->resetDiamonds();

        if (auto* physics = water->getComponent<PhysicsComponent>()) {
            physics->setVelocity(0.0f, 0.0f);
        }

        _watergirl = water; // Update pointer
        std::cout << "[RESET] Reset Watergirl to (400, 500)" << std::endl;
    }

    // 5. Collect diamond positions from grid
    std::cout << "[RESET] Collecting diamond positions from grid..." << std::endl;
    std::vector<std::pair<int, int>> redDiamondPositions;
    std::vector<std::pair<int, int>> blueDiamondPositions;

    LevelGrid* grid = GridManager::getGrid(getName());
    if (grid) {
        for (int x = 0; x < grid->getWidth(); ++x) {
            for (int y = 0; y < grid->getHeight(); ++y) {
                CellType type = grid->getCellType(x, y);
                if (type == CellType::RedDiamond) {
                    redDiamondPositions.push_back({x, y});
                }
                else if (type == CellType::BlueDiamond) {
                    blueDiamondPositions.push_back({x, y});
                }
            }
        }
    }
    std::cout << "[RESET] Found " << redDiamondPositions.size() << " red and "
              << blueDiamondPositions.size() << " blue diamond positions" << std::endl;

    // 6. Remove all existing diamonds
    std::cout << "[RESET] Removing old diamonds..." << std::endl;
    objects.erase(
        std::remove_if(objects.begin(), objects.end(), [](const std::unique_ptr<GameObject>& obj) {
            if (!obj) return false;
            return dynamic_cast<RedDiamond*>(obj.get()) != nullptr ||
                   dynamic_cast<BlueDiamond*>(obj.get()) != nullptr;
        }),
        objects.end()
    );

    // 7. Respawn all diamonds
    std::cout << "[RESET] Respawning diamonds..." << std::endl;
    for (const auto& pos : redDiamondPositions) {
        auto diamond = std::make_unique<RedDiamond>(grid, pos.first, pos.second);
        addObject(std::move(diamond));
    }

    for (const auto& pos : blueDiamondPositions) {
        auto diamond = std::make_unique<BlueDiamond>(grid, pos.first, pos.second);
        addObject(std::move(diamond));
    }

    // 8. Re-register and reset all doors
    std::cout << "[RESET] Resetting doors..." << std::endl;
    _doors.clear();
    for (auto& obj : objects) {
        if (!obj) continue;
        if (Door* door = dynamic_cast<Door*>(obj.get())) {
            _doors.push_back(door);
            door->resetOccupied();
            std::cout << "[RESET] Reset " << door->getColor() << " door" << std::endl;
        }
    }

    // 9. Update UI
    updateDiamondCounters();

    std::cout << "[RESET] COMPLETE - Level ready to play again!" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void LevelScene::updateDiamondCounters() {
    Fireboy *fire = _fireboy;
    if (!fire) fire = getFireboy(this);
    if (_fireboyDiamondText && fire) {
        _fireboyDiamondText->setText("Fireboy: " + std::to_string(fire->getDiamonds()));
    }
    Watergirl *water = _watergirl;
    if (!water) water = getWatergirl(this);
    if (_watergirlDiamondText && water) {
        _watergirlDiamondText->setText("Watergirl: " + std::to_string(water->getDiamonds()));
    }
}

void LevelScene::createBasicLevelGrid() {
    auto levelGrid = std::make_unique<LevelGrid>(32, 18, 40);
    for (int x = 0; x < 32; x++) levelGrid->setCellType(x, 17, CellType::Ground);
    levelGrid->setCellType(15, 16, CellType::RedDiamond);
    levelGrid->setCellType(18, 16, CellType::BlueDiamond);
    levelGrid->setCellType(3, 15, CellType::RedDoor);
    levelGrid->setCellType(22, 15, CellType::BlueDoor);
    for (int y = 0; y < 18; y++) {
        levelGrid->setCellType(0, y, CellType::Ground);
        levelGrid->setCellType(31, y, CellType::Ground);
    }
    for (int x = 5; x < 12; x++) levelGrid->setCellType(x, 12, CellType::Ground);
    for (int x = 20; x < 27; x++) {
        levelGrid->setCellType(x, 12, CellType::Ground);
        levelGrid->setCellType(x, 12, CellType::Lava);
    }
    for (int x = 12; x < 20; x++) levelGrid->setCellType(x, 8, CellType::Ground);
    GridManager::registerGrid(getName(), std::move(levelGrid));
}

void LevelScene::checkDoorCollisions() {
    if (!_isInitialized) return;
    Fireboy* fire = getFireboy(this);
    Watergirl* water = getWatergirl(this);
    if (fire) _fireboy = fire;
    if (water) _watergirl = water;
    if (!fire && !water) return;

    for (Door* door : _doors) {
        if (!door) continue;
        if (fire && door->getColor() == "red") door->checkCollisionWithFireboy(fire);
        if (water && door->getColor() == "blue") door->checkCollisionWithWatergirl(water);
    }
}

void LevelScene::setupLevel() {
    GameEngine *gameEngine = &GameEngine::getInstance();
    PhysicsSystem *physicsSystem = gameEngine->getSystem<PhysicsSystem>();
    physicsSystem->setGravity(0.0f, 981.0f);

    auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    setCamera(std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360)));

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

    LevelGrid* grid = GridManager::getGrid(getName());
    if (!grid) return;

    int cellSize = grid->getCellSize();
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
            if (type == CellType::Water) addObject(std::make_unique<Water>(grid, x, y));
            if (type == CellType::Lava) addObject(std::make_unique<Lava>(grid, x, y));
            if (type == CellType::RedDiamond) addObject(std::make_unique<RedDiamond>(grid, x, y));
            if (type == CellType::BlueDiamond) addObject(std::make_unique<BlueDiamond>(grid, x, y));
            if (type == CellType::RedDoor) {
                auto door = std::make_unique<Door>(this, grid->getCellSize(), x, y);
                _doors.push_back(door.get());
                addObject(std::move(door));
            }
            if (type == CellType::BlueDoor) {
                auto door = std::make_unique<Door>(this, grid->getCellSize(), x, y, "blue");
                _doors.push_back(door.get());
                addObject(std::move(door));
            }
        }
    }
}

void LevelScene::setupCharacters() {
    GameEngine *gameEngine = &GameEngine::getInstance();
    const int FIREBOY_ID = 99;
    const int WATERGIRL_ID = 100;

    if (_isOnline) {
        std::string role = GameState::getInstance().get("role");
        if (role == "fireboy") {
            auto fireboy = std::make_unique<Fireboy>(FIREBOY_ID, _network, _eventManager, gameEngine, true);
            _fireboy = fireboy.get();
            addObject(std::move(fireboy));
            auto water = std::make_unique<Watergirl>(WATERGIRL_ID, _network, _eventManager, gameEngine, false);
            water->getTransform()->getPosition()->setX(600);
            water->getTransform()->getPosition()->setY(500);
            addObject(std::move(water));
        } else {
            auto watergirl = std::make_unique<Watergirl>(WATERGIRL_ID, _network, _eventManager, gameEngine, true);
            _watergirl = watergirl.get();
            addObject(std::move(watergirl));
            auto fire = std::make_unique<Fireboy>(FIREBOY_ID, _network, _eventManager, gameEngine, false);
            fire->getTransform()->getPosition()->setX(200);
            fire->getTransform()->getPosition()->setY(500);
            addObject(std::move(fire));
        }
    } else {
        auto fireboy = std::make_unique<Fireboy>(nullptr, _eventManager, gameEngine, true);
        fireboy->getTransform()->getPosition()->setX(200);
        fireboy->getTransform()->getPosition()->setY(500);
        _fireboy = fireboy.get();
        addObject(std::move(fireboy));
        auto watergirl = std::make_unique<Watergirl>(nullptr, _eventManager, gameEngine, true);
        watergirl->getTransform()->getPosition()->setX(400);
        watergirl->getTransform()->getPosition()->setY(500);
        _watergirl = watergirl.get();
        addObject(std::move(watergirl));
    }
}

void LevelScene::createBat() {
    if (_isOnline) {
        std::string role = GameState::getInstance().get("role");
        if (role != "fireboy") return;
    }

    std::string sceneName = getName();
    LevelGrid* grid = GridManager::getGrid(sceneName);
    if (!grid) return;

    const int CELL_SIZE = grid->getCellSize();
    const int GRID_WIDTH = grid->getWidth();
    const int GRID_HEIGHT = grid->getHeight();

    int startGridX = GRID_WIDTH / 2;
    int startGridY = GRID_HEIGHT / 2;
    bool foundStart = false;

    for (int radius = 0; radius < std::min(GRID_WIDTH, GRID_HEIGHT) / 2 && !foundStart; ++radius) {
        for (int y = startGridY - radius; y <= startGridY + radius && !foundStart; ++y) {
            for (int x = startGridX - radius; x <= startGridX + radius && !foundStart; ++x) {
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

    auto bat = std::make_unique<Bat>(grid, CELL_SIZE, 80.0f);
    float worldX, worldY;
    grid->gridToWorld(startGridX, startGridY, worldX, worldY);
    worldX += CELL_SIZE / 2.0f;
    worldY += CELL_SIZE / 2.0f;

    bat->getTransform()->getPosition()->setX(static_cast<int>(worldX));
    bat->getTransform()->getPosition()->setY(static_cast<int>(worldY));
    bat->getTransform()->getSize()->setWidth(CELL_SIZE);
    bat->getTransform()->getSize()->setHeight(CELL_SIZE);

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