#ifndef VALIDATIEAPP_BATANIMATOR_H
#define VALIDATIEAPP_BATANIMATOR_H

#include "GameObjects/Component/Component.h"
#include "GameObjects/Spritesheet/Animator.h"
#include <memory>

class BatAnimator : public Component {
private:
    std::unique_ptr<Animator> _animator;
    bool _flipHorizontal;

public:
    BatAnimator(const std::string &path, int rows, int columns);

    void setFlipHorizontal(bool flip) { _flipHorizontal = flip; }
    bool getFlipHorizontal() const { return _flipHorizontal; }

    void update(float deltaTime) override;

    void render(const std::unique_ptr<Window> &window) override;
};

#endif //VALIDATIEAPP_BATANIMATOR_H
