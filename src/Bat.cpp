#include "Bat.h"
#include "LevelGrid.h"
#include "GameObjects/Transform/Position.h"

Bat::Bat(LevelGrid* grid, int cellSize, float speed)
    : _grid(grid), _cellSize(cellSize), _speed(speed) {
}

void Bat::update(float deltaTime) {
    GameObject::update(deltaTime);
}

