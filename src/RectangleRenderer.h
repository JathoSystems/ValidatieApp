#pragma once
#include "GameObjects/Component/Component.h"
#include "GameObjects/GameObject.h"
#include <SDL3/SDL.h>

class RectangleRenderer : public Component {
private:
    GameObject* _object;
    int w, h;

public:
    RectangleRenderer(GameObject* object, int width, int height)
        : _object(object), w(width), h(height) {}


    void render(const std::unique_ptr<Window> &window) override {
        SDL_Renderer* renderer = window->getRenderer();
        auto pos = _object->getTransform()->getPosition();

        SDL_FRect rect = {
            static_cast<float>(pos->getX()),
            static_cast<float>(pos->getY()),
            static_cast<float>(w),
            static_cast<float>(h)
        };

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderRect(renderer, &rect);
    }


    void update(float delta) override {
    }
};