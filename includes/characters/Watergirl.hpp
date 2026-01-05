#ifndef VUURJONGEN_WATERMEISJE_WATERGIRL_HPP
#define VUURJONGEN_WATERMEISJE_WATERGIRL_HPP
#include "BaseCharacter.hpp"
#include "Engine/GameEngine.h"

class Watergirl : public BaseCharacter {
public:
    Watergirl(std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine, bool active);
    Watergirl(int parentId, std::shared_ptr<NetworkSystem> network, EventManager *eventManager, GameEngine *engine, bool active);
    
    static KeyBindings getDefaultBindings() {
        return { Key::LEFT, Key::RIGHT, Key::UP };
    }
};

#endif