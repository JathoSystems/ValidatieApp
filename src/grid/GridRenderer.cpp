#include "grid/GridRenderer.h"
#include "GameObjects/Component/SpriteRenderer.h"

GridRenderer::GridRenderer(LevelGrid* grid, Scene* scene, const std::string& spritePath)
    : _grid(grid), _scene(scene), _spritePath(spritePath) {
}

void GridRenderer::renderCell(int x, int y) {
    if (!_grid || !_scene) return;
    
    CellType cellType = _grid->getCellType(x, y);
    if (cellType == CellType::Empty) return; // Don't render empty cells
    
    int cellSize = _grid->getCellSize();
    
    auto block = std::make_unique<GameObject>();
    block->getTransform()->getPosition()->setX(x * cellSize);
    block->getTransform()->getPosition()->setY(y * cellSize);
    block->getTransform()->getSize()->setWidth(cellSize);
    block->getTransform()->getSize()->setHeight(cellSize);
    
    // Add SpriteRenderer so the block is visible
    auto spriteRenderer = std::make_unique<SpriteRenderer>(_spritePath);
    block->addComponent(std::move(spriteRenderer));
    
    _scene->addObject(std::move(block));
}

void GridRenderer::renderCellsOfType(CellType type) {
    if (!_grid || !_scene) return;
    
    for (int x = 0; x < _grid->getWidth(); ++x) {
        for (int y = 0; y < _grid->getHeight(); ++y) {
            if (_grid->getCellType(x, y) == type) {
                renderCell(x, y);
            }
        }
    }
}

void GridRenderer::renderGrid() {
    renderCellsOfType(CellType::Ground);
}

void GridRenderer::updateVisualization() {
    // This can be extended to update existing GameObjects instead of recreating them
    // For now, we'll just re-render the grid
    renderGrid();
}
