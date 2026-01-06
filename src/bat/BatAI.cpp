#include "bat/BatAI.h"
#include "bat/Bat.h"
#include "grid/LevelGrid.h"
#include "grid/AStarPathfinder.h"
#include "GameObjects/Transform/Position.h"
#include "GameObjects/Transform/Transform.h"
#include "GameObjects/Spritesheet/Animator.h"
#include "bat/events/BatMoveEvent.hpp"
#include "Events/EventManager.h"
#include "GameObjects/ObjectRegistry.hpp"
#include "Physics/PhysicsComponent.h"
#include <cmath>
#include <algorithm>
#include <iostream>

BatAI::BatAI(Bat* bat, LevelGrid* grid, Scene* scene, int cellSize, float speed, bool isNetworked, EventManager* eventManager, int objectId, bool isAuthoritative)
    : _grid(grid), _scene(scene), _cellSize(cellSize), _speed(speed),
      _pathfinder(std::make_unique<AStarPathfinder>(grid)),
      _currentPathIndex(0),
      _targetChangeTimer(0.0f),
      _targetChangeInterval(5.0f),
      _stuckTimer(0.0f),
      _accumulatedX(0.0f),
      _accumulatedY(0.0f),
      _rng(std::random_device{}()),
      _timerDist(3.0f, 7.0f), // 3-7 seconds instead of 1-3
      _isNetworked(isNetworked),
      _networkSyncTimer(0.0f),
      _networkSyncInterval(0.1f),
      _eventManager(eventManager),
      _objectId(objectId),
      _isAuthoritative(isAuthoritative) {

    if (_isAuthoritative && _grid) {
        float minWorldX, minWorldY, maxWorldX, maxWorldY;
        _grid->gridToWorld(0, 0, minWorldX, minWorldY);
        _grid->gridToWorld(_grid->getWidth() - 1, _grid->getHeight() - 1, maxWorldX, maxWorldY);
        maxWorldX += _cellSize;
        maxWorldY += _cellSize;

        chooseNewTarget();
    }
}

void BatAI::update(float deltaTime) {
    if (!_parent || !_grid || !_pathfinder) return;
    
    if (_isAuthoritative) {
        updatePathfinding(deltaTime);
    }
    
    updateMovement(deltaTime);
}

void BatAI::render(const std::unique_ptr<Window>& window) {
}

void BatAI::updatePathfinding(float deltaTime) {
    _targetChangeTimer += deltaTime;
    
    // Check if we need a new target
    if (_targetChangeTimer >= _targetChangeInterval || _currentPath.empty() || _currentPathIndex >= _currentPath.size()) {
        chooseNewTarget();
        _targetChangeTimer = 0.0f;
        _targetChangeInterval = _timerDist(_rng);
    }
}

