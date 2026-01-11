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
#include "characters/BaseCharacter.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>

BatAI::BatAI(Bat* bat, LevelGrid* grid, Scene* scene, int cellSize, float speed, bool isNetworked, EventManager* eventManager, int objectId, bool isAuthoritative)
    : _grid(grid), _scene(scene), _cellSize(cellSize), _speed(speed),
      _pathfinder(nullptr),
      _currentPathIndex(0),
      _targetChangeTimer(0.0f),
      _targetChangeInterval(5.0f),
      _stuckTimer(0.0f),
      _accumulatedX(0.0f),
      _accumulatedY(0.0f),
      _fleeDistance(400.0f),
      _isFleeing(false),
      _fleeSpeedMultiplier(1.5f),
      _rng(std::random_device{}()),
      _timerDist(3.0f, 7.0f),
      _isNetworked(isNetworked),
      _networkSyncTimer(0.0f),
      _networkSyncInterval(0.033f),
      _eventManager(eventManager),
      _objectId(objectId),
      _isAuthoritative(isAuthoritative),
      _lastNetworkX(0.0f),
      _lastNetworkY(0.0f),
      _hasNetworkUpdate(false),
      _needsInitialPath(true),
      _previousX(0.0f),
      _previousY(0.0f) {

    if (!grid) {
        std::cerr << "[BatAI] ERROR: Grid is nullptr in constructor!" << std::endl;
        return;
    }

    _pathfinder = std::make_unique<AStarPathfinder>(grid);

    if (bat && !_isAuthoritative) {
        Transform* transform = bat->getTransform(); // <-- GEWIJZIGD
        if (transform && transform->getPosition()) {
            _lastNetworkX = static_cast<float>(transform->getPosition()->getX());
            _lastNetworkY = static_cast<float>(transform->getPosition()->getY());
            _previousX = _lastNetworkX;
            _previousY = _lastNetworkY;
        }
    }

    if (_isAuthoritative && _grid) {
        float minWorldX, minWorldY, maxWorldX, maxWorldY;
        _grid->gridToWorld(0, 0, minWorldX, minWorldY);
        _grid->gridToWorld(_grid->getWidth() - 1, _grid->getHeight() - 1, maxWorldX, maxWorldY);
        maxWorldX += _cellSize;
        maxWorldY += _cellSize;
    }
}

void BatAI::update(float deltaTime) {
    if (!_parent || !_grid || !_pathfinder || !_scene) return;

    if (_needsInitialPath && _isAuthoritative) {
        chooseNewTarget();
        _needsInitialPath = false;
    }

    if (!_isAuthoritative && _isNetworked) {
        applyNetworkPosition();
    }

    if (_isAuthoritative) {
        updatePathfinding(deltaTime);
    }

    updateMovement(deltaTime);
}

void BatAI::render(const std::unique_ptr<Window>& window) {
}

void BatAI::updatePathfinding(float deltaTime) {
    float distanceToPlayer = 0.0f;
    GameObject* nearestPlayer = findNearestPlayer(distanceToPlayer);

    if (nearestPlayer && distanceToPlayer < _fleeDistance) {
        bool wasFleeing = _isFleeing;
        _isFleeing = true;

        Transform* playerTransform = nearestPlayer->getTransform();
        if (playerTransform && playerTransform->getPosition()) {
            float playerX = static_cast<float>(playerTransform->getPosition()->getX());
            float playerY = static_cast<float>(playerTransform->getPosition()->getY());

            if (!wasFleeing || _currentPath.empty() || _currentPathIndex >= _currentPath.size()) {
                chooseFleeTarget(playerX, playerY);
                _targetChangeTimer = 0.0f;
            } else {
                _targetChangeTimer += deltaTime;
                if (_targetChangeTimer > 0.2f) {
                    chooseFleeTarget(playerX, playerY);
                    _targetChangeTimer = 0.0f;
                }
            }
        }
    } else {
        if (_isFleeing) {
            _isFleeing = false;
            chooseNewTarget();
            _targetChangeTimer = 0.0f;
            _targetChangeInterval = _timerDist(_rng);
        } else {
            _targetChangeTimer += deltaTime;
            if (_targetChangeTimer >= _targetChangeInterval || _currentPath.empty() || _currentPathIndex >= _currentPath.size()) {
                chooseNewTarget();
                _targetChangeTimer = 0.0f;
                _targetChangeInterval = _timerDist(_rng);
            }
        }
    }
}

