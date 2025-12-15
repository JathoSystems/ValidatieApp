#ifndef VALIDATIEAPP_BATSPRITERENDERER_H
#define VALIDATIEAPP_BATSPRITERENDERER_H

#include "GameObjects/Component/Component.h"
#include "GameObjects/Texture.h"
#include "SDL/Window.h"
#include <memory>
#include <string>

class BatSpriteRenderer : public Component {
private:
    std::unique_ptr<Texture> _texture;
    bool _flipHorizontal;

public:
    BatSpriteRenderer(const std::string& path);
    
    void setFlipHorizontal(bool flip) { _flipHorizontal = flip; }
    bool getFlipHorizontal() const { return _flipHorizontal; }
    
    void update(float delta) override;
    void render(const std::unique_ptr<Window>& window) override;
};

#endif //VALIDATIEAPP_BATSPRITERENDERER_H

