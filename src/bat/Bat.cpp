#include "bat/Bat.h"

Bat::Bat(LevelGrid *grid, int cellSize, float speed)
    : Broadcastable(this), _grid(grid), _cellSize(cellSize), _speed(speed) {
}

Bat::Bat(LevelGrid *grid, int cellSize, float speed, int id)
    : Broadcastable(this, id), _grid(grid), _cellSize(cellSize), _speed(speed) {
}

void Bat::update(float deltaTime) {
    GameObject::update(deltaTime);
}
