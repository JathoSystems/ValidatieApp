#ifndef VALIDATIEAPP_BAT_H
#define VALIDATIEAPP_BAT_H

#include "GameObjects/GameObject.h"
#include "GameObjects/Broadcastable.h"
#include <memory>

class LevelGrid;

class Bat : public GameObject, public Broadcastable {
private:
    LevelGrid* _grid;
    float _speed;
    int _cellSize;

public:
    // Constructor that generates a new ID
    Bat(LevelGrid* grid, int cellSize, float speed = 50.0f);
    // Constructor with explicit ID
    Bat(LevelGrid* grid, int cellSize, float speed, int id);
    
    void update(float deltaTime) override;
    
    LevelGrid* getGrid() const { return _grid; }
    float getSpeed() const { return _speed; }
    int getCellSize() const { return _cellSize; }
    void setSpeed(float speed) { _speed = speed; }
};

#endif //VALIDATIEAPP_BAT_H
