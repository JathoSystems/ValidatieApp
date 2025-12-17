#include "GridRenderer.h"
#include "GameObjects/Component/SpriteRenderer.h"
#include "Physics/PhysicsComponent.h"

GridRenderer::GridRenderer(LevelGrid* grid, Scene* scene, Box2DFacade* box2DFacade, const std::string& spritePath)
    : _grid(grid), _scene(scene), _box2DFacade(box2DFacade), _spritePath(spritePath) {
}

std::string GridRenderer::getSpritePathForCellType(CellType type) {
    switch (type) {
        case CellType::Ground:
            return "resources/tile.png";
        case CellType::Water:
            return "resources/fluids/water/water_middle.jpg";
        case CellType::WaterRight:
            return "resources/fluids/water/water_right.jpg";
        case CellType::WaterLeft:
            return "resources/fluids/water/water_left.jpg";
        case CellType::Fire:
            return "resources/fluids/fire/fire_middle.jpg";
        case CellType::FireLeft:
            return "resources/fluids/fire/fire_left.jpg";
        case CellType::FireRight:
            return "resources/fluids/fire/fire_right.jpg";
        case CellType::Poison:
            return "resources/fluids/poison/poison_middle.jpg";
        case CellType::PoisonLeft:
            return "resources/fluids/poison/poison_left.jpg";
        case CellType::PoisonRight:
            return "resources/fluids/poison/poison_right.jpg";
        case CellType::RedDoor:
            return "resources/doors/door_red.png";
        case CellType::BlueDoor:
            return "resources/doors/door_blue.png";
        case CellType::Button:
            return "resources/button.png";
        case CellType::DiamondBlue:
            return "resources/diamonds/diamond_blue.png";
        case CellType::DiamondRed:
            return "resources/diamonds/diamond_red.png";
        case CellType::SlopeLeft:
            return "resources/slope_left.png";
        case CellType::SlopeRight:
            return "resources/slope_right.png";
        default:
            return "resources/tile.png";
    }
}

bool GridRenderer::isCollidable(CellType type) {
    return type == CellType::Ground;
}

void GridRenderer::renderCell(int x, int y) {
    if (!_grid || !_scene || !_box2DFacade) return;

    CellType cellType = _grid->getCellType(x, y);
    if (cellType == CellType::Empty) return;

    int cellSize = _grid->getCellSize();

    // Check if this is a door
    bool isDoor = (cellType == CellType::RedDoor || cellType == CellType::BlueDoor);

    // For doors, only render if this is the top-left corner
    if (isDoor) {
        if (!isTopLeftOfDoor(x, y, cellType)) {
            return;
        }
        renderDoorLayers(x, y, cellType);
        return;
    }

    // Check if this is a diamond - only render at top-left of 2x2 block
    bool isDiamond = (cellType == CellType::DiamondBlue || cellType == CellType::DiamondRed);
    if (isDiamond) {
        if (!isTopLeftOfDiamond(x, y, cellType)) {
            return; // Skip rendering, this is part of a diamond but not the anchor point
        }
    }

    int spriteWidth = isDiamond ? cellSize * 2 : cellSize;
    int spriteHeight = isDiamond ? cellSize * 2 : cellSize;

    int collisionWidth = isDiamond ? cellSize * 2 : cellSize;
    int collisionHeight = isDiamond ? cellSize * 2 : cellSize;

    auto block = std::make_unique<GameObject>();

    // Center within the 2x2 block for diamonds
    float xPos = x * cellSize + spriteWidth / 2;
    float yPos = y * cellSize + spriteHeight / 2;

    block->getTransform()->getPosition()->setX(xPos);
    block->getTransform()->getPosition()->setY(yPos);
    block->getTransform()->getSize()->setWidth(spriteWidth);
    block->getTransform()->getSize()->setHeight(spriteHeight);

    std::string spritePath = getSpritePathForCellType(cellType);
    auto spriteRenderer = std::make_unique<SpriteRenderer>(spritePath);
    block->addComponent(std::move(spriteRenderer));

    auto physics = std::make_unique<PhysicsComponent>(_box2DFacade);

    if (isCollidable(cellType)) {
        physics->setBodyType(BodyType::STATIC);
        physics->setCollider(std::make_unique<BoxCollider>(cellSize, cellSize));
    } else {
        physics->setBodyType(BodyType::STATIC);
        physics->setCollider(std::make_unique<BoxCollider>(collisionWidth, collisionHeight));
    }

    block->addComponent(std::move(physics));
    _scene->addObject(std::move(block));
}

