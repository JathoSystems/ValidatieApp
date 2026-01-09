#ifndef VUURJONGEN_WATERMEISJE_GAME_DOOR_HPP
#define VUURJONGEN_WATERMEISJE_GAME_DOOR_HPP

#include "characters/BaseCharacter.hpp"
#include "characters/Fireboy.hpp"
#include "characters/Watergirl.hpp"
#include "GameObjects/Component/SpriteRenderer.h"
#include "scenes/LevelScene.hpp"

class Door : public GameObject {
private:
    std::string _color;
    LevelScene *_scene;
    bool _isOccupied;

public:
    explicit Door(LevelScene* scene, int cellSize, int x, int y, std::string color = "red")
        : _color(color), _scene(scene), _isOccupied(false) {

        float half = cellSize * 0.5f;
        float scale = 2.0f;
        auto* pos = getTransform()->getPosition();
        auto* size = getTransform()->getSize();

        pos->setX(x * cellSize + half);
        pos->setY(y * cellSize + cellSize);
        size->setWidth(cellSize * scale);
        size->setHeight(cellSize * scale);
        setLayer(5);


        std::string sprite = color == "red" ?
            "resources/doors/door_red.png" :
            "resources/doors/door_blue.png";

        std::cout << "Loading door sprite: " << sprite << std::endl;
        auto spriteRenderer = std::make_unique<SpriteRenderer>(sprite);
        addComponent(std::move(spriteRenderer));
    }

    // Separate methods to avoid dynamic_cast (which was causing crashes due to vtable corruption)
    void checkCollisionWithFireboy(Fireboy* fireboy) {
        if (!fireboy || _color != "red") return;
        checkCollisionInternal(fireboy);
    }
    
    void checkCollisionWithWatergirl(Watergirl* watergirl) {
        if (!watergirl || _color != "blue") return;
        checkCollisionInternal(watergirl);
    }
    
    bool isOccupied() const { return _isOccupied; }
    std::string getColor() const { return _color; }
    
private:
    void checkCollisionInternal(BaseCharacter* character) {
        if (!character) return;
        
        auto* doorTransform = getTransform();
        if (!doorTransform) return;
        
        auto* doorPos = doorTransform->getPosition();
        auto* doorSize = doorTransform->getSize();
        if (!doorPos || !doorSize) return;
        
        auto* charTransform = character->getTransform();
        if (!charTransform) return;
        
        auto* charPos = charTransform->getPosition();
        auto* charSize = charTransform->getSize();
        if (!charPos || !charSize) return;

        float x1 = doorPos->getX();
        float y1 = doorPos->getY();
        float w1 = doorSize->getWidth();
        float h1 = doorSize->getHeight();

        float x2 = charPos->getX();
        float y2 = charPos->getY();
        float w2 = charSize->getWidth();
        float h2 = charSize->getHeight();

        bool collision = (std::abs(x1 - x2) < (w1 + w2) / 2.0f) &&
                        (std::abs(y1 - y2) < (h1 + h2) / 2.0f);

        if (collision && !_isOccupied) {
            _isOccupied = true;
            _scene->reachedDoor();
            std::cout << _color << " character reached door!" << std::endl;
        }
    }
};

#endif //VUURJONGEN_WATERMEISJE_GAME_DOOR_HPP