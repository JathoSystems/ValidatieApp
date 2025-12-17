#ifndef VALIDATIEAPP_BATAI_H
#define VALIDATIEAPP_BATAI_H

#include "GameObjects/Component/Component.h"
#include "grid/LevelGrid.h"
#include "Events/EventManager.h"
#include "Scenes/Scene.h"
#include <random>
#include <memory>

class Bat;

class BatAI : public Component {
private:
    LevelGrid* _grid;
    Scene* _scene;
    int _cellSize;
    float _speed;

    float _directionX;
    float _directionY;
    float _changeDirectionTimer;
    float _changeDirectionInterval;
    
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _directionDist;
    std::uniform_real_distribution<float> _timerDist;
    std::uniform_int_distribution<int> _directionTypeDist;

    bool _isNetworked;
    float _networkSyncTimer;
    float _networkSyncInterval;
    EventManager* _eventManager;
    int _objectId;
    bool _isAuthoritative;

public:
    BatAI(Bat* bat, LevelGrid* grid, Scene* scene, int cellSize, float speed = 50.0f, bool isNetworked = false, EventManager* eventManager = nullptr, int objectId = -1, bool isAuthoritative = true);
    
    void update(float deltaTime) override;
    void render(const std::unique_ptr<Window>& window) override;
    
    void setSpeed(float speed) { _speed = speed; }
    void setIsNetworked(bool networked) { _isNetworked = networked; }
    void setEventManager(EventManager* eventManager) { _eventManager = eventManager; }
    void setDirection(float directionX, float directionY);
    
    // Get current direction for sprite flipping
    float getDirectionX() const { return _directionX; }
    float getDirectionY() const { return _directionY; }

private:
    void updateMovement(float deltaTime);
    void chooseNewDirection();
    bool canMoveTo(float worldX, float worldY) const;
    bool isPositionWalkable(float worldX, float worldY) const;
    bool collidesWithDynamicObjects(float worldX, float worldY, float batWidth, float batHeight) const;
    void syncToNetwork();
};

#endif //VALIDATIEAPP_BATAI_H