void GridRenderer::renderDoorLayers(int x, int y, CellType doorType) {
    int cellSize = _grid->getCellSize();

    // Actual image dimensions
    int baseWidth = 53;
    int baseHeight = 57;
    int doorWidth = 41;
    int doorHeight = 52;

    int blockWidthPx = 6 * cellSize;
    int blockHeightPx = 6 * cellSize;

    float baseCenterX = x * cellSize + blockWidthPx / 2.0f;
    float baseCenterY = y * cellSize + blockHeightPx / 2.0f + 5.0f;

    // Layer 1: Door base/frame (behind)
    auto doorBase = std::make_unique<GameObject>();
    doorBase->getTransform()->getPosition()->setX(baseCenterX);
    doorBase->getTransform()->getPosition()->setY(baseCenterY);
    doorBase->getTransform()->getSize()->setWidth(baseWidth);
    doorBase->getTransform()->getSize()->setHeight(baseHeight);

    std::string basePath = "resources/doors/door_base.png";

    auto baseSprite = std::make_unique<SpriteRenderer>(basePath);
    doorBase->addComponent(std::move(baseSprite));
    doorBase->setLayer(0);

    _scene->addObject(std::move(doorBase));

    // Layer 2: Door itself (in front, centered within the base)
    auto doorFront = std::make_unique<GameObject>();
    doorFront->getTransform()->getPosition()->setX(baseCenterX);  // Same center as base
    doorFront->getTransform()->getPosition()->setY(baseCenterY);  // Same center as base
    doorFront->getTransform()->getSize()->setWidth(doorWidth);
    doorFront->getTransform()->getSize()->setHeight(doorHeight);

    std::string doorPath = (doorType == CellType::RedDoor)
        ? "resources/doors/door_red.png"
        : "resources/doors/door_blue.png";

    auto doorSprite = std::make_unique<SpriteRenderer>(doorPath);
    doorFront->addComponent(std::move(doorSprite));
    doorFront->setLayer(1);

    // Add collision to match the base size (full door frame)
    auto physics = std::make_unique<PhysicsComponent>(_box2DFacade);
    physics->setBodyType(BodyType::STATIC);
    physics->setCollider(std::make_unique<BoxCollider>(baseWidth, baseHeight));
    doorFront->addComponent(std::move(physics));

    _scene->addObject(std::move(doorFront));
}

bool GridRenderer::isTopLeftOfDoor(int x, int y, CellType doorType) {
    // Base is 53 wide, 57 tall
    // At 10px per cell: 6 cells wide (rounding up from 5.3), 6 cells tall (rounding up from 5.7)
    const int doorWidthCells = 6;   // 53px / 10px per cell = 5.3, round up
    const int doorHeightCells = 6;  // 57px / 10px per cell = 5.7, round up

    for (int dy = 0; dy < doorHeightCells; ++dy) {
        for (int dx = 0; dx < doorWidthCells; ++dx) {
            if (_grid->getCellType(x + dx, y + dy) != doorType) {
                return false;
            }
        }
    }

    return true;
}

bool GridRenderer::isTopLeftOfDiamond(int x, int y, CellType diamondType) {
    // Check if there's a 2x2 block of the same diamond type
    const int diamondSize = 2;

    for (int dy = 0; dy < diamondSize; ++dy) {
        for (int dx = 0; dx < diamondSize; ++dx) {
            if (_grid->getCellType(x + dx, y + dy) != diamondType) {
                return false;
            }
        }
    }

    return true;
}

std::string GridRenderer::getCellType(CellType type) {
    switch (type) {
        case CellType::Water: return "Water";
        case CellType::WaterLeft: return "Water";
        case CellType::WaterRight: return "Water";

        case CellType::Fire: return "Fire";
        case CellType::FireLeft: return "Fire";
        case CellType::FireRight: return "Fire";

        case CellType::Poison: return "Poison";
        case CellType::PoisonLeft: return "Poison";
        case CellType::PoisonRight: return "Poison";

        case CellType::RedDoor: return "RedDoor";
        case CellType::BlueDoor: return "BlueDoor";
        case CellType::Button: return "Button";
        case CellType::DiamondBlue: return "DiamondBlue";
        case CellType::DiamondRed: return "DiamondRed";
        case CellType::Ground: return "Ground";
        default: return "Unknown";
    }
}

void GridRenderer::renderCellsOfType(CellType type) {
    if (!_grid || !_scene) return;

    for (int x = 0; x < _grid->getWidth(); ++x) {
        for (int y = 0; y < _grid->getHeight(); ++y) {
            if (_grid->getCellType(x, y) == type) {
                renderCell(x, y);
            }
        }
    }
}

void GridRenderer::renderGrid() {
    if (!_grid || !_scene) return;

    // Render all non-empty cells
    for (int x = 0; x < _grid->getWidth(); ++x) {
        for (int y = 0; y < _grid->getHeight(); ++y) {
            renderCell(x, y);
        }
    }
}

void GridRenderer::updateVisualization() {
    // This can be extended to update existing GameObjects instead of recreating them
    // For now, we'll just re-render the grid
    renderGrid();
}