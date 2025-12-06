#ifndef VUURJONGEN_WATERMEISJE_MOVEMENTCOMPONENT_HPP
#define VUURJONGEN_WATERMEISJE_MOVEMENTCOMPONENT_HPP

#include <memory>
#include <cmath>
#include <iostream>

#include "characters/events/MoveEvent.hpp"
#include "Engine/GameEngine.h"
#include "Events/EventManager.h"
#include "Events/EventRegistry.h"
#include "GameObjects/Component/KeyInputComponent.h"
#include "Input/InputSystem.h"

class MovementInput : public IKeyListener {
public:
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;

private:
    Key keyUp;
    Key keyDown;
    Key keyLeft;
    Key keyRight;

public:
    MovementInput(Key up, Key down, Key left, Key right)
        : keyUp(up), keyDown(down), keyLeft(left), keyRight(right) {
    }

    void onKeyPress(Key key) override {
        if (key == keyUp) moveUp = true;
        if (key == keyDown) moveDown = true;
        if (key == keyLeft) moveLeft = true;
        if (key == keyRight) moveRight = true;
    }

    void onKeyRelease(Key key) override {
        if (key == keyUp) moveUp = false;
        if (key == keyDown) moveDown = false;
        if (key == keyLeft) moveLeft = false;
        if (key == keyRight) moveRight = false;
    }
};

class MovementComponent : public KeyInputComponent {
private:
    std::unique_ptr<MovementInput> input;
    GameObject *object;
    GameEngine *engineRef = nullptr;
    float speed = 300.0f; // pixels per second

public:
    explicit MovementComponent(GameObject *gameObject, GameEngine *engine, Key up, Key down, Key left, Key right)
        : KeyInputComponent(gameObject),
          input(std::make_unique<MovementInput>(up, down, left, right)),
          object(gameObject),
          engineRef(engine) {
        this->setListener(input.get());

        if (engineRef) {
            engineRef->getSystem<InputSystem>()->registerKeyComponent(this);
        }
    }

    ~MovementComponent() override {
        if (engineRef) {
            engineRef->getSystem<InputSystem>()->unregisterKeyComponent(this);
        }
    }

    void update(float deltaTime) override {
        KeyInputComponent::update(deltaTime);

        // Nu onze movement logic
        auto pos = object->getTransform()->getPosition();

        float dx = 0.0f;
        float dy = 0.0f;

        if (input->moveUp) dy -= 1.0f;
        if (input->moveDown) dy += 1.0f;
        if (input->moveLeft) dx -= 1.0f;
        if (input->moveRight) dx += 1.0f;

        if (dx != 0.0f || dy != 0.0f) {
            float len = std::hypot(dx, dy);
            dx = (dx / len) * speed * deltaTime;
            dy = (dy / len) * speed * deltaTime;

            int x = pos->getX() + static_cast<int>(std::round(dx));
            int y = pos->getY() + static_cast<int>(std::round(dy));

            MoveEvent event (x, y);
            event.apply(_parent);

            // std::string eventName = "fireboy-move";
            // auto registery = EventRegistry::getInstance();
            // registery->registerEvent(eventName, [this, x, y]() {
            //     return std::make_shared<MoveEvent>(x, y);
            // });
            // EventRegistry::getInstance()->createEvent(eventName);
            // auto event = EventRegistry::getInstance()->getEvent(eventName);
            //
            // if (!event)
            //     return;
            //
            // event->apply(_parent);
        }
    }
};

#endif // VUURJONGEN_WATERMEISJE_MOVEMENTCOMPONENT_HPP
