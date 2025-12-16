//
// Created by jusra on 15-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_GAME_BASECHARACTERCONTROLLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_BASECHARACTERCONTROLLER_HPP
#include "enums/Direction.hpp"
#include "Events/EventManager.h"
#include "Input/IKeyListener.h"
#include "Network/NetworkSystem.h"
#include "Physics/PhysicsComponent.h"

class BaseCharacterController : public IKeyListener {
private:
    bool _grounded = true;
    Direction _movementDirection = Direction::NONE;
    float _movementSpeed = 300.0f;
    float _jumpingSpeed = 5000.0f;
    EventManager *_eventManager;
    int _parentId;
    std::shared_ptr<NetworkSystem> _network;

public:
    BaseCharacterController(std::shared_ptr<NetworkSystem> network, int parentId, EventManager *eventManager);

    void onKeyPress(Key key) override;

    void onKeyRelease(Key key) override;

    bool isGrounded() const;

    void setGrounded(bool grounded);

    Direction getMovementDirection() const { return _movementDirection; }
    void setMovementDirection(Direction direction) { _movementDirection = direction; }


    void move(Direction direction, PhysicsComponent *physics);
};

#endif //VUURJONGEN_WATERMEISJE_GAME_BASECHARACTERCONTROLLER_HPP
