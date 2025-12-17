//
// Created by jusra on 8-12-2025.
//

#include "characters/input/MovementInputHandler.hpp"

MovementInputHandler::MovementInputHandler(Key up, Key down, Key left, Key right)
    : keyUp(up), keyDown(down), keyLeft(left), keyRight(right) {
}

void MovementInputHandler::onKeyPress(Key key) {
    if (key == keyUp) moveUp = true;
    if (key == keyDown) moveDown = true;
    if (key == keyLeft) moveLeft = true;
    if (key == keyRight) moveRight = true;
}

void MovementInputHandler::onKeyRelease(Key key) {
    if (key == keyUp) moveUp = false;
    if (key == keyDown) moveDown = false;
    if (key == keyLeft) moveLeft = false;
    if (key == keyRight) moveRight = false;
}
