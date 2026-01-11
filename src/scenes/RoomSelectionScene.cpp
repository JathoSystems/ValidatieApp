#include "scenes/RoomSelectionScene.hpp"
#include "UI/Button.h"
#include "UI/Text.h"
#include "Scenes/Camera/FixedCamera.h"
#include "Engine/GameEngine.h"
#include "Scenes/SceneSystem.h"
#include "Network/NetworkSystem.h"
#include "server/packet/CreateLobbyPacket.hpp"
#include "server/packet/JoinLobbyPacket.hpp"
#include <iostream>

#include "Network/GameState.hpp"

RoomSelectionScene::RoomSelectionScene(std::shared_ptr<NetworkSystem> network, int levelNumber)
    : Scene("room_selection_level_" + std::to_string(levelNumber)),
      _network(network),
      _selectedLevel(levelNumber),
      _currentLobbyIdInput(0) {
    
    // Title
    auto titleText = std::make_unique<Text>("Room Selection - Level " + std::to_string(levelNumber));
    titleText->setColor(std::make_unique<Color>(255, 255, 255));
    titleText->setFontSize(40);
    auto titleObj = std::make_unique<GameObject>();
    titleObj->addComponent(std::move(titleText));
    titleObj->getTransform()->getPosition()->setX(380);
    titleObj->getTransform()->getPosition()->setY(50);
    titleObj->getTransform()->getSize()->setWidth(600);
    titleObj->getTransform()->getSize()->setHeight(60);
    addObject(std::move(titleObj));

    // Create New Lobby Button
    auto createButton = std::make_unique<Button>("Create New Lobby", std::make_unique<Color>(0, 200, 0));
    createButton->setOnClick([this]() {
        CreateLobbyPacket packet(_selectedLevel);
        packet.serialize();
        // Send packet through NetworkSystem
        _network->send(packet);
        std::cout << "[RoomSelection] Creating lobby for level " << _selectedLevel << std::endl;
    });
    auto createButtonObj = std::make_unique<GameObject>();
    createButtonObj->addComponent(std::move(createButton));
    createButtonObj->getTransform()->getPosition()->setX(440);
    createButtonObj->getTransform()->getPosition()->setY(200);
    createButtonObj->getTransform()->getSize()->setWidth(400);
    createButtonObj->getTransform()->getSize()->setHeight(60);
    addObject(std::move(createButtonObj));

    // Lobby ID Input Label
    auto labelText = std::make_unique<Text>("Enter Lobby ID (0-9):");
    labelText->setColor(std::make_unique<Color>(255, 255, 255));
    labelText->setFontSize(24);
    auto labelObj = std::make_unique<GameObject>();
    labelObj->addComponent(std::move(labelText));
    labelObj->getTransform()->getPosition()->setX(440);
    labelObj->getTransform()->getPosition()->setY(320);
    labelObj->getTransform()->getSize()->setWidth(400);
    labelObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(labelObj));

    // Display current input
    auto displayText = std::make_unique<Text>("0");
    displayText->setColor(std::make_unique<Color>(255, 255, 255));
    displayText->setFontSize(48);
    auto displayObj = std::make_unique<GameObject>();
    Text* displayTextPtr = displayText.get();
    displayObj->addComponent(std::move(displayText));
    displayObj->getTransform()->getPosition()->setX(640);
    displayObj->getTransform()->getPosition()->setY(370);
    displayObj->getTransform()->getSize()->setWidth(80);
    displayObj->getTransform()->getSize()->setHeight(60);
    addObject(std::move(displayObj));
    
    // Store reference for later updates
    _displayTextPtr = displayTextPtr;

    // Number buttons (0-9)
    float startX = 540;
    float startY = 450;
    float btnWidth = 60;
    float btnHeight = 50;
    float spacing = 10;
    
    for (int i = 0; i <= 9; i++) {
        int num = i;
        auto numButton = std::make_unique<Button>(std::to_string(i), std::make_unique<Color>(100, 150, 255));
        numButton->setOnClick([this, num]() {
            _currentLobbyIdInput = num;
            updateLobbyIdDisplay();
        });
        auto numButtonObj = std::make_unique<GameObject>();
        numButtonObj->addComponent(std::move(numButton));
        
        int col = i % 5;
        int row = i / 5;
        numButtonObj->getTransform()->getPosition()->setX(startX + col * (btnWidth + spacing));
        numButtonObj->getTransform()->getPosition()->setY(startY + row * (btnHeight + spacing));
        numButtonObj->getTransform()->getSize()->setWidth(btnWidth);
        numButtonObj->getTransform()->getSize()->setHeight(btnHeight);
        addObject(std::move(numButtonObj));
    }

    // Join Lobby Button
    auto joinButton = std::make_unique<Button>("Join Lobby", std::make_unique<Color>(200, 100, 0));
    joinButton->setOnClick([this]() {
        if (_currentLobbyIdInput > 0) {
            JoinLobbyPacket packet(_currentLobbyIdInput, _selectedLevel);
            packet.serialize();
            // Send packet through NetworkSystem
            _network->send(packet);
            std::cout << "[RoomSelection] Joining lobby " << _currentLobbyIdInput << std::endl;
        }
    });
    auto joinButtonObj = std::make_unique<GameObject>();
    joinButtonObj->addComponent(std::move(joinButton));
    joinButtonObj->getTransform()->getPosition()->setX(440);
    joinButtonObj->getTransform()->getPosition()->setY(580);
    joinButtonObj->getTransform()->getSize()->setWidth(400);
    joinButtonObj->getTransform()->getSize()->setHeight(60);
    addObject(std::move(joinButtonObj));

    // Back Button
    std::string currentSceneName = getName();
    auto backButton = std::make_unique<Button>("Back", std::make_unique<Color>(255, 100, 100));
    backButton->setOnClick([currentSceneName]() {
        SceneSystem* sceneSystem = GameEngine::getInstance().getSystem<SceneSystem>();
        sceneSystem->setScene("level_selector");
        sceneSystem->removeScene(currentSceneName);
    });
    auto backButtonObj = std::make_unique<GameObject>();
    backButtonObj->addComponent(std::move(backButton));
    backButtonObj->getTransform()->getPosition()->setX(20);
    backButtonObj->getTransform()->getPosition()->setY(20);
    backButtonObj->getTransform()->getSize()->setWidth(80);
    backButtonObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(backButtonObj));

    auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
    setCamera(std::move(camera));
}

void RoomSelectionScene::updateLobbyIdDisplay() {
    // Update the display text using stored pointer
    if (_displayTextPtr) {
        _displayTextPtr->setText(std::to_string(_currentLobbyIdInput));
        // Note: Don't set GameState "lobby" here - it will be set when server
        // confirms lobby join via LobbyInfoPacketHandler
    }
}
