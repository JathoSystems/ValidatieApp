#ifndef VALIDATIEAPP_BATAI_H
#define VALIDATIEAPP_BATAI_H

#include "GameObjects/Component/Component.h"
#include "LevelGrid.h"
#include <random>
#include <memory>

class Bat;

class BatAI : public Component {
private:
    LevelGrid* _grid;
    int _cellSize;
    float _speed;

    float _directionX;
    float _directionY;
    float _changeDirectionTimer;
    float _changeDirectionInterval;
    
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _directionDist;
    std::uniform_real_distribution<float> _timerDist;
    std::uniform_int_distribution<int> _directionTypeDist; // For choosing direction type (cardinal vs diagonal)

public:
    BatAI(Bat* bat, LevelGrid* grid, int cellSize, float speed = 50.0f);
    
    void update(float deltaTime) override;
    void render(const std::unique_ptr<Window>& window) override;
    
    void setSpeed(float speed) { _speed = speed; }
    
    // Get current direction for sprite flipping
    float getDirectionX() const { return _directionX; }

private:
    void updateMovement(float deltaTime);
    void chooseNewDirection();
    bool canMoveTo(float worldX, float worldY) const;
    bool isPositionWalkable(float worldX, float worldY) const;
};

#endif //VALIDATIEAPP_BATAI_H

