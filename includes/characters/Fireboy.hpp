#include "BaseCharacter.hpp"
#include "GameObjects/GameObject.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "GameObjects/Spritesheet/Animator.h"

class Fireboy : public BaseCharacter {
public:
    Fireboy(EventManager* eventManager, GameEngine *engine, bool activePlayer = false);
};
