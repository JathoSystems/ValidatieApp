#include "bat/BatAnimator.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Transform/Transform.h"
#include "GameObjects/Transform/Size.h"

BatAnimator::BatAnimator(const std::string &path, int rows, int columns)
    : _animator(std::make_unique<Animator>(path, rows, columns)), _flipHorizontal(false) {
}

void BatAnimator::update(float deltaTime) {
    if (_animator && _parent) {
        _animator->setParent(_parent);
        _animator->update(deltaTime);

        if (_parent->getTransform()) {
            Size *size = _parent->getTransform()->getSize();
            if (size) {
                float currentWidth = size->getWidth();
                float absWidth = (currentWidth < 0) ? -currentWidth : currentWidth;

                if (_flipHorizontal) {
                    size->setWidth(-absWidth);
                } else {
                    size->setWidth(absWidth);
                }
            }
        }
    }
}

void BatAnimator::render(const std::unique_ptr<Window> &window) {
    if (_animator && _parent) {
        _animator->setParent(_parent);
        _animator->render(window);
    }
}
