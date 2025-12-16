#include "LevelSelector.h"
#include "UI/Button.h"
#include "UI/Text.h"
#include "Scenes/Camera/FixedCamera.h"

LevelSelector::LevelSelector(SceneSystem *sceneSystem) : _sceneSystem(sceneSystem) {
}

void LevelSelector::createLevelSelectorScene() {
    std::unique_ptr<Scene> selectorScene = std::make_unique<Scene>("level_selector");

    std::unique_ptr<Text> titleText = std::make_unique<Text>("Vuurjongen en Watermeisje");
    titleText->setColor(std::make_unique<Color>(255, 255, 255));
    std::unique_ptr<GameObject> titleObject = std::make_unique<GameObject>();
    titleObject->addComponent(std::move(titleText));
    titleObject->getTransform()->getPosition()->setX(250);
    titleObject->getTransform()->getPosition()->setY(50);
    titleObject->getTransform()->getSize()->setWidth(300);
    titleObject->getTransform()->getSize()->setHeight(50);
    selectorScene->addObject(std::move(titleObject));

    float startX = 150;
    float startY = 150;
    float buttonWidth = 200;
    float buttonHeight = 80;
    float spacing = 100;

    for (int i = 1; i <= 4; i++) {
        float x = startX + ((i - 1) % 2) * (buttonWidth + spacing);
        float y = startY + ((i - 1) / 2) * (buttonHeight + spacing);

        std::unique_ptr<Button> levelButton = std::make_unique<Button>("Level " + std::to_string(i),
                                                                       std::make_unique<Color>(0, 128, 255));
        std::string levelName = "level_" + std::to_string(i);
        levelButton->setOnClick([this, levelName]() {
            _sceneSystem->setScene(levelName);
        });
        std::unique_ptr<GameObject> levelButtonObj = std::make_unique<GameObject>();
        levelButtonObj->addComponent(std::move(levelButton));
        levelButtonObj->setLayer(0);
        levelButtonObj->getTransform()->getPosition()->setX(x);
        levelButtonObj->getTransform()->getPosition()->setY(y);
        levelButtonObj->getTransform()->getSize()->setWidth(buttonWidth);
        levelButtonObj->getTransform()->getSize()->setHeight(buttonHeight);
        selectorScene->addObject(std::move(levelButtonObj));
    }

    std::unique_ptr<Viewport> viewport = std::make_unique<Viewport>(Size(800, 600), Position(0, 0));
    std::unique_ptr<FixedCamera> camera = std::make_unique<FixedCamera>(std::move(viewport), Position(400, 300));
    selectorScene->setCamera(std::move(camera));

    _sceneSystem->addScene(std::move(selectorScene));
}

void LevelSelector::createLevelScenes() {
    for (int i = 1; i <= 4; i++) {
        _sceneSystem->addScene(createLevel(i));
    }
}

std::unique_ptr<Scene> LevelSelector::createLevel(int levelNumber) {
    std::string levelName = "level_" + std::to_string(levelNumber);
    std::unique_ptr<Scene> levelScene = std::make_unique<Scene>(levelName);

    std::unique_ptr<Text> levelText = std::make_unique<Text>("Level " + std::to_string(levelNumber));
    levelText->setColor(std::make_unique<Color>(255, 255, 255));
    std::unique_ptr<GameObject> levelTextObj = std::make_unique<GameObject>();
    levelTextObj->addComponent(std::move(levelText));
    levelTextObj->getTransform()->getPosition()->setX(350);
    levelTextObj->getTransform()->getPosition()->setY(50);
    levelTextObj->getTransform()->getSize()->setWidth(200);
    levelTextObj->getTransform()->getSize()->setHeight(60);
    levelScene->addObject(std::move(levelTextObj));

    // Dit proces gaat anders worden... voor nu gewoon een button hier
    // Ik zat zelf te denken: level selector krijgt deze knop er onder, dus niet hier,
    // en dan doe je via die knop online fratsen, of met invite codes ofzo idk
    // zodat hier meteen level wordt geladen
    std::unique_ptr<Button> onlineButton = std::make_unique<
        Button>("Online Play", std::make_unique<Color>(128, 0, 128));
    std::unique_ptr<GameObject> onlineButtonObj = std::make_unique<GameObject>();
    onlineButtonObj->addComponent(std::move(onlineButton));
    onlineButtonObj->getTransform()->getPosition()->setX(325);
    onlineButtonObj->getTransform()->getPosition()->setY(250);
    onlineButtonObj->getTransform()->getSize()->setWidth(150);
    onlineButtonObj->getTransform()->getSize()->setHeight(50);
    levelScene->addObject(std::move(onlineButtonObj));

    std::unique_ptr<Button> backButton = std::make_unique<Button>("Back", std::make_unique<Color>(255, 100, 100));
    backButton->setOnClick([this]() {
        _sceneSystem->setScene("level_selector");
    });
    std::unique_ptr<GameObject> backButtonObj = std::make_unique<GameObject>();
    backButtonObj->addComponent(std::move(backButton));
    backButtonObj->getTransform()->getPosition()->setX(20);
    backButtonObj->getTransform()->getPosition()->setY(20);
    backButtonObj->getTransform()->getSize()->setWidth(80);
    backButtonObj->getTransform()->getSize()->setHeight(40);
    levelScene->addObject(std::move(backButtonObj));

    std::unique_ptr<Viewport> viewport = std::make_unique<Viewport>(Size(800, 600), Position(0, 0));
    std::unique_ptr<FixedCamera> camera = std::make_unique<FixedCamera>(std::move(viewport), Position(400, 300));
    levelScene->setCamera(std::move(camera));

    return levelScene;
}
