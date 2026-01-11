#ifndef VUURJONGEN_WATERMEISJE_GAME_BASECHARACTERCONTROLLER_HPP
#define VUURJONGEN_WATERMEISJE_GAME_BASECHARACTERCONTROLLER_HPP
#include "enums/Direction.hpp"
#include "Events/EventManager.h"
#include "Input/IKeyListener.h"
#include "Network/NetworkSystem.h"
#include "Physics/PhysicsComponent.h"

struct KeyBindings {
    Key left;
    Key right;
    Key jump;
};

class BaseCharacterController : public IKeyListener {
private:
    bool _grounded = false;
    bool _shouldJump = false;
    Direction _movementDirection = Direction::NONE;
    float _movementSpeed = 300.0f;
    float _jumpForce = 5000000.0f;
    EventManager *_eventManager;
    int _parentId;
    std::shared_ptr<NetworkSystem> _network;
    KeyBindings _keyBindings;
    bool _active;

    bool _isLeftPressed = false;
    bool _isRightPressed = false;

    float _syncTimer = 0.0f;

public:
    BaseCharacterController(std::shared_ptr<NetworkSystem> network, int parentId, EventManager *eventManager,
                            KeyBindings bindings, bool active);

    void getCurrentPhysicsState(float &x, float &y, float &vx, float &vy);

    void onKeyPress(Key key) override;

    void onKeyRelease(Key key) override;

    bool isGrounded() const;

    void setGrounded(bool grounded);

    void setParentId(int parentId) { _parentId = parentId; }
    int getParentId() const { return _parentId; }

    Direction getMovementDirection() const { return _movementDirection; }
    void setMovementDirection(Direction direction) { _movementDirection = direction; }

    void move(Direction direction, PhysicsComponent *physics);

    void getCurrentPosition(float &x, float &y);

    void updateMovementDirection();

    bool isActive() const { return _active; }

    void update(float delta);
};

#endif
