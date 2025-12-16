#ifndef FIREBOY_HPP
#define FIREBOY_HPP

#include "BaseCharacter.hpp"

class Fireboy : public BaseCharacter {
public:
    Fireboy(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine, bool active);
};

#endif // FIREBOY_HPP