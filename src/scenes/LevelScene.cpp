#include "scenes/LevelScene.hpp"
#include "LevelGrid.h"
#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "GridRenderer.h"
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

LevelScene::LevelScene(int levelNumber, bool isOnline, std::shared_ptr<NetworkSystem> network,
                       EventManager *eventManager)
    : Scene("level_" + std::to_string(levelNumber) + (isOnline ? "_online" : "")),
      _levelNumber(levelNumber),
      _isOnline(isOnline),
      _network(network),
      _eventManager(eventManager),
      _fireboyDiamondText(nullptr),
      _watergirlDiamondText(nullptr),
      _fireboy(nullptr),
      _watergirl(nullptr),
      _isInitialized(false) {
}

void LevelScene::onInitialRender() {
    std::cout << "[LevelScene] Initialize started for level " << _levelNumber << std::endl;

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

    // Clear the level grid
    _levelGrid.reset();

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
        if (Fireboy *fireboy = dynamic_cast<Fireboy *>(obj.get())) {
            return fireboy;
        }
    }
    return nullptr;
}

Watergirl *getWatergirl(Scene *scene) {
    auto &objects = scene->getObjects();
    for (auto &obj: objects) {
        if (Watergirl *watergirl = dynamic_cast<Watergirl *>(obj.get())) {
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
        // Check if object is a diamond
        if (RedDiamond *redDiamond = dynamic_cast<RedDiamond *>(obj.get())) {
            redDiamond->checkCollisionWith(getFireboy(this));
        } else if (BlueDiamond *blueDiamond = dynamic_cast<BlueDiamond *>(obj.get())) {
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
    _levelGrid = std::make_unique<LevelGrid>(32, 18, 40);

    for (int x = 0; x < 32; x++) {
        _levelGrid->setCellType(x, 17, CellType::Ground);
    }
    _levelGrid->setCellType(15, 16, CellType::RedDiamond);
    _levelGrid->setCellType(18, 16, CellType::BlueDiamond);
    _levelGrid->setCellType(3, 16, CellType::RedDoor);
    _levelGrid->setCellType(22, 16, CellType::BlueDoor);

    for (int y = 0; y < 18; y++) {
        _levelGrid->setCellType(0, y, CellType::Ground);
        _levelGrid->setCellType(31, y, CellType::Ground);
    }

    for (int x = 5; x < 12; x++) {
        _levelGrid->setCellType(x, 12, CellType::Ground);
    }

    for (int x = 20; x < 27; x++) {
        _levelGrid->setCellType(x, 12, CellType::Lava);
    }

    for (int x = 12; x < 20; x++) {
        _levelGrid->setCellType(x, 8, CellType::Ground);
    }
}

void LevelScene::checkDoorCollisions() {
    Fireboy* fire = _fireboy ? _fireboy : getFireboy(this);
    Watergirl* water = _watergirl ? _watergirl : getWatergirl(this);
    if (!fire && !water)
        return;

    auto &objects = getObjects();

    for (auto &obj : objects) {
        if (Door* door = dynamic_cast<Door*>(obj.get())) {
            if (fire) door->checkCollisionWith(fire);
            if (water) door->checkCollisionWith(water);
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

    int cellSize = _levelGrid->getCellSize();

    // Create individual blocks for each ground cell
    int blockCount = 0;
    for (int x = 0; x < _levelGrid->getWidth(); ++x) {
        for (int y = 0; y < _levelGrid->getHeight(); ++y) {
            CellType type = _levelGrid->getCellType(x, y);
            if (type == CellType::Ground) {
                blockCount++;

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
                blockCount++;
                addObject(std::make_unique<Water>(_levelGrid.get(), x, y));
            }

            if (type == CellType::Lava)
                addObject(std::make_unique<Lava>(_levelGrid.get(), x, y));

            if (type == CellType::RedDiamond)
                addObject(std::make_unique<RedDiamond>(_levelGrid.get(), x, y));

            if (type == CellType::BlueDiamond)
                addObject(std::make_unique<BlueDiamond>(_levelGrid.get(), x, y));

            if (type == CellType::RedDoor)
                addObject(std::make_unique<Door>(this, _levelGrid->getCellSize(), x, y));

            if (type == CellType::BlueDoor)
                addObject(std::make_unique<Door>(this, _levelGrid->getCellSize(), x, y, "blue"));
        }
    }
}

void LevelScene::setupCharacters() {
    GameEngine *gameEngine = &GameEngine::getInstance();

    if (_isOnline) {
        std::string role = GameState::getInstance().get("role");
        if (role == "fireboy") {
            auto fireboy = std::make_unique<Fireboy>(_network, _eventManager, gameEngine, true);
            _fireboy = fireboy.get(); // Bewaar pointer
            addObject(std::move(fireboy));
        } else {
            auto watergirl = std::make_unique<Watergirl>(_network, _eventManager, gameEngine, true);
            _watergirl = watergirl.get(); // Bewaar pointer
            addObject(std::move(watergirl));
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