void BatAI::updateMovement(float deltaTime) {
    if (!_parent || !_grid) return;

    if (!_isAuthoritative) {
        return;
    }

    Transform* transform = _parent->getTransform();
    if (!transform) return;

    Position* pos = transform->getPosition();
    if (!pos) return;

    float currentX = static_cast<float>(pos->getX());
    float currentY = static_cast<float>(pos->getY());

    if (!_currentPath.empty() && _currentPathIndex < _currentPath.size()) {
        float targetX = _currentPath[_currentPathIndex].first;
        float targetY = _currentPath[_currentPathIndex].second;

        float dx = targetX - currentX;
        float dy = targetY - currentY;
        float distance = std::sqrt(dx * dx + dy * dy);

        const float waypointReachDistance = static_cast<float>(_cellSize) * 0.3f;

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
                chooseNewTarget();
                return;
            }
        }

        lastMoveTime += deltaTime;
        lastX = currentX;
        lastY = currentY;

        if (distance > 0.0f) {
            float directionX = dx / distance;
            float directionY = dy / distance;

            Size* size = transform->getSize();
            if (size) {
                float currentWidth = size->getWidth();
                float absWidth = (currentWidth < 0) ? -currentWidth : currentWidth;

                if (directionX < 0.0f) {
                    size->setWidth(-absWidth);
                } else {
                    size->setWidth(absWidth);
                }
            }

            float clampedDelta = std::min(deltaTime, 0.016f);
            float currentSpeed = _isFleeing ? _speed * _fleeSpeedMultiplier : _speed;
            float moveDistance = currentSpeed * clampedDelta;
            float moveX = directionX * moveDistance;
            float moveY = directionY * moveDistance;

            if (moveDistance > distance) {
                moveX = dx;
                moveY = dy;
            }

            float newX = currentX + moveX;
            float newY = currentY + moveY;

            bool canMove = true;
            const int samples = 10;
            for (int i = 1; i <= samples; i++) {
                float t = static_cast<float>(i) / static_cast<float>(samples);
                float checkX = currentX + moveX * t;
                float checkY = currentY + moveY * t;

                float batHalfSize = static_cast<float>(_cellSize) * 0.5f;
                std::vector<std::pair<float, float>> checkPoints = {
                    {checkX, checkY},
                    {checkX - batHalfSize, checkY},
                    {checkX + batHalfSize, checkY},
                    {checkX, checkY - batHalfSize},
                    {checkX, checkY + batHalfSize}
                };

                for (const auto& point : checkPoints) {
                    if (!isPositionWalkable(point.first, point.second)) {
                        canMove = false;
                        break;
                    }
                }

                if (!canMove) break;
            }

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

            static int moveAttemptCount = 0;
            moveAttemptCount++;
            if (moveAttemptCount % 60 == 0) {
                if (!canMove) {
                    int gridX, gridY;
                    _grid->worldToGrid(newX, newY, gridX, gridY);
                }
            }

            if (!canMove) {
                float stepSize = moveDistance / 3.0f;
                for (int step = 1; step <= 3; step++) {
                    float stepX = directionX * stepSize * static_cast<float>(step);
                    float stepY = directionY * stepSize * static_cast<float>(step);
                    float testX = currentX + stepX;
                    float testY = currentY + stepY;

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
                _accumulatedX += moveX;
                _accumulatedY += moveY;

                int pixelMoveX = static_cast<int>(_accumulatedX);
                int pixelMoveY = static_cast<int>(_accumulatedY);

                if (pixelMoveX != 0 || pixelMoveY != 0) {
                    int oldX = pos->getX();
                    int oldY = pos->getY();
                    pos->setX(oldX + pixelMoveX);
                    pos->setY(oldY + pixelMoveY);

                    _accumulatedX -= static_cast<float>(pixelMoveX);
                    _accumulatedY -= static_cast<float>(pixelMoveY);

                    _stuckTimer = 0.0f;

                    static int updateCount = 0;
                    updateCount++;
                } else {
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
                _stuckTimer += deltaTime;
                if (_stuckTimer > 0.5f) {
                   _stuckTimer = 0.0f;
                    _currentPathIndex++;
                    if (_currentPathIndex >= _currentPath.size()) {
                        chooseNewTarget();
                    }
                }
            }
        } else {
            _currentPathIndex++;
            if (_currentPathIndex >= _currentPath.size()) {
                chooseNewTarget();
            }
        }
    } else {
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

    int currentGridX, currentGridY;
    _grid->worldToGrid(currentX, currentY, currentGridX, currentGridY);

    bool currentWalkable = _grid->isWalkable(currentGridX, currentGridY);
    if (!currentWalkable) {
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

    auto targetGrid = _pathfinder->getRandomWalkablePosition();
    float targetX, targetY;
    _grid->gridToWorld(targetGrid.first, targetGrid.second, targetX, targetY);
    targetX += _cellSize / 2.0f;
    targetY += _cellSize / 2.0f;

    _currentPath = _pathfinder->findPath(currentX, currentY, targetX, targetY);
    _currentPathIndex = 0;

    if (_currentPath.empty() || _currentPath.size() < 2) {
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

GameObject* BatAI::findNearestPlayer(float& distance) const {
    if (!_scene || !_parent) return nullptr;

    Transform* batTransform = _parent->getTransform();
    if (!batTransform) return nullptr;

    Position* batPos = batTransform->getPosition();
    if (!batPos) return nullptr;

    float batX = static_cast<float>(batPos->getX());
    float batY = static_cast<float>(batPos->getY());

    GameObject* nearestPlayer = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    const auto& objects = _scene->getObjects();
    for (const auto& obj : objects) {
        if (obj.get() == _parent) continue;

        BaseCharacter* character = dynamic_cast<BaseCharacter*>(obj.get());
        if (!character) continue;

        Transform* playerTransform = obj->getTransform();
        if (!playerTransform) continue;

        Position* playerPos = playerTransform->getPosition();
        if (!playerPos) continue;

        float playerX = static_cast<float>(playerPos->getX());
        float playerY = static_cast<float>(playerPos->getY());

        float dx = playerX - batX;
        float dy = playerY - batY;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist < nearestDistance) {
            nearestDistance = dist;
            nearestPlayer = obj.get();
        }
    }

    distance = nearestDistance;
    return nearestPlayer;
}

void BatAI::chooseFleeTarget(float playerX, float playerY) {
    if (!_parent || !_grid || !_pathfinder) return;

    Transform* transform = _parent->getTransform();
    if (!transform) return;

    Position* pos = transform->getPosition();
    if (!pos) return;

    float currentX = static_cast<float>(pos->getX());
    float currentY = static_cast<float>(pos->getY());

    int currentGridX, currentGridY;
    _grid->worldToGrid(currentX, currentY, currentGridX, currentGridY);

    float dx = currentX - playerX;
    float dy = currentY - playerY;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < 0.001f) {
        std::uniform_int_distribution<int> dirDist(0, 1);
        dx = (dirDist(_rng) == 0) ? 1.0f : -1.0f;
        dy = (dirDist(_rng) == 0) ? 1.0f : -1.0f;
        distance = std::sqrt(dx * dx + dy * dy);
    }

    dx /= distance;
    dy /= distance;

    std::vector<std::pair<float, float>> directions = {
        {dx, dy},
        {-dy, dx},
        {dy, -dx},
        {dx * 0.707f - dy * 0.707f, dx * 0.707f + dy * 0.707f},
        {dx * 0.707f + dy * 0.707f, -dx * 0.707f + dy * 0.707f}
    };

    bool foundPath = false;
    float fleeDistance = _fleeDistance * 2.0f;

    for (const auto& dir : directions) {
        float targetX = currentX + dir.first * fleeDistance;
        float targetY = currentY + dir.second * fleeDistance;

        int targetGridX, targetGridY;
        _grid->worldToGrid(targetX, targetY, targetGridX, targetGridY);

        targetGridX = std::max(0, std::min(targetGridX, _grid->getWidth() - 1));
        targetGridY = std::max(0, std::min(targetGridY, _grid->getHeight() - 1));

        if (!_grid->isWalkable(targetGridX, targetGridY)) {
            bool foundWalkable = false;
            for (int radius = 1; radius <= 3 && !foundWalkable; radius++) {
                for (int ddx = -radius; ddx <= radius && !foundWalkable; ddx++) {
                    for (int ddy = -radius; ddy <= radius && !foundWalkable; ddy++) {
                        int testX = targetGridX + ddx;
                        int testY = targetGridY + ddy;
                        if (testX >= 0 && testX < _grid->getWidth() &&
                            testY >= 0 && testY < _grid->getHeight() &&
                            _grid->isWalkable(testX, testY)) {
                            targetGridX = testX;
                            targetGridY = testY;
                            foundWalkable = true;
                        }
                    }
                }
            }

            if (!foundWalkable) {
                continue;
            }
        }

        _grid->gridToWorld(targetGridX, targetGridY, targetX, targetY);
        targetX += _cellSize / 2.0f;
        targetY += _cellSize / 2.0f;

        _currentPath = _pathfinder->findPath(currentX, currentY, targetX, targetY);
        _currentPathIndex = 0;

        if (!_currentPath.empty() && _currentPath.size() >= 2) {
            foundPath = true;
            break;
        }
    }

    if (!foundPath) {
        for (int attempt = 0; attempt < 10; attempt++) {
            auto randomPos = _pathfinder->getRandomWalkablePosition();
            float targetX, targetY;
            _grid->gridToWorld(randomPos.first, randomPos.second, targetX, targetY);
            targetX += _cellSize / 2.0f;
            targetY += _cellSize / 2.0f;

            float distToPlayer = std::sqrt((targetX - playerX) * (targetX - playerX) +
                                          (targetY - playerY) * (targetY - playerY));
            if (distToPlayer > _fleeDistance) {
                _currentPath = _pathfinder->findPath(currentX, currentY, targetX, targetY);
                _currentPathIndex = 0;
                if (!_currentPath.empty() && _currentPath.size() >= 2) {
                    foundPath = true;
                    break;
                }
            }
        }
    }

    if (!foundPath) {
        chooseNewTarget();
    }
}

void BatAI::syncToNetwork() {
    if (!_parent || !_eventManager) return;
    if (_objectId == -1) return;

    Transform* transform = _parent->getTransform();
    if (!transform) return;

    Position* pos = transform->getPosition();
    if (!pos) return;

    float x = static_cast<float>(pos->getX());
    float y = static_cast<float>(pos->getY());

    auto event = std::make_shared<BatMoveEvent>(_objectId, x, y);
    _eventManager->broadcast(_objectId, event);
}

void BatAI::setDirection(float directionX, float directionY) {
}

void BatAI::setNetworkPosition(float x, float y) {
    _lastNetworkX = x;
    _lastNetworkY = y;
    _hasNetworkUpdate = true;
}

void BatAI::applyNetworkPosition() {
    if (!_parent) return;

    Transform* transform = _parent->getTransform();
    if (!transform) return;

    Position* pos = transform->getPosition();
    if (!pos) return;

    float currentX = static_cast<float>(pos->getX());
    float currentY = static_cast<float>(pos->getY());

    if (!_hasNetworkUpdate && _lastNetworkX == 0.0f && _lastNetworkY == 0.0f) {
        _lastNetworkX = currentX;
        _lastNetworkY = currentY;
        _previousX = currentX;
        _previousY = currentY;
    }

    float errorX = _lastNetworkX - currentX;
    float errorY = _lastNetworkY - currentY;
    float errorDistance = std::sqrt(errorX * errorX + errorY * errorY);

    float movementX = currentX - _previousX;

    if (std::abs(movementX) > 0.1f) {
        Size* size = transform->getSize();
        if (size) {
            float currentWidth = size->getWidth();
            float absWidth = (currentWidth < 0) ? -currentWidth : currentWidth;

            if (movementX < 0.0f) {
                size->setWidth(-absWidth);
            } else {
                size->setWidth(absWidth);
            }
        }
    }

    if (errorDistance > 50.0f) {
        pos->setX(static_cast<int>(_lastNetworkX));
        pos->setY(static_cast<int>(_lastNetworkY));
    } else if (errorDistance > 3.0f) {
        const float CORRECTION_STRENGTH = 0.5f;
        float newX = currentX + errorX * CORRECTION_STRENGTH;
        float newY = currentY + errorY * CORRECTION_STRENGTH;
        pos->setX(static_cast<int>(newX));
        pos->setY(static_cast<int>(newY));
    } else if (errorDistance > 0.5f) {
        pos->setX(static_cast<int>(_lastNetworkX));
        pos->setY(static_cast<int>(_lastNetworkY));
    }

    if (_hasNetworkUpdate) {
        _hasNetworkUpdate = false;
    }

    _previousX = currentX;
    _previousY = currentY;
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