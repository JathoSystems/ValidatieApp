#ifndef VALIDATIEAPP_BATAI_H
#define VALIDATIEAPP_BATAI_H

#include "GameObjects/Component/Component.h"
#include "grid/LevelGrid.h"
#include "grid/AStarPathfinder.h"
#include "Events/EventManager.h"
#include "Scenes/Scene.h"
#include <random>
#include <memory>
#include <vector>
#include <queue>

class Bat;

class BatAI : public Component {
private:
    LevelGrid* _grid;
    Scene* _scene;
    int _cellSize;
    float _speed;

    // Pathfinding
    std::unique_ptr<AStarPathfinder> _pathfinder;
    std::vector<std::pair<float, float>> _currentPath;
    size_t _currentPathIndex;
    float _targetChangeTimer;
    float _targetChangeInterval;
    float _stuckTimer;
    
    // Movement accumulation for sub-pixel movement
    float _accumulatedX;
    float _accumulatedY;
    
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _timerDist;

    bool _isNetworked;
    float _networkSyncTimer;
    float _networkSyncInterval;
    EventManager* _eventManager;
    int _objectId;
    bool _isAuthoritative;

public:
    BatAI(Bat* bat, LevelGrid* grid, Scene* scene, int cellSize, float speed = 80.0f, bool isNetworked = false, EventManager* eventManager = nullptr, int objectId = -1, bool isAuthoritative = true);
    
    void update(float deltaTime) override;
    void render(const std::unique_ptr<Window>& window) override;
    
    void setSpeed(float speed) { _speed = speed; }
    void setIsNetworked(bool networked) { _isNetworked = networked; }
    void setEventManager(EventManager* eventManager) { _eventManager = eventManager; }
    void setDirection(float directionX, float directionY);
    
    // Get current direction for sprite flipping
    float getDirectionX() const;
    float getDirectionY() const;

private:
    void updateMovement(float deltaTime);
    void chooseNewTarget();
    void updatePathfinding(float deltaTime);
    bool canMoveTo(float worldX, float worldY) const;
    bool isPositionWalkable(float worldX, float worldY) const;
    bool collidesWithDynamicObjects(float worldX, float worldY, float batWidth, float batHeight) const;
    void syncToNetwork();
};

#endif //VALIDATIEAPP_BATAI_H
