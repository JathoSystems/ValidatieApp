//
// Created by jusra on 16-12-2025.
//

#include "scenes/Lobby.hpp"
#include "UI/Text.h"
#include "UI/Button.h"
#include "Scenes/Camera/FixedCamera.h"
#include "Scenes/SceneSystem.h"
#include "Engine/GameEngine.h"
#include "Network/GameState.hpp"
#include "server/packet/QuitPacket.hpp"

Lobby::Lobby(std::shared_ptr<NetworkSystem> network)
    : Scene("Lobby"),
      _lobbyId(0),
      _levelId(0),
      _playerCount(0),
      _lobbyIdTextObj(nullptr),
      _statusTextObj(nullptr),
      _levelTextObj(nullptr),
      _network(network) {

    // Title
    auto titleText = std::make_unique<Text>("Waiting in Lobby");
    titleText->setColor(std::make_unique<Color>(255, 255, 255));
    titleText->setFontSize(48);
    auto titleObj = std::make_unique<GameObject>();
    titleObj->addComponent(std::move(titleText));
    titleObj->getTransform()->getPosition()->setX(440);
    titleObj->getTransform()->getPosition()->setY(150);
    titleObj->getTransform()->getSize()->setWidth(600);
    titleObj->getTransform()->getSize()->setHeight(60);
    addObject(std::move(titleObj));

    // Lobby ID display
    auto lobbyIdText = std::make_unique<Text>("Lobby ID: --");
    lobbyIdText->setColor(std::make_unique<Color>(255, 255, 0));
    lobbyIdText->setFontSize(36);
    auto lobbyIdObj = std::make_unique<GameObject>();
    _lobbyIdTextObj = lobbyIdObj.get();
    lobbyIdObj->addComponent(std::move(lobbyIdText));
    lobbyIdObj->getTransform()->getPosition()->setX(440);
    lobbyIdObj->getTransform()->getPosition()->setY(250);
    lobbyIdObj->getTransform()->getSize()->setWidth(400);
    lobbyIdObj->getTransform()->getSize()->setHeight(50);
    addObject(std::move(lobbyIdObj));

    // Status display
    auto statusText = std::make_unique<Text>("Waiting for player...");
    statusText->setColor(std::make_unique<Color>(200, 200, 200));
    statusText->setFontSize(28);
    auto statusObj = std::make_unique<GameObject>();
    _statusTextObj = statusObj.get();
    statusObj->addComponent(std::move(statusText));
    statusObj->getTransform()->getPosition()->setX(440);
    statusObj->getTransform()->getPosition()->setY(350);
    statusObj->getTransform()->getSize()->setWidth(400);
    statusObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(statusObj));

    // Level info
    auto levelText = std::make_unique<Text>("Level: --");
    levelText->setColor(std::make_unique<Color>(255, 255, 255));
    levelText->setFontSize(24);
    auto levelObj = std::make_unique<GameObject>();
    _levelTextObj = levelObj.get();
    levelObj->addComponent(std::move(levelText));
    levelObj->getTransform()->getPosition()->setX(440);
    levelObj->getTransform()->getPosition()->setY(420);
    levelObj->getTransform()->getSize()->setWidth(400);
    levelObj->getTransform()->getSize()->setHeight(40);
    addObject(std::move(levelObj));

    // Leave Lobby Button - WITH NETWORK CLEANUP
    auto backButton = std::make_unique<Button>("Leave Lobby", std::make_unique<Color>(255, 100, 100));
    backButton->setOnClick([this]() {
        std::cout << "[Lobby] Leave Lobby clicked" << std::endl;

        // Get lobby ID from GameState
        std::string lobbyIdStr = GameState::getInstance().get("lobby");

        // Send quit packet if we have a network connection and lobby ID
        if (!lobbyIdStr.empty() && _network) {
            std::cout << "[Lobby] Sending quit packet for lobby " << lobbyIdStr << std::endl;
            QuitPacket quit;
            _network->send(quit);
        }

        // Clear lobby state from GameState
        GameState::getInstance().remove("lobby");
        GameState::getInstance().remove("role");
        std::cout << "[Lobby] Cleared lobby state from GameState" << std::endl;

        // Go back to level selector
        GameEngine::getInstance().getSystem<SceneSystem>()->setScene("level_selector");
    });
    auto backButtonObj = std::make_unique<GameObject>();
    backButtonObj->addComponent(std::move(backButton));
    backButtonObj->getTransform()->getPosition()->setX(440);
    backButtonObj->getTransform()->getPosition()->setY(550);
    backButtonObj->getTransform()->getSize()->setWidth(400);
    backButtonObj->getTransform()->getSize()->setHeight(60);
    addObject(std::move(backButtonObj));

    auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
    setCamera(std::move(camera));
}

void Lobby::setLobbyInfo(int lobbyId, int levelId, int playerCount) {
    _lobbyId = lobbyId;
    _levelId = levelId;
    _playerCount = playerCount;

    if (_lobbyIdTextObj) {
        if (auto* text = _lobbyIdTextObj->getComponent<Text>()) {
            text->setText("Lobby ID: " + std::to_string(_lobbyId));
        }
    }

    if (_levelTextObj) {
        if (auto* text = _levelTextObj->getComponent<Text>()) {
            text->setText("Level: " + std::to_string(_levelId));
        }
    }

    // Update status
    std::string status = _playerCount == 1 ? "Waiting for player..." : "Ready to start!";
    updateStatus(status);
}

void Lobby::updateStatus(const std::string& status) {
    if (_statusTextObj) {
        if (auto* text = _statusTextObj->getComponent<Text>()) {
            text->setText(status);
        }
    }
}