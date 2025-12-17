//
// Created by jusra on 5-12-2025.
//

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

public:
    MoveEvent(int _objectId, Direction direction, bool toggle);

    std::string getName() const override;
    Package serialize() const override;
    Data deserialize(const Package &package) override;
    void apply(GameObject* gameObject) override;
};

#endif //VUURJONGEN_WATERMEISJE_MOVEEVENT_HPP