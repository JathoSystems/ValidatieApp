#include "GridRenderer.h"
#include "Scenes/Camera/Viewport.h"
#include <SDL3/SDL.h>

GridRendererComponent::GridRendererComponent(LevelGrid* grid)
    : _grid(grid) {
}

void GridRendererComponent::render(const std::unique_ptr<Window>& window) {
    if (!_grid || !window) return;
    
    SDL_Renderer* renderer = window->getRenderer();
    if (!renderer) return;
    
    int cellSize = _grid->getCellSize();
    
    // Get viewport offset for camera
    const Viewport* viewport = window->getActiveViewport();
    float offsetX = 0, offsetY = 0;
    if (viewport) {
        Position viewportPos = viewport->getPosition();
        offsetX = viewportPos.getX();
        offsetY = viewportPos.getY();
    }
    
    // Render each ground cell
    for (int x = 0; x < _grid->getWidth(); ++x) {
        for (int y = 0; y < _grid->getHeight(); ++y) {
            if (_grid->getCellType(x, y) == CellType::Ground) {
                SDL_FRect rect;
                rect.x = x * cellSize - offsetX;
                rect.y = y * cellSize - offsetY;
                rect.w = static_cast<float>(cellSize);
                rect.h = static_cast<float>(cellSize);
                
                // Draw a brown/gray rectangle for ground
                SDL_SetRenderDrawColor(renderer, 100, 80, 60, 255);
                SDL_RenderFillRect(renderer, &rect);
                
                // Draw a darker border
                SDL_SetRenderDrawColor(renderer, 60, 50, 40, 255);
                SDL_RenderRect(renderer, &rect);
            }
        }
    }
}