void BatAI::updateMovement(float deltaTime) {
    if (!_parent || !_grid) return;
    
    Transform* transform = _parent->getTransform();
    if (!transform) return;
    
    Position* pos = transform->getPosition();
    if (!pos) return;
    
    float currentX = static_cast<float>(pos->getX());
    float currentY = static_cast<float>(pos->getY());
    
    // Follow path if we have one
    if (!_currentPath.empty() && _currentPathIndex < _currentPath.size()) {
        float targetX = _currentPath[_currentPathIndex].first;
        float targetY = _currentPath[_currentPathIndex].second;
        
        float dx = targetX - currentX;
        float dy = targetY - currentY;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        const float waypointReachDistance = static_cast<float>(_cellSize) * 0.3f; // Smaller threshold

        static float lastMoveTime = 0.0f;
        static float lastX = 0.0f;
        static float lastY = 0.0f;
        bool hasMoved = (std::abs(currentX - lastX) > 0.1f || std::abs(currentY - lastY) > 0.1f);
        
        if (distance < waypointReachDistance && (hasMoved || lastMoveTime > 0.1f)) {
            _currentPathIndex++;
            lastMoveTime = 0.0f;
            if (_currentPathIndex < _currentPath.size()) {
                targetX = _currentPath[_currentPathIndex].first;
                targetY = _currentPath[_currentPathIndex].second;
                dx = targetX - currentX;
                dy = targetY - currentY;
                distance = std::sqrt(dx * dx + dy * dy);
            } else {
                // Reached end of path - choose new target immediately
                chooseNewTarget();
                return;
            }
        }
        
        lastMoveTime += deltaTime;
        lastX = currentX;
        lastY = currentY;
        
        // Move towards target
        if (distance > 0.0f) {
            float directionX = dx / distance;
            float directionY = dy / distance;
            
            // Flip sprite based on direction (negative width flips horizontally)
            Size* size = transform->getSize();
            if (size) {
                float currentWidth = size->getWidth();
                float absWidth = (currentWidth < 0) ? -currentWidth : currentWidth;
                
                if (directionX < 0.0f) {
                    // Facing left - flip by using negative width
                    size->setWidth(-absWidth);
                } else {
                    // Facing right - normal width
                    size->setWidth(absWidth);
                }
            }
            
            // Use consistent movement - clamp deltaTime to prevent huge jumps
            float clampedDelta = std::min(deltaTime, 0.016f); // Max 60 FPS equivalent
            float moveDistance = _speed * clampedDelta;
            float moveX = directionX * moveDistance;
            float moveY = directionY * moveDistance;
            
            // Don't overshoot the target
            if (moveDistance > distance) {
                moveX = dx;
                moveY = dy;
            }
            
            float newX = currentX + moveX;
            float newY = currentY + moveY;
            
            // Check if the path to the new position is walkable
            // Sample multiple points along the movement to ensure we don't go through blocks
            bool canMove = true;
            const int samples = 10; // Check 10 points along the path for better collision detection
            for (int i = 1; i <= samples; i++) {
                float t = static_cast<float>(i) / static_cast<float>(samples);
                float checkX = currentX + moveX * t;
                float checkY = currentY + moveY * t;
                
                // Check multiple points around the bat's position to account for its size
                float batHalfSize = static_cast<float>(_cellSize) * 0.5f;
                std::vector<std::pair<float, float>> checkPoints = {
                    {checkX, checkY}, // Center
                    {checkX - batHalfSize, checkY}, // Left
                    {checkX + batHalfSize, checkY}, // Right
                    {checkX, checkY - batHalfSize}, // Top
                    {checkX, checkY + batHalfSize}  // Bottom
                };
                
                for (const auto& point : checkPoints) {
                    if (!isPositionWalkable(point.first, point.second)) {
                        canMove = false;
                        break;
                    }
                }
                
                if (!canMove) break;
            }
            
            // Also check the final position
            if (canMove) {
                float batHalfSize = static_cast<float>(_cellSize) * 0.5f;
                std::vector<std::pair<float, float>> finalCheckPoints = {
                    {newX, newY},
                    {newX - batHalfSize, newY},
                    {newX + batHalfSize, newY},
                    {newX, newY - batHalfSize},
                    {newX, newY + batHalfSize}
                };
                
                for (const auto& point : finalCheckPoints) {
                    if (!isPositionWalkable(point.first, point.second)) {
                        canMove = false;
                        break;
                    }
                }
            }
            
            // Debug logging for movement attempts
            static int moveAttemptCount = 0;
            moveAttemptCount++;
            if (moveAttemptCount % 60 == 0) { // Log every 60th attempt (roughly once per second at 60fps)
                if (!canMove) {
                    int gridX, gridY;
                    _grid->worldToGrid(newX, newY, gridX, gridY);
                }
            }
            
            // If we can't move directly, try to move closer to the waypoint in smaller steps
            if (!canMove) {
                // Try moving in smaller increments along the path
                float stepSize = moveDistance / 3.0f; // Try 3 smaller steps
                for (int step = 1; step <= 3; step++) {
                    float stepX = directionX * stepSize * static_cast<float>(step);
                    float stepY = directionY * stepSize * static_cast<float>(step);
                    float testX = currentX + stepX;
                    float testY = currentY + stepY;
                    
                    // Check if this step is walkable
                    bool stepWalkable = true;
                    for (int i = 1; i <= 3; i++) {
                        float t = static_cast<float>(i) / 3.0f;
                        float checkX = currentX + stepX * t;
                        float checkY = currentY + stepY * t;
                        if (!isPositionWalkable(checkX, checkY)) {
                            stepWalkable = false;
                            break;
                        }
                    }
                    
                    if (stepWalkable && isPositionWalkable(testX, testY)) {
                        canMove = true;
                        newX = testX;
                        newY = testY;
                        moveX = stepX;
                        moveY = stepY;
                        break;
                    }
                }
            }
            
            if (canMove) {
                // Accumulate movement for sub-pixel precision
                _accumulatedX += moveX;
                _accumulatedY += moveY;
                
                // Only update position when we've accumulated at least 1 pixel
                int pixelMoveX = static_cast<int>(_accumulatedX);
                int pixelMoveY = static_cast<int>(_accumulatedY);
                
                if (pixelMoveX != 0 || pixelMoveY != 0) {
                    int oldX = pos->getX();
                    int oldY = pos->getY();
                    pos->setX(oldX + pixelMoveX);
                    pos->setY(oldY + pixelMoveY);
                    
                    // Keep the fractional part
                    _accumulatedX -= static_cast<float>(pixelMoveX);
                    _accumulatedY -= static_cast<float>(pixelMoveY);
                    
                    _stuckTimer = 0.0f; // Reset stuck timer when moving successfully
                    
                    // Log actual position update
                    static int updateCount = 0;
                    updateCount++;
                } else {
                    // No pixel movement yet, but we're accumulating
                    _stuckTimer = 0.0f;
                }
                
                if (_isNetworked) {
                    _networkSyncTimer += deltaTime;
                    if (_networkSyncTimer >= _networkSyncInterval) {
                        syncToNetwork();
                        _networkSyncTimer = 0.0f;
                    }
                }
            } else {
                // Can't move to this position - skip to next waypoint or choose new target
                _stuckTimer += deltaTime;
                if (_stuckTimer > 0.5f) { // If stuck for more than 0.5 seconds
                   _stuckTimer = 0.0f;
                    
                    // Skip to next waypoint
                    _currentPathIndex++;
                    if (_currentPathIndex >= _currentPath.size()) {
                        chooseNewTarget();
                    }
                }
            }
        } else {
            // Already at waypoint, move to next
            _currentPathIndex++;
            if (_currentPathIndex >= _currentPath.size()) {
                chooseNewTarget();
            }
        }
    } else {
        // No path - choose new target
        if (_isAuthoritative) {
            chooseNewTarget();
        }
    }
}

