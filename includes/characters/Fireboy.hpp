#include "BaseCharacter.hpp"
#include "GameObjects/GameObject.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "characters/movement/MovementComponent.hpp"

class Fireboy : public BaseCharacter {
public:
    Fireboy(GameEngine *engine, bool activePlayer = false);
};
