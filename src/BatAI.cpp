#include "BatAI.h"
#include "Bat.h"
#include "LevelGrid.h"
#include "GameObjects/Transform/Position.h"
#include "GameObjects/Transform/Transform.h"
#include "BatSpriteRenderer.h"
#include <cmath>
#include <algorithm>

BatAI::BatAI(Bat* bat, LevelGrid* grid, int cellSize, float speed)
    : _grid(grid), _cellSize(cellSize), _speed(speed),
      _directionX(0.0f), _directionY(0.0f),
      _changeDirectionTimer(0.0f), _changeDirectionInterval(2.0f),
      _rng(std::random_device{}()),
      _directionDist(-1.0f, 1.0f),
      _timerDist(1.0f, 3.0f),
      _directionTypeDist(0, 7) { // 8 directions: 4 cardinal + 4 diagonal

    chooseNewDirection();
}

void BatAI::update(float deltaTime) {
    if (!_parent || !_grid) return;
    
    updateMovement(deltaTime);

    _changeDirectionTimer += deltaTime;
    if (_changeDirectionTimer >= _changeDirectionInterval) {
        chooseNewDirection();
        _changeDirectionTimer = 0.0f;
        _changeDirectionInterval = _timerDist(_rng);
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
    
    // Update sprite direction based on movement direction
    BatSpriteRenderer* spriteRenderer = _parent->getComponent<BatSpriteRenderer>();
    if (spriteRenderer) {
        // Flip sprite based on movement direction
        spriteRenderer->setFlipHorizontal(_directionX < 0.0f);
    }
    
    float currentX = static_cast<float>(pos->getX());
    float currentY = static_cast<float>(pos->getY());
    
    // Calculate new position - CONSTANT MOVEMENT
    float moveDistance = _speed * deltaTime;
    float newX = currentX + (_directionX * moveDistance);
    float newY = currentY + (_directionY * moveDistance);
    
    // Check if we can move to the new position
    bool canMove = canMoveTo(newX, newY);
    
    if (canMove) {
        // Also check the center of the bat's bounding box
        float batWidth = static_cast<float>(transform->getSize()->getWidth());
        float batHeight = static_cast<float>(transform->getSize()->getHeight());
        float centerX = newX + batWidth / 2.0f;
        float centerY = newY + batHeight / 2.0f;
        
        if (isPositionWalkable(centerX, centerY)) {
            // Can move - update position
            pos->setX(static_cast<int>(newX));
            pos->setY(static_cast<int>(newY));
        } else {
            // Hit a wall, immediately choose new direction and try to move
            chooseNewDirection();
            // Try moving in new direction immediately
            newX = currentX + (_directionX * moveDistance);
            newY = currentY + (_directionY * moveDistance);
            if (canMoveTo(newX, newY)) {
                pos->setX(static_cast<int>(newX));
                pos->setY(static_cast<int>(newY));
            }
        }
    } else {
        // Hit a wall, immediately choose new direction and try to move
        chooseNewDirection();
        // Try moving in new direction immediately
        newX = currentX + (_directionX * moveDistance);
        newY = currentY + (_directionY * moveDistance);
        if (canMoveTo(newX, newY)) {
            pos->setX(static_cast<int>(newX));
            pos->setY(static_cast<int>(newY));
        }
    }
}

void BatAI::chooseNewDirection() {
    // Choose a random direction including diagonals
    // 8 possible directions: 4 cardinal (up, down, left, right) + 4 diagonal
    int directionType = _directionTypeDist(_rng);
    
    switch (directionType) {
        case 0: // Right
            _directionX = 1.0f;
            _directionY = 0.0f;
            break;
        case 1: // Left
            _directionX = -1.0f;
            _directionY = 0.0f;
            break;
        case 2: // Up
            _directionX = 0.0f;
            _directionY = -1.0f;
            break;
        case 3: // Down
            _directionX = 0.0f;
            _directionY = 1.0f;
            break;
        case 4: // Up-Right (diagonal)
            _directionX = 0.707f; // 1/sqrt(2) ≈ 0.707
            _directionY = -0.707f;
            break;
        case 5: // Up-Left (diagonal)
            _directionX = -0.707f;
            _directionY = -0.707f;
            break;
        case 6: // Down-Right (diagonal)
            _directionX = 0.707f;
            _directionY = 0.707f;
            break;
        case 7: // Down-Left (diagonal)
            _directionX = -0.707f;
            _directionY = 0.707f;
            break;
        default:
            // Fallback to right
            _directionX = 1.0f;
            _directionY = 0.0f;
            break;
    }
    
    // Normalize to ensure consistent speed (diagonals are already normalized)
    float length = std::sqrt(_directionX * _directionX + _directionY * _directionY);
    if (length > 0.0f) {
        _directionX /= length;
        _directionY /= length;
    }
}

bool BatAI::canMoveTo(float worldX, float worldY) const {
    if (!_grid) return false;
    
    // Check if the position is within grid bounds
    int gridX, gridY;
    _grid->worldToGrid(worldX, worldY, gridX, gridY);
    
    // Check bounds
    if (gridX < 0 || gridX >= _grid->getWidth() || 
        gridY < 0 || gridY >= _grid->getHeight()) {
        return false;
    }
    
    // Check if the cell is walkable (not Ground)
    return _grid->isWalkable(gridX, gridY);
}

bool BatAI::isPositionWalkable(float worldX, float worldY) const {
    return canMoveTo(worldX, worldY);
}

