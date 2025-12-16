#ifndef VUURJONGEN_WATERMEISJE_BASECHARACTER_HPP
#define VUURJONGEN_WATERMEISJE_BASECHARACTER_HPP
#include "BaseCharacterController.hpp"
#include "Engine/GameEngine.h"
#include "events/MoveEvent.hpp"
#include "GameObjects/GameObject.h"

enum class Animation {
    IDLE,
    LEFT,
    RIGHT,
    JUMP,
    FALLING
};

class BaseCharacter : public GameObject {
private:
    std::string idle;
    std::string left;
    std::string right;
    std::string jump;
    std::string falling;
    std::unique_ptr<BaseCharacterController> _controller;
    
    // Animatie state tracking
    Animation _currentAnimation = Animation::IDLE;
    
    // Interne methode die daadwerkelijk de animator switch doet
    void updateAnimator(Animation newAnimation);

public:
    BaseCharacter(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                  bool activePlayer);

    BaseCharacter(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine,
                  bool activePlayer);

    void build();

    std::string getJumpingSpritesheet() const;
    std::string getLeftSpritesheet() const;
    std::string getRightSpritesheet() const;
    std::string getIdleSpritesheet() const;
    std::string getFallingSpritesheet() const;
    
    Direction _direction = Direction::NONE;

    virtual ~BaseCharacter() = default;

    void setMovementDirection(Direction direction);
    
    // Centrale methode die de juiste animatie bepaalt op basis van controller state
    void updateAnimation();
    
    // Getter voor controller
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

#endif //VUURJONGEN_WATERMEISJE_BASECHARACTER_HPP