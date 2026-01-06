#ifndef VALIDATIEAPP_ASTARPATHFINDER_H
#define VALIDATIEAPP_ASTARPATHFINDER_H

#include "grid/LevelGrid.h"
#include <vector>
#include <memory>

struct GridNode {
    int x, y;
    float gCost;  // Cost from start
    float hCost;  // Heuristic cost to end
    float fCost() const { return gCost + hCost; }
    GridNode* parent;
    
    GridNode(int x, int y) : x(x), y(y), gCost(0), hCost(0), parent(nullptr) {}
    
    bool operator==(const GridNode& other) const {
        return x == other.x && y == other.y;
    }
};

class AStarPathfinder {
private:
    LevelGrid* _grid;
    
    float heuristic(int x1, int y1, int x2, int y2) const;
    std::vector<GridNode> getNeighbors(const GridNode& node) const;
    GridNode* findNodeInList(std::vector<GridNode>& list, int x, int y) const;
    bool isValidNode(int x, int y) const;

public:
    AStarPathfinder(LevelGrid* grid);
    
    // Find path from world coordinates to world coordinates
    std::vector<std::pair<float, float>> findPath(float startX, float startY, float endX, float endY);
    
    // Find path from grid coordinates to grid coordinates
    std::vector<std::pair<int, int>> findPathGrid(int startX, int startY, int endX, int endY);
    
    // Get a random walkable position on the grid
    std::pair<int, int> getRandomWalkablePosition() const;
};

#endif //VALIDATIEAPP_ASTARPATHFINDER_H

