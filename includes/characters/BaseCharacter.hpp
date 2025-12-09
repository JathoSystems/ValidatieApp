//
// Created by jusra on 9-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_BASECHARACTER_HPP
#define VUURJONGEN_WATERMEISJE_BASECHARACTER_HPP
#include "Engine/GameEngine.h"
#include "GameObjects/GameObject.h"

class BaseCharacter : public GameObject {
private:
    std::string idle;
    std::string left;
    std::string right;
    std::string jump;
    std::string falling;
public:
    BaseCharacter(GameEngine* engine, bool activePlayer);
    virtual ~BaseCharacter() = default;

protected:
    void setIdleSpritesheet(std::string idle);
    void setMovingLeftSpritesheet(std::string left);
    void setMovingRightSpritesheet(std::string right);
    void setJumpingSpritesheet(std::string jump);
    void setFallingSpritesheet(std::string falling);
};

#endif //VUURJONGEN_WATERMEISJE_BASECHARACTER_HPP
