//
// Created by jusra on 5-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_MOVEEVENT_HPP
#define VUURJONGEN_WATERMEISJE_MOVEEVENT_HPP
#include "Events/EventRegistry.h"
#include "Events/IEvent.h"

class MoveEvent : public IEvent {
private:
    int _x, _y;
public:
    MoveEvent(int x, int y);

    std::string getName() const override;
    Package serialize() const override;
    Data deserialize(const Package &package) override;
    void apply(GameObject* gameObject) override;
};

#endif //VUURJONGEN_WATERMEISJE_MOVEEVENT_HPP