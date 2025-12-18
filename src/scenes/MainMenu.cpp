#include "scenes/MainMenu.hpp"
#include "UI/Button.h"
#include "UI/Text.h"
#include "Scenes/SceneSystem.h"
#include "Scenes/Camera/FixedCamera.h"
#include "Engine/GameEngine.h"

MainMenu::MainMenu() : Scene("MainMenu") {
    auto titleText = std::make_unique<Text>("Vuurjongen en Watermeisje");
    titleText->setColor(std::make_unique<Color>(255, 255, 255));
    titleText->setFontSize(48);
    auto titleObj = std::make_unique<GameObject>();
    titleObj->addComponent(std::move(titleText));
    titleObj->getTransform()->getPosition()->setX(350);
    titleObj->getTransform()->getPosition()->setY(100);
    titleObj->getTransform()->getSize()->setWidth(600);
    titleObj->getTransform()->getSize()->setHeight(80);
    addObject(std::move(titleObj));

    auto playButton = std::make_unique<Button>("Play", std::make_unique<Color>(0, 128, 255));
    playButton->setOnClick([]() {
        GameEngine::getInstance().getSystem<SceneSystem>()->setScene("level_selector");
    });
    auto playButtonObj = std::make_unique<GameObject>();
    playButtonObj->addComponent(std::move(playButton));
    playButtonObj->getTransform()->getPosition()->setX(540);
    playButtonObj->getTransform()->getPosition()->setY(300);
    playButtonObj->getTransform()->getSize()->setWidth(200);
    playButtonObj->getTransform()->getSize()->setHeight(60);
    addObject(std::move(playButtonObj));

    auto quitButton = std::make_unique<Button>("Quit", std::make_unique<Color>(255, 80, 80));
    quitButton->setOnClick([]() {
        GameEngine::getInstance().stop();
    });
    auto quitButtonObj = std::make_unique<GameObject>();
    quitButtonObj->addComponent(std::move(quitButton));
    quitButtonObj->getTransform()->getPosition()->setX(540);
    quitButtonObj->getTransform()->getPosition()->setY(400);
    quitButtonObj->getTransform()->getSize()->setWidth(200);
    quitButtonObj->getTransform()->getSize()->setHeight(60);
    addObject(std::move(quitButtonObj));

    auto viewport = std::make_unique<Viewport>(Size(1280, 720), Position(0, 0));
    auto camera = std::make_unique<FixedCamera>(std::move(viewport), Position(640, 360));
    setCamera(std::move(camera));
}
