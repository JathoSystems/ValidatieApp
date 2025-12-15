#ifndef FIREBOY_HPP
#define FIREBOY_HPP

#include "BaseCharacter.hpp"

class Fireboy : public BaseCharacter {
public:
    Fireboy(EventManager* eventManager, GameEngine *engine, bool activePlayer = false);
};

#endif // FIREBOY_HPP