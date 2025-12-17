#ifndef VALIDATIEAPP_GRIDRENDERER_H
#define VALIDATIEAPP_GRIDRENDERER_H

#include "LevelGrid.h"
#include "Scenes/Scene.h"
#include "GameObjects/GameObject.h"
#include "Physics/Box2DFacade.h"
#include <memory>
#include <string>

class GridRenderer {
private:
    LevelGrid* _grid;
    Scene* _scene;
    Box2DFacade* _box2DFacade;
    std::string _spritePath;

    // Helper methods
    std::string getSpritePathForCellType(CellType type);
    bool isCollidable(CellType type);
    std::string getCellType(CellType type);

public:
    GridRenderer(LevelGrid* grid, Scene* scene, Box2DFacade* box2DFacade, const std::string& spritePath = "resources/tile.png");

    // Render a specific cell as a GameObject
    void renderCell(int x, int y);

    void renderDoorLayers(int x, int y, CellType doorType);

    bool isTopLeftOfDoor(int x, int y, CellType doorType);

    bool isTopLeftOfDiamond(int x, int y, CellType diamondType);

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