void BatAI::chooseNewTarget() {
    if (!_parent || !_grid || !_pathfinder) return;
    
    Transform* transform = _parent->getTransform();
    if (!transform) return;
    
    Position* pos = transform->getPosition();
    if (!pos) return;
    
    float currentX = static_cast<float>(pos->getX());
    float currentY = static_cast<float>(pos->getY());
    
    // Get current grid position
    int currentGridX, currentGridY;
    _grid->worldToGrid(currentX, currentY, currentGridX, currentGridY);
    
    // Check if current position is walkable
    bool currentWalkable = _grid->isWalkable(currentGridX, currentGridY);
    if (!currentWalkable) {
        // Try to find a nearby walkable position
        bool foundNearby = false;
        for (int radius = 1; radius <= 3 && !foundNearby; radius++) {
            for (int dx = -radius; dx <= radius && !foundNearby; dx++) {
                for (int dy = -radius; dy <= radius && !foundNearby; dy++) {
                    int testX = currentGridX + dx;
                    int testY = currentGridY + dy;
                    if (_grid->isWalkable(testX, testY)) {
                        float newWorldX, newWorldY;
                        _grid->gridToWorld(testX, testY, newWorldX, newWorldY);
                        newWorldX += _cellSize / 2.0f;
                        newWorldY += _cellSize / 2.0f;
                        pos->setX(static_cast<int>(newWorldX));
                        pos->setY(static_cast<int>(newWorldY));
                        currentX = newWorldX;
                        currentY = newWorldY;
                        currentGridX = testX;
                        currentGridY = testY;
                        foundNearby = true;
                    }
                }
            }
        }
    }
    
    // Get a random walkable position
    auto targetGrid = _pathfinder->getRandomWalkablePosition();
    float targetX, targetY;
    _grid->gridToWorld(targetGrid.first, targetGrid.second, targetX, targetY);
    // Center in cell
    targetX += _cellSize / 2.0f;
    targetY += _cellSize / 2.0f;
    
   // Find path to target
    _currentPath = _pathfinder->findPath(currentX, currentY, targetX, targetY);
    _currentPathIndex = 0;
    
    // If path is empty or too short, try again
    if (_currentPath.empty() || _currentPath.size() < 2) {
        // Try a different target
        for (int i = 0; i < 5; i++) {
            targetGrid = _pathfinder->getRandomWalkablePosition();
            _grid->gridToWorld(targetGrid.first, targetGrid.second, targetX, targetY);
            targetX += _cellSize / 2.0f;
            targetY += _cellSize / 2.0f;
            
            _currentPath = _pathfinder->findPath(currentX, currentY, targetX, targetY);
            if (!_currentPath.empty() && _currentPath.size() >= 2) {
                break;
            }
        }
    }
    
    if (_currentPath.empty()) {
        std::cout << "  WARNING: No path found to target!" << std::endl;
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
    
    float dirX = getDirectionX();
    float dirY = getDirectionY();
    auto event = std::make_shared<BatMoveEvent>(_objectId, dirX, dirY);
    _eventManager->broadcast(_objectId, event);
}

void BatAI::setDirection(float directionX, float directionY) {
    // This is for network sync - not used with pathfinding
    // But we keep it for compatibility
}

float BatAI::getDirectionX() const {
    if (!_parent || _currentPath.empty() || _currentPathIndex >= _currentPath.size()) {
        return 0.0f;
    }
    
    Transform* transform = _parent->getTransform();
    if (!transform) return 0.0f;
    
    Position* pos = transform->getPosition();
    if (!pos) return 0.0f;
    
    float currentX = static_cast<float>(pos->getX());
    float targetX = _currentPath[_currentPathIndex].first;
    float dx = targetX - currentX;
    
    if (std::abs(dx) < 0.001f) return 0.0f;
    return dx > 0.0f ? 1.0f : -1.0f;
}

float BatAI::getDirectionY() const {
    if (!_parent || _currentPath.empty() || _currentPathIndex >= _currentPath.size()) {
        return 0.0f;
    }
    
    Transform* transform = _parent->getTransform();
    if (!transform) return 0.0f;
    
    Position* pos = transform->getPosition();
    if (!pos) return 0.0f;
    
    float currentY = static_cast<float>(pos->getY());
    float targetY = _currentPath[_currentPathIndex].second;
    float dy = targetY - currentY;
    
    if (std::abs(dy) < 0.001f) return 0.0f;
    return dy > 0.0f ? 1.0f : -1.0f;
}
