#ifndef VUURJONGEN_WATERMEISJE_MOVEEVENT_HPP
#define VUURJONGEN_WATERMEISJE_MOVEEVENT_HPP

#include "enums/Direction.hpp"
#include "Events/EventRegistry.h"
#include "Events/IEvent.h"

class MoveEvent : public IEvent {
private:
    int _objectId;
    Direction _direction;
    bool _toggle;
    float _x;
    float _y;

public:
    // Update constructor to take x and y
    MoveEvent(int objectId, Direction direction, bool toggle, float x = 0.0f, float y = 0.0f);

    std::string getName() const override;
    Package serialize() const override;
    Data deserialize(const Package &package) override;
    void apply(GameObject* gameObject) override;
};

#endif