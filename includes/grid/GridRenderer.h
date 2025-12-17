#ifndef VALIDATIEAPP_GRIDRENDERER_H
#define VALIDATIEAPP_GRIDRENDERER_H

#include "grid/LevelGrid.h"
#include "Scenes/Scene.h"
#include "GameObjects/GameObject.h"
#include <memory>
#include <string>

class GridRenderer {
private:
    LevelGrid* _grid;
    Scene* _scene;
    std::string _spritePath;

public:
    GridRenderer(LevelGrid* grid, Scene* scene, const std::string& spritePath = "resources/sprite2.png");
    void renderCell(int x, int y);
    void renderCellsOfType(CellType type);
    void renderGrid();
    void updateVisualization();
    void setSpritePath(const std::string& path) { _spritePath = path; }
};

#endif //VALIDATIEAPP_GRIDRENDERER_H
