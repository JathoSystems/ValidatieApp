#include "LevelGrid.h"

LevelGrid::LevelGrid(int width, int height) 
    : _width(width), _height(height) {
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
