#include "bat/BatAI.h"
#include "bat/Bat.h"
#include "grid/LevelGrid.h"
#include "GameObjects/Transform/Position.h"
#include "GameObjects/Transform/Transform.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "bat/events/BatMoveEvent.hpp"
#include "Events/EventManager.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "Physics/PhysicsComponent.h"
#include <cmath>
#include <algorithm>

BatAI::BatAI(Bat* bat, LevelGrid* grid, Scene* scene, int cellSize, float speed, bool isNetworked, EventManager* eventManager, int objectId, bool isAuthoritative)
    : _grid(grid), _scene(scene), _cellSize(cellSize), _speed(speed),
      _directionX(0.0f), _directionY(0.0f),
      _changeDirectionTimer(0.0f), _changeDirectionInterval(2.0f),
      _rng(std::random_device{}()),
      _directionDist(-1.0f, 1.0f),
      _timerDist(1.0f, 3.0f),
      _directionTypeDist(0, 7),
      _isNetworked(isNetworked),
      _networkSyncTimer(0.0f),
      _networkSyncInterval(0.1f),
      _eventManager(eventManager),
      _objectId(objectId),
      _isAuthoritative(isAuthoritative) {

    if (_isAuthoritative) {
        chooseNewDirection();
    } else {
        _directionX = -0.707f;
        _directionY = 0.707f;
        float length = std::sqrt(_directionX * _directionX + _directionY * _directionY);
        if (length > 0.0f) {
            _directionX /= length;
            _directionY /= length;
        }
    }
}

void BatAI::update(float deltaTime) {
    if (!_parent || !_grid) return;
    
    updateMovement(deltaTime);

    if (_isAuthoritative) {
        _changeDirectionTimer += deltaTime;
        if (_changeDirectionTimer >= _changeDirectionInterval) {
            chooseNewDirection();
            _changeDirectionTimer = 0.0f;
            _changeDirectionInterval = _timerDist(_rng);
            
            if (_isNetworked) {
                syncToNetwork();
            }
        }
    }
}

void BatAI::render(const std::unique_ptr<Window>& window) {
}

void BatAI::updateMovement(float deltaTime) {
    if (!_parent || !_grid) return;
    
    Transform* transform = _parent->getTransform();
    if (!transform) return;
    
    Position* pos = transform->getPosition();
    if (!pos) return;
    
    // Flip sprite based on direction (negative width flips horizontally)
    if (transform) {
        Size* size = transform->getSize();
        if (size) {
            float currentWidth = size->getWidth();
            float absWidth = (currentWidth < 0) ? -currentWidth : currentWidth;
            
            if (_directionX < 0.0f) {
                // Facing left - flip by using negative width
                size->setWidth(-absWidth);
            } else {
                // Facing right - normal width
                size->setWidth(absWidth);
            }
        }
    }
    
    float currentX = static_cast<float>(pos->getX());
    float currentY = static_cast<float>(pos->getY());
    
    float moveDistance = _speed * deltaTime;
    float newX = currentX + (_directionX * moveDistance);
    float newY = currentY + (_directionY * moveDistance);
    
    float batWidth = static_cast<float>(transform->getSize()->getWidth());
    float batHeight = static_cast<float>(transform->getSize()->getHeight());
    
    float halfWidth = batWidth / 2.0f;
    float halfHeight = batHeight / 2.0f;
    
    bool gridWalkable = isPositionWalkable(newX, newY);
    
    int walkableCorners = 0;
    if (isPositionWalkable(newX - halfWidth, newY - halfHeight)) walkableCorners++;
    if (isPositionWalkable(newX + halfWidth, newY - halfHeight)) walkableCorners++;
    if (isPositionWalkable(newX - halfWidth, newY + halfHeight)) walkableCorners++;
    if (isPositionWalkable(newX + halfWidth, newY + halfHeight)) walkableCorners++;
    
    gridWalkable = gridWalkable && (walkableCorners >= 3);
    
    bool noDynamicCollision = !collidesWithDynamicObjects(newX, newY, batWidth, batHeight);
    
    bool canMove = gridWalkable && noDynamicCollision;
    
    if (canMove) {
        pos->setX(static_cast<int>(newX));
        pos->setY(static_cast<int>(newY));
    } else {
        if (_isAuthoritative) {
            chooseNewDirection();
            if (_isNetworked) {
                syncToNetwork();
            }
            
            newX = currentX + (_directionX * moveDistance);
            newY = currentY + (_directionY * moveDistance);
            
            bool gridWalkableNew = isPositionWalkable(newX, newY);
            int walkableCornersNew = 0;
            if (isPositionWalkable(newX - halfWidth, newY - halfHeight)) walkableCornersNew++;
            if (isPositionWalkable(newX + halfWidth, newY - halfHeight)) walkableCornersNew++;
            if (isPositionWalkable(newX - halfWidth, newY + halfHeight)) walkableCornersNew++;
            if (isPositionWalkable(newX + halfWidth, newY + halfHeight)) walkableCornersNew++;
            gridWalkableNew = gridWalkableNew && (walkableCornersNew >= 3);
            
            bool noDynamicCollisionNew = !collidesWithDynamicObjects(newX, newY, batWidth, batHeight);
            canMove = gridWalkableNew && noDynamicCollisionNew;
            
            if (canMove) {
                pos->setX(static_cast<int>(newX));
                pos->setY(static_cast<int>(newY));
            }
        }
    }
}

