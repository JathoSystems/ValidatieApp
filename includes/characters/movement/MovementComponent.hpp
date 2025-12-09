#ifndef VUURJONGEN_WATERMEISJE_MOVEMENTCOMPONENT_HPP
#define VUURJONGEN_WATERMEISJE_MOVEMENTCOMPONENT_HPP

#include <memory>
#include <cmath>
#include <iostream>
#include <string>

#include "characters/events/MoveEvent.hpp"
#include "characters/input/MovementInputHandler.hpp"
#include "Engine/GameEngine.h"
#include "Events/EventManager.h"
#include "Events/EventRegistry.h"
#include "GameObjects/Component/KeyInputComponent.h"
#include "Input/InputSystem.h"

class MovementComponent : public KeyInputComponent {
private:
    std::unique_ptr<MovementInputHandler> input;
    GameObject *object;
    GameEngine *engineRef = nullptr;
    float speed = 300.0f;
    std::function<void()> onUpCallback;
    std::function<void()> onDownCallback;
    std::function<void()> onLeftCallback;
    std::function<void()> onRightCallback;
    std::function<void()> onIdleCallback;

    std::string lastState = "idle";

public:
    explicit MovementComponent(GameObject *gameObject, GameEngine *engine, Key up, Key down, Key left, Key right);

    ~MovementComponent() override;

    void update(float deltaTime) override;

    void onUp(std::function<void()> callback) {
        this->onUpCallback = callback;
    }

    void onDown(std::function<void()> callback) {
        this->onDownCallback = callback;
    }

    void onLeft(std::function<void()> callback) {
        this->onLeftCallback = callback;
    }

    void onRight(std::function<void()> callback) {
        this->onRightCallback = callback;
    }

    void onIdle(std::function<void()> callback) {
        this->onIdleCallback = callback;
    }
};

#endif // VUURJONGEN_WATERMEISJE_MOVEMENTCOMPONENT_HPP