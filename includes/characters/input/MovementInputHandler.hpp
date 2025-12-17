//
// Created by jusra on 8-12-2025.
//

#ifndef VUURJONGEN_WATERMEISJE_MOVEMENTINPUTHANDLER_HPP
#define VUURJONGEN_WATERMEISJE_MOVEMENTINPUTHANDLER_HPP
#include "Input/IKeyListener.h"

class MovementInputHandler : public IKeyListener {
public:
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;

private:
    Key keyUp;
    Key keyDown;
    Key keyLeft;
    Key keyRight;

public:
    MovementInputHandler(Key up, Key down, Key left, Key right);

    void onKeyPress(Key key) override;

    void onKeyRelease(Key key) override;
};

#endif //VUURJONGEN_WATERMEISJE_MOVEMENTINPUTHANDLER_HPP