#include "scenes/MainMenu.hpp"

#include "Animation/Animation.hpp"
#include "UI/Button.h"
#include "UI/Text.h"
#include "Scenes/SceneSystem.h"
#include "Scenes/Camera/FixedCamera.h"
#include "Engine/GameEngine.h"
#include "Audio/AudioSystem.h"
#include "GameObjects/Component/AudioComponent.h"
#include "GameObjects/Spritesheet/Animator.h"

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

    std::unique_ptr<Animation> animator = std::make_unique<Animation>(AnimationType::BOUNCE);
    std::unique_ptr<Transform> a = std::make_unique<Transform>();
    a->getPosition()->setX(350);
    a->getPosition()->setY(100);
    a->getSize()->setWidth(600);
    a->getSize()->setHeight(80);
    a->getScale()->setScale(1);

    std::unique_ptr<Transform> b = std::make_unique<Transform>();
    b->getPosition()->setX(350);
    b->getPosition()->setY(100);
    b->getSize()->setWidth(650);
    b->getSize()->setHeight(130);

    std::unique_ptr<Transform> c = std::make_unique<Transform>();
    c->getPosition()->setX(350);
    c->getPosition()->setY(100);
    c->getSize()->setWidth(600);
    c->getSize()->setHeight(80);

    std::unique_ptr<Keyframe> f = std::make_unique<Keyframe>(std::move(a));
    std::unique_ptr<Keyframe> s = std::make_unique<Keyframe>(std::move(b));
    std::unique_ptr<Keyframe> t = std::make_unique<Keyframe>(std::move(c));

    int speed {3};

    animator->addKeyframe(0, std::move(f));
    animator->addKeyframe(speed, std::move(s));
    animator->addKeyframe(speed*2, std::move(t));
    titleObj->addComponent(std::move(animator));

    addObject(std::move(titleObj));

    auto playButton = std::make_unique<Button>("Play", std::make_unique<Color>(0, 128, 255));
    playButton->setOnClick([]() {
        GameEngine::getInstance().getSystem<AudioSystem>()->stopMusic();
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

void MainMenu::onInitialRender() {
    GameEngine::getInstance().getSystem<AudioSystem>()->initialize();
    auto* audioSystem = GameEngine::getInstance().getSystem<AudioSystem>();
    if (audioSystem) {
        audioSystem->loadSound("mainmenu", "resources/mainmenu.mp3");
        audioSystem->playMusic("mainmenu", 0.05f, true);
    }
}
