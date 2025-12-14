#ifndef VALIDATIEAPP_LEVELGRID_H
#define VALIDATIEAPP_LEVELGRID_H

#include "GridCell.h"
#include <vector>

class LevelGrid {
private:
    int _width;
    int _height;
    std::vector<std::vector<CellType>> _grid;

public:
    LevelGrid(int width, int height);

    void setCellType(int x, int y, CellType type);
    CellType getCellType(int x, int y) const;

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

    const std::vector<std::vector<CellType>>& getGrid() const { return _grid; }
};

#endif //VALIDATIEAPP_LEVELGRID_H