void BatAI::chooseNewDirection() {
    int directionType = _directionTypeDist(_rng);
    
    switch (directionType) {
        case 0:
            _directionX = 1.0f;
            _directionY = 0.0f;
            break;
        case 1:
            _directionX = -1.0f;
            _directionY = 0.0f;
            break;
        case 2:
            _directionX = 0.0f;
            _directionY = -1.0f;
            break;
        case 3:
            _directionX = 0.0f;
            _directionY = 1.0f;
            break;
        case 4:
            _directionX = 0.707f;
            _directionY = -0.707f;
            break;
        case 5:
            _directionX = -0.707f;
            _directionY = -0.707f;
            break;
        case 6:
            _directionX = 0.707f;
            _directionY = 0.707f;
            break;
        case 7:
            _directionX = -0.707f;
            _directionY = 0.707f;
            break;
        default:
            _directionX = 1.0f;
            _directionY = 0.0f;
            break;
    }
    
    float length = std::sqrt(_directionX * _directionX + _directionY * _directionY);
    if (length > 0.0f) {
        _directionX /= length;
        _directionY /= length;
    }
}

bool BatAI::canMoveTo(float worldX, float worldY) const {
    if (!_grid) return false;
    
    int gridX, gridY;
    _grid->worldToGrid(worldX, worldY, gridX, gridY);
    
    if (gridX < 0 || gridX >= _grid->getWidth() || 
        gridY < 0 || gridY >= _grid->getHeight()) {
        return false;
    }
    
    return _grid->isWalkable(gridX, gridY);
}

bool BatAI::isPositionWalkable(float worldX, float worldY) const {
    return canMoveTo(worldX, worldY);
}

bool BatAI::collidesWithDynamicObjects(float worldX, float worldY, float batWidth, float batHeight) const {
    if (!_scene || !_parent) return false;
    
    float batLeft = worldX - batWidth / 2.0f;
    float batTop = worldY - batHeight / 2.0f;
    float batRight = worldX + batWidth / 2.0f;
    float batBottom = worldY + batHeight / 2.0f;
    
    const auto& objects = _scene->getObjects();
    for (const auto& obj : objects) {
        if (obj.get() == _parent) continue;
        
        PhysicsComponent* physics = obj->getComponent<PhysicsComponent>();
        if (!physics) continue;
        
        Transform* objTransform = obj->getTransform();
        if (!objTransform) continue;
        
        Position* objPos = objTransform->getPosition();
        Size* objSize = objTransform->getSize();
        if (!objPos || !objSize) continue;
        
        float objCenterX = static_cast<float>(objPos->getX());
        float objCenterY = static_cast<float>(objPos->getY());
        float objWidth = static_cast<float>(objSize->getWidth());
        float objHeight = static_cast<float>(objSize->getHeight());
        
        float objLeft = objCenterX - objWidth / 2.0f;
        float objTop = objCenterY - objHeight / 2.0f;
        float objRight = objCenterX + objWidth / 2.0f;
        float objBottom = objCenterY + objHeight / 2.0f;
        
        if (batRight > objLeft && batLeft < objRight &&
            batBottom > objTop && batTop < objBottom) {
            return true;
        }
    }
    
    return false;
}

void BatAI::syncToNetwork() {
    if (!_parent || !_eventManager) return;
    if (_objectId == -1) return;
    
    auto event = std::make_shared<BatMoveEvent>(_objectId, _directionX, _directionY);
    _eventManager->broadcast(_objectId, event);
}

void BatAI::setDirection(float directionX, float directionY) {
    _directionX = directionX;
    _directionY = directionY;
    
    float length = std::sqrt(_directionX * _directionX + _directionY * _directionY);
    if (length > 0.0f) {
        _directionX /= length;
        _directionY /= length;
    }
    
    if (!_isAuthoritative) {
        _changeDirectionTimer = 0.0f;
        _changeDirectionInterval = 2.0f;
    }
}
