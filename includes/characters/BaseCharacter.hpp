#ifndef VUURJONGEN_WATERMEISJE_BASECHARACTER_HPP
#define VUURJONGEN_WATERMEISJE_BASECHARACTER_HPP
#include "characters/BaseCharacterController.hpp"
#include "Engine/GameEngine.h"
#include "characters/events/MoveEvent.hpp"
#include "GameObjects/Broadcastable.h"
#include "GameObjects/GameObject.h"
#include <string>
#include <memory>

enum class Animation {
    IDLE,
    LEFT,
    RIGHT,
    JUMP,
    FALLING
};

struct PendingNetworkUpdate {
    bool hasPending = false;
    float x = 0.0f;
    float y = 0.0f;
    // NEW: Velocity
    float vx = 0.0f;
    float vy = 0.0f;

    Direction direction = Direction::NONE;
    bool toggle = false;
};

struct PendingJump {
    bool shouldJump = false;
};

class BaseCharacter : public GameObject , public Broadcastable {
private:
    std::string idle;
    std::string left;
    std::string right;
    std::string jump;
    std::string falling;
    float _physicsAccumulator = 0.0f;
    std::unique_ptr<BaseCharacterController> _controller;

    Animation _currentAnimation = Animation::IDLE;
    PendingNetworkUpdate _pendingUpdate;
    PendingJump _pendingJump;

    void updateAnimator(Animation newAnimation);
    void initializeCharacter(int id, std::shared_ptr<NetworkSystem> network, EventManager *eventManager,
                             GameEngine *engine, bool activePlayer, KeyBindings bindings);
    void applyPendingNetworkUpdate();
    void applyPendingJump();

public:
    // Standard Constructor (Generates Random ID)
    BaseCharacter(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                  bool activePlayer, KeyBindings bindings);

    // Fixed ID Constructor (Uses specific ID like 99 or 100)
    BaseCharacter(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                  bool activePlayer, KeyBindings bindings);

    virtual ~BaseCharacter() = default;

    std::string getJumpingSpritesheet() const;
    std::string getLeftSpritesheet() const;
    std::string getRightSpritesheet() const;
    std::string getIdleSpritesheet() const;
    std::string getFallingSpritesheet() const;

    void setMovementDirection(Direction direction);
    void setPendingNetworkUpdate(float x, float y, float vx, float vy, Direction direction, bool toggle);
    void setPendingJump(bool shouldJump);

    void updateAnimation();

    BaseCharacterController* getController() { return _controller.get(); }

protected:
    void update(float delta) override;
    void onCollisionEnter(const CollisionData &collision) override;
    void onCollisionExit(const CollisionData &collision) override;

    void setIdleSpritesheet(std::string idle);
    void setMovingLeftSpritesheet(std::string left);
    void setMovingRightSpritesheet(std::string right);
    void setJumpingSpritesheet(std::string jump);
    void setFallingSpritesheet(std::string falling);
};

#endif