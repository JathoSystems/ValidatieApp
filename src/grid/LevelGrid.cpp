#include "grid/LevelGrid.h"
#include <cmath>

LevelGrid::LevelGrid(int width, int height, int cellSize) 
    : _width(width), _height(height), _cellSize(cellSize) {
    _grid.resize(width, std::vector<CellType>(height, CellType::Empty));
}

void LevelGrid::setCellType(int x, int y, CellType type) {
    if (x >= 0 && x < _width && y >= 0 && y < _height) {
        _grid[x][y] = type;
    }
}

CellType LevelGrid::getCellType(int x, int y) const {
    if (x >= 0 && x < _width && y >= 0 && y < _height) {
        return _grid[x][y];
    }
    return CellType::Empty;
}

bool LevelGrid::isWalkable(int x, int y) const {
    return getCellType(x, y) == CellType::Empty;
}

bool LevelGrid::isWalkableWorld(float worldX, float worldY) const {
    int gridX, gridY;
    worldToGrid(worldX, worldY, gridX, gridY);
    return isWalkable(gridX, gridY);
}

void LevelGrid::worldToGrid(float worldX, float worldY, int& gridX, int& gridY) const {
    gridX = static_cast<int>(std::floor(worldX / _cellSize));
    gridY = static_cast<int>(std::floor(worldY / _cellSize));
}

void LevelGrid::gridToWorld(int gridX, int gridY, float& worldX, float& worldY) const {
    worldX = static_cast<float>(gridX * _cellSize);
    worldY = static_cast<float>(gridY * _cellSize);
}
