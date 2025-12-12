// LevelBuilder.cpp
#include "LevelBuilder.h"

#include "RectangleRenderer.h" // Include the new component

LevelBuilder::LevelBuilder(Window * window) : _window(window) {
    grid = std::make_unique<GameObject>();
    SDL_GetWindowSizeInPixels(window->getWindow(), &windowW, &windowH);

    for (int i = 0; i < windowW; i += gridBlockW) {
        for (int j = 0; j < windowH; j += gridBlockH) {
            
            auto block = std::make_unique<GameObject>();
            block->getTransform()->getPosition()->setX(i);
            block->getTransform()->getPosition()->setY(j);

            block->addComponent(std::make_unique<RectangleRenderer>(block.get(), gridBlockW, gridBlockH));

            grid->addChild(std::move(block));
        }
    }
}