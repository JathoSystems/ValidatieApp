#include <iostream>
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"
#include "Scenes/Camera/FixedCamera.h"
#include "UI/Text.h"

int main() {
    try {
        std::unique_ptr<GameEngine> engine = std::make_unique<GameEngine>();
        engine->init("Vuurjongen en Watermeisje", 800, 600);

        std::unique_ptr<Scene> scene = std::make_unique<Scene>("main_scene");

        std::unique_ptr<Text> textComponent = std::make_unique<Text>("Vuurjongen en Watermeisje");
        textComponent->setColor(std::make_unique<Color>(255, 255 , 255));
        std::unique_ptr<GameObject> textObject = std::make_unique<GameObject>();
        textObject->addComponent(std::move(textComponent));
        textObject->getTransform()->getPosition()->setX(50);
        textObject->getTransform()->getPosition()->setY(50);
        textObject->getTransform()->getSize()->setWidth(500);
        textObject->getTransform()->getSize()->setHeight(60);

        std::unique_ptr<GameObject> fireboy = std::make_unique<GameObject>();
        std::unique_ptr<SpriteRenderer> sprite = std::make_unique<SpriteRenderer>("../resources/sprite.jpeg");
        fireboy->addComponent(std::move(sprite));
        fireboy->getTransform()->getPosition()->setX(100);
        fireboy->getTransform()->getPosition()->setY(200);

        std::unique_ptr<GameObject> watergirl = std::make_unique<GameObject>();
        std::unique_ptr<SpriteRenderer> sprite2 = std::make_unique<SpriteRenderer>("../resources/sprite2.png");
        watergirl->addComponent(std::move(sprite2));
        watergirl->getTransform()->getPosition()->setX(400);
        watergirl->getTransform()->getPosition()->setY(200);

        std::unique_ptr<Viewport> viewport = std::make_unique<Viewport>(Size(800, 600), Position(0, 0));
        std::unique_ptr<FixedCamera> camera = std::make_unique<FixedCamera>(std::move(viewport), Position(400, 300));
        scene->setCamera(std::move(camera));

        scene->addObject(std::move(textObject));
        scene->addObject(std::move(fireboy));
        scene->addObject(std::move(watergirl));
        engine->getSystem<SceneSystem>()->addScene(std::move(scene));
        engine->getSystem<SceneSystem>()->setScene("main_scene");

        // Main game loop
        engine->start();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
