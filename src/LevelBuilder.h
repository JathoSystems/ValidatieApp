#pragma once

#include <string>
#include <vector>
#include "LevelGrid.h"
#include "GridCell.h"

class LevelBuilder {
public:
    // '.' = Empty
    // '#' = Ground
    // 'W' = Water
    // '[' = Water left corner
    // ']' = Water right corner
    // 'F' = Fire/Lava
    // '{' = Fire right corner
    // '}' = Fire right corner
    // 'P' = Poison (deadly for both)
    // '<' = Poison right corner
    // '>' = Poison right corner
    // 'R' = Red Door (Fireboy exit)
    // 'B' = Blue Door (Watergirl exit)
    // 'S' = Button/Switch
    // '$' = Diamond Blue
    // '%' = Diamond Red

    static void buildFromString(LevelGrid* grid, const std::vector<std::string>& levelMap) {
        if (!grid) return;

        int height = levelMap.size();

        for (int y = 0; y < height && y < grid->getHeight(); ++y) {
            const std::string& row = levelMap[y];
            int width = row.length();

            for (int x = 0; x < width && x < grid->getWidth(); ++x) {
                char cell = row[x];

                switch (cell) {
                    case '#':
                        grid->setCellType(x, y, CellType::Ground);
                        break;
                    case 'W':
                        grid->setCellType(x, y, CellType::Water);
                        break;
                    case ']':
                        grid->setCellType(x, y, CellType::WaterRight);
                        break;
                    case '[':
                        grid->setCellType(x, y, CellType::WaterLeft);
                        break;
                    case 'F':
                        grid->setCellType(x, y, CellType::Fire);
                        break;
                    case '}':
                        grid->setCellType(x, y, CellType::FireRight);
                        break;
                    case '{':
                        grid->setCellType(x, y, CellType::FireLeft);
                        break;
                    case 'P':
                        grid->setCellType(x, y, CellType::Poison);
                        break;
                    case '>':
                        grid->setCellType(x, y, CellType::PoisonRight);
                        break;
                    case '<':
                        grid->setCellType(x, y, CellType::PoisonLeft);
                        break;
                    case 'R':
                        grid->setCellType(x, y, CellType::RedDoor);
                        break;
                    case 'B':
                        grid->setCellType(x, y, CellType::BlueDoor);
                        break;
                    case 'S':
                        grid->setCellType(x, y, CellType::Button);
                        break;
                    case '$':
                        grid->setCellType(x, y, CellType::DiamondBlue);
                        break;
                    case '%':
                        grid->setCellType(x, y, CellType::DiamondRed);
                        break;
                    case '.':
                    case ' ':
                    default:
                        grid->setCellType(x, y, CellType::Empty);
                        break;
                }
            }
        }
    }

    // Helper to create a level with borders
    static void buildWithBorders(LevelGrid* grid, int borderThickness = 5) {
        if (!grid) return;

        // Top border
        for (int x = 0; x < grid->getWidth(); ++x) {
            for (int y = 0; y < borderThickness; ++y) {
                grid->setCellType(x, y, CellType::Ground);
            }
        }

        // Bottom border
        for (int x = 0; x < grid->getWidth(); ++x) {
            for (int y = grid->getHeight() - borderThickness; y < grid->getHeight(); ++y) {
                grid->setCellType(x, y, CellType::Ground);
            }
        }

        // Left border
        for (int x = 0; x < borderThickness; ++x) {
            for (int y = 0; y < grid->getHeight(); ++y) {
                grid->setCellType(x, y, CellType::Ground);
            }
        }

        // Right border
        for (int x = grid->getWidth() - borderThickness; x < grid->getWidth(); ++x) {
            for (int y = 0; y < grid->getHeight(); ++y) {
                grid->setCellType(x, y, CellType::Ground);
            }
        }
    }

    // Create a rectangular area of any type
    static void buildPlatform(LevelGrid* grid, int startX, int startY, int width, int height, CellType type = CellType::Ground) {
        if (!grid) return;

        for (int x = startX; x < startX + width && x < grid->getWidth(); ++x) {
            for (int y = startY; y < startY + height && y < grid->getHeight(); ++y) {
                grid->setCellType(x, y, type);
            }
        }
    }

    // Create stairs
    static void buildStairs(LevelGrid* grid, int startX, int startY, int steps, int stepWidth, int stepHeight, bool ascending = true) {
        if (!grid) return;

        for (int i = 0; i < steps; i++) {
            int x = startX + (i * stepWidth);
            int y = ascending ? startY - (i * stepHeight) : startY + (i * stepHeight);

            buildPlatform(grid, x, y, stepWidth, stepHeight);
        }
    }

    // Create a pool of liquid
    static void buildPool(LevelGrid* grid, int startX, int startY, int width, int depth, CellType liquidType) {
        buildPlatform(grid, startX, startY, width, depth, liquidType);
    }
};