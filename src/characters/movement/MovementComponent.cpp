//
// Created by jusra on 8-12-2025.
//

#include "characters/movement/MovementComponent.hpp"

void sync(std::initializer_list<std::string> events, std::shared_ptr<GameObject> object) {
    for (const auto &eventName: events) {
        EventRegistry::getInstance()->createEvent(eventName);
        auto event = EventRegistry::getInstance()->getEvent(eventName);

        if (!event)
            return;

        // ToDo: centralized manager?
        EventManager manager;
        manager.broadcast(event);
    }
}

MovementComponent::MovementComponent(GameObject *gameObject, GameEngine *engine, Key up, Key down, Key left, Key right)
    : KeyInputComponent(gameObject),
      input(std::make_unique<MovementInputHandler>(up, down, left, right)),
      object(gameObject),
      engineRef(engine) {
    this->setListener(input.get());

    if (engineRef) {
        engineRef->getSystem<InputSystem>()->registerKeyComponent(this);
    }
}

MovementComponent::~MovementComponent() {
    if (engineRef) {
        engineRef->getSystem<InputSystem>()->unregisterKeyComponent(this);
    }
}

void MovementComponent::update(float deltaTime) {
    KeyInputComponent::update(deltaTime);

    auto pos = object->getTransform()->getPosition();

    float dx = 0.0f;
    float dy = 0.0f;

    // Bepaal huidige bewegingsrichting
    std::string currentState = "idle";

    if (input->moveUp) {
        dy -= 1.0f;
        currentState = "up";
    }
    if (input->moveDown) {
        dy += 1.0f;
        currentState = "down";
    }
    if (input->moveLeft) {
        dx -= 1.0f;
        currentState = "left";
    }
    if (input->moveRight) {
        dx += 1.0f;
        currentState = "right";
    }

    // Als er meerdere knoppen ingedrukt zijn, gebruik een gecombineerde state
    if ((input->moveUp || input->moveDown) && (input->moveLeft || input->moveRight)) {
        currentState = "moving"; // of een andere logica voor diagonaal
    }

    // Roep callback aan ALLEEN als de state is veranderd
    if (currentState != lastState) {
        if (currentState == "up" && onUpCallback) {
            onUpCallback();
        } else if (currentState == "down" && onDownCallback) {
            onDownCallback();
        } else if (currentState == "left" && onLeftCallback) {
            onLeftCallback();
        } else if (currentState == "right" && onRightCallback) {
            onRightCallback();
        } else if (currentState == "idle" && onIdleCallback) {
            onIdleCallback();
        }

        lastState = currentState;
    }

    // Voer beweging uit
    if (dx != 0.0f || dy != 0.0f) {
        float len = std::hypot(dx, dy);
        dx = (dx / len) * speed * deltaTime;
        dy = (dy / len) * speed * deltaTime;

        int x = pos->getX() + static_cast<int>(std::round(dx));
        int y = pos->getY() + static_cast<int>(std::round(dy));

        MoveEvent event(x, y);
        std::shared_ptr<GameObject> obj(_parent, [](GameObject *) {
        });

        if (!obj->getTransform()) {
            std::cerr << "Transform is null!" << std::endl;
            return;
        }

        std::string eventName = "fireboy-move";
        auto registery = EventRegistry::getInstance();
        registery->registerEvent(eventName, [this, x, y]() {
            return std::make_shared<MoveEvent>(x, y);
        });

        sync({eventName}, obj);
    }
}