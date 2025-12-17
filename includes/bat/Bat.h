#ifndef VALIDATIEAPP_BAT_H
#define VALIDATIEAPP_BAT_H

#include "GameObjects/GameObject.h"
#include <memory>

class LevelGrid;

class Bat : public GameObject {
private:
    LevelGrid* _grid;
    float _speed;
    int _cellSize;

public:
    Bat(LevelGrid* grid, int cellSize, float speed = 50.0f);
    
    void update(float deltaTime) override;
    
    LevelGrid* getGrid() const { return _grid; }
    float getSpeed() const { return _speed; }
    int getCellSize() const { return _cellSize; }
    void setSpeed(float speed) { _speed = speed; }
};

#endif //VALIDATIEAPP_BAT_H
