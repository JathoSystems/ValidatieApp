#ifndef VALIDATIEAPP_BATMOVEEVENT_HPP
#define VALIDATIEAPP_BATMOVEEVENT_HPP

#include "Events/IEvent.h"
#include "Events/EventRegistry.h"
#include <mutex>
#include <vector>
#include <functional>

extern std::mutex eventMutex;
extern std::vector<std::function<void()>> eventQueue;

class BatMoveEvent : public IEvent {
private:
    int _objectId;
    float _x;
    float _y;

public:
    BatMoveEvent(int objectId, float x, float y);

    std::string getName() const override;
    Package serialize() const override;
    Data deserialize(const Package &package) override;
    void apply(GameObject* gameObject) override;
};

#endif //VALIDATIEAPP_BATMOVEEVENT_HPP
