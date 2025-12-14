#ifndef VALIDATIEAPP_LEVELGRID_H
#define VALIDATIEAPP_LEVELGRID_H

#include "GridCell.h"
#include <vector>

class LevelGrid {
private:
    int _width;
    int _height;
    int _cellSize;
    std::vector<std::vector<CellType>> _grid;

public:
    LevelGrid(int width, int height, int cellSize = 10);

    void setCellType(int x, int y, CellType type);
    CellType getCellType(int x, int y) const;

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    int getCellSize() const { return _cellSize; }
    void setCellSize(int cellSize) { _cellSize = cellSize; }

    const std::vector<std::vector<CellType>>& getGrid() const { return _grid; }
    
    // Pathfinding helper methods
    bool isWalkable(int x, int y) const;
    bool isWalkableWorld(float worldX, float worldY) const;
    
    // Coordinate conversion methods
    void worldToGrid(float worldX, float worldY, int& gridX, int& gridY) const;
    void gridToWorld(int gridX, int gridY, float& worldX, float& worldY) const;
};

#endif //VALIDATIEAPP_LEVELGRID_H
