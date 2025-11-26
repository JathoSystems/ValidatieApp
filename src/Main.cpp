#include <iostream>
#include "Engine/GameEngine.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Scenes/Scene.h"
#include "Scenes/SceneSystem.h"
#include "Scenes/Camera/FixedCamera.h"

int main() {
    try {
        std::unique_ptr<GameEngine> engine = std::make_unique<GameEngine>();
        engine->init("Vuurjongen en Watermeisje", 800, 600);

        std::unique_ptr<Scene> scene = std::make_unique<Scene>("main_scene");

        std::unique_ptr<GameObject> object = std::make_unique<GameObject>();
        std::unique_ptr<SpriteRenderer> sprite = std::make_unique<SpriteRenderer>("../resources/sprite.jpeg");
        object->addComponent(std::move(sprite));
        object->getTransform()->getPosition()->setX(100);
        object->getTransform()->getPosition()->setY(200);

        std::unique_ptr<Viewport> viewport = std::make_unique<Viewport>(Size(800, 600), Position(0, 0));
        std::unique_ptr<FixedCamera> camera = std::make_unique<FixedCamera>(std::move(viewport), Position(400, 300));
        scene->setCamera(std::move(camera));

        scene->addObject(std::move(object));
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
