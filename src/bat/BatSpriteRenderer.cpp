#include "bat/BatSpriteRenderer.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Transform/Transform.h"
#include "Scenes/Camera/Viewport.h"
#include "SDL3/SDL_render.h"
#include <iostream>

BatSpriteRenderer::BatSpriteRenderer(const std::string& path)
    : _texture(std::make_unique<Texture>(path)), _flipHorizontal(false) {
}

void BatSpriteRenderer::update(float delta) {
    if (!_parent || !_texture) return;
    
    Transform* transform = _parent->getTransform();
    if (!transform) return;
    
    Size* size = transform->getSize();
    
    if (size->getHeight() == 0)
        size->setHeight(_texture->getHeight());
    
    if (size->getWidth() == 0)
        size->setWidth(_texture->getWidth());
    
    _texture->transform(transform);
}

void BatSpriteRenderer::render(const std::unique_ptr<Window>& window) {
    if (!_parent || !_texture || !window) return;
    
    SDL_Renderer* renderer = window->getRenderer();
    if (!renderer) return;
    
    const Viewport* viewport = window->getActiveViewport();
    if (!viewport) {
        std::cout << "ViewPort null" << std::endl;
        return;
    }
    
    SDL_Texture* sdlTexture = _texture->getTexture(window.get());
    if (!sdlTexture) return;
    
    Transform* transform = _parent->getTransform();
    if (!transform) return;
    
    Position* pos = transform->getPosition();
    Size* size = transform->getSize();
    if (!pos || !size) return;
    
    // Calculate screen position with viewport offset
    Position viewportPos = viewport->getPosition();
    float screenX = static_cast<float>(pos->getX()) - viewportPos.getX();
    float screenY = static_cast<float>(pos->getY()) - viewportPos.getY();
    
    float width = static_cast<float>(size->getWidth());
    float height = static_cast<float>(size->getHeight());
    
    SDL_FRect dstRect;
    
    // If flipped, adjust the destination rect (negative width flips horizontally)
    if (_flipHorizontal) {
        // Flip by using negative width and adjusting x position
        dstRect.x = screenX + width / 2.0f;
        dstRect.y = screenY - height / 2.0f;
        dstRect.w = -width;
        dstRect.h = height;
    } else {
        // Normal rendering
        dstRect.x = screenX - width / 2.0f;
        dstRect.y = screenY - height / 2.0f;
        dstRect.w = width;
        dstRect.h = height;
    }
    
    SDL_RenderTexture(renderer, sdlTexture, nullptr, &dstRect);
}
