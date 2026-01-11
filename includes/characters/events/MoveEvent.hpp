#ifndef VUURJONGEN_WATERMEISJE_MOVEEVENT_HPP
#define VUURJONGEN_WATERMEISJE_MOVEEVENT_HPP

#include "Events/IEvent.h"
#include "enums/Direction.hpp"

class MoveEvent : public IEvent {
private:
    int _objectId;
    Direction _direction;
    bool _toggle;
    float _x;
    float _y;
    float _vx;
    float _vy;

public:
    MoveEvent(int objectId = 0, Direction direction = Direction::NONE, bool toggle = false,
              float x = 0.0f, float y = 0.0f, float vx = 0.0f, float vy = 0.0f);

    std::string getName() const override;

    Package serialize() const override;

    Data deserialize(const Package &package) override;

    void apply(GameObject *gameObject) override;
};

#endif
