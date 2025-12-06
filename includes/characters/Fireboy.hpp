#include "GameObjects/GameObject.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "characters/movement/MovementComponent.hpp"

class Fireboy : public GameObject {
public:
    Fireboy(GameEngine* engine);
};
