#ifndef VALIDATIEAPP_GRIDRENDERER_H
#define VALIDATIEAPP_GRIDRENDERER_H

#include "LevelGrid.h"
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
    GridRenderer(LevelGrid* grid, Scene* scene, const std::string& spritePath = "../resources/square.png");
    
    // Render a specific cell as a GameObject
    void renderCell(int x, int y);
    
    // Render all cells of a specific type
    void renderCellsOfType(CellType type);
    
    // Render the entire grid
    void renderGrid();
    
    // Update grid visualization (call when grid changes)
    void updateVisualization();
    
    // Set sprite path for rendering
    void setSpritePath(const std::string& path) { _spritePath = path; }
};

#endif //VALIDATIEAPP_GRIDRENDERER_H

