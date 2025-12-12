//
// Created by kikker234 on 11-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_JUMPEVENT_H
#define VUURJONGEN_WATERMEISJE_JUMPEVENT_H
#include "Events/IEvent.h"
#include <string>


class JumpEvent : public IEvent {
private:
    int _objectId;

public:
    JumpEvent(int objectId = 0): _objectId(objectId) {}
    std::string getName() const override;
    Package serialize() const override;
    Data deserialize(const Package &package) override;
    void apply(GameObject* gameObject) override;
};

#endif //VUURJONGEN_WATERMEISJE_JUMPEVENT_H