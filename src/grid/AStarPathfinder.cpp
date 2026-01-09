#include "grid/AStarPathfinder.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <limits>
#include <map>
#include <set>
#include <iostream>

AStarPathfinder::AStarPathfinder(LevelGrid* grid) : _grid(grid) {
}

float AStarPathfinder::heuristic(int x1, int y1, int x2, int y2) const {
    // Euclidean distance
    int dx = x2 - x1;
    int dy = y2 - y1;
    return std::sqrt(static_cast<float>(dx * dx + dy * dy));
}

std::vector<GridNode> AStarPathfinder::getNeighbors(const GridNode& node) const {
    std::vector<GridNode> neighbors;
    
    // 8-directional movement (including diagonals)
    int directions[8][2] = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0},  // Cardinal directions
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // Diagonal directions
    };
    
    for (int i = 0; i < 8; i++) {
        int newX = node.x + directions[i][0];
        int newY = node.y + directions[i][1];
        
        if (isValidNode(newX, newY)) {
            neighbors.emplace_back(newX, newY);
        }
    }
    
    return neighbors;
}

GridNode* AStarPathfinder::findNodeInList(std::vector<GridNode>& list, int x, int y) const {
    for (auto& node : list) {
        if (node.x == x && node.y == y) {
            return &node;
        }
    }
    return nullptr;
}

bool AStarPathfinder::isValidNode(int x, int y) const {
    if (!_grid) return false;
    if (x < 0 || x >= _grid->getWidth() || y < 0 || y >= _grid->getHeight()) {
        return false;
    }
    return _grid->isWalkable(x, y);
}

std::vector<std::pair<float, float>> AStarPathfinder::findPath(float startX, float startY, float endX, float endY) {
    if (!_grid) return {};
    
    int startGridX, startGridY, endGridX, endGridY;
    _grid->worldToGrid(startX, startY, startGridX, startGridY);
    _grid->worldToGrid(endX, endY, endGridX, endGridY);
    
    auto gridPath = findPathGrid(startGridX, startGridY, endGridX, endGridY);
    
    if (gridPath.empty()) {
        return {};
    }
    
    std::vector<std::pair<float, float>> worldPath;
    
    // Convert grid path to world coordinates
    for (size_t i = 0; i < gridPath.size(); i++) {
        float worldX, worldY;
        _grid->gridToWorld(gridPath[i].first, gridPath[i].second, worldX, worldY);
        // Center the position in the cell
        worldX += _grid->getCellSize() / 2.0f;
        worldY += _grid->getCellSize() / 2.0f;
        worldPath.emplace_back(worldX, worldY);
    }
    
    // Add intermediate waypoints for smoother movement around obstacles
    // This helps the bat avoid flying through blocks between waypoints
    std::vector<std::pair<float, float>> smoothedPath;
    if (!worldPath.empty()) {
        smoothedPath.push_back(worldPath[0]);
        
        for (size_t i = 1; i < worldPath.size(); i++) {
            float prevX = worldPath[i - 1].first;
            float prevY = worldPath[i - 1].second;
            float currX = worldPath[i].first;
            float currY = worldPath[i].second;
            
            float dx = currX - prevX;
            float dy = currY - prevY;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            // If waypoints are far apart, add intermediate points
            int cellSize = _grid->getCellSize();
            if (dist > cellSize * 1.5f) {
                int steps = static_cast<int>(dist / (cellSize * 0.8f));
                for (int j = 1; j < steps; j++) {
                    float t = static_cast<float>(j) / static_cast<float>(steps);
                    float interpX = prevX + dx * t;
                    float interpY = prevY + dy * t;
                    smoothedPath.emplace_back(interpX, interpY);
                }
            }
            
            smoothedPath.push_back(worldPath[i]);
        }
    }
    
    return smoothedPath;
}

std::vector<std::pair<int, int>> AStarPathfinder::findPathGrid(int startX, int startY, int endX, int endY) {
    if (!_grid) {
        std::cout << "[AStarPathfinder] No grid!" << std::endl;
        return {};
    }
    
    // Check if start and end are valid
    bool startValid = isValidNode(startX, startY);
    bool endValid = isValidNode(endX, endY);

    if (!startValid || !endValid) {
        return {};
    }
    
    if (startX == endX && startY == endY) {
        return {{startX, startY}};
    }
    
    std::vector<GridNode> openList;
    std::set<std::pair<int, int>> closedSet;
    std::map<std::pair<int, int>, std::pair<int, int>> parentMap;
    std::map<std::pair<int, int>, float> gCostMap;
    
    GridNode startNode(startX, startY);
    startNode.gCost = 0;
    startNode.hCost = heuristic(startX, startY, endX, endY);
    openList.push_back(startNode);
    gCostMap[{startX, startY}] = 0;
    
    while (!openList.empty()) {
        // Find node with lowest fCost
        auto it = std::min_element(openList.begin(), openList.end(),
            [](const GridNode& a, const GridNode& b) {
                if (std::abs(a.fCost() - b.fCost()) < 0.0001f) {
                    return a.hCost < b.hCost; // Tie-breaker: prefer closer to goal
                }
                return a.fCost() < b.fCost();
            });
        
        GridNode currentNode = *it;
        openList.erase(it);
        closedSet.insert({currentNode.x, currentNode.y});
        
        // Check if we reached the goal
        if (currentNode.x == endX && currentNode.y == endY) {
            // Reconstruct path backwards
            std::vector<std::pair<int, int>> path;
            int currentX = endX;
            int currentY = endY;
            path.emplace_back(currentX, currentY);
            
            while (true) {
                auto parentIt = parentMap.find({currentX, currentY});
                if (parentIt == parentMap.end()) {
                    break;
                }
                currentX = parentIt->second.first;
                currentY = parentIt->second.second;
                path.emplace_back(currentX, currentY);
            }
            
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Check neighbors
        auto neighbors = getNeighbors(currentNode);
        for (auto& neighbor : neighbors) {
            // Skip if already in closed set
            if (closedSet.find({neighbor.x, neighbor.y}) != closedSet.end()) {
                continue;
            }
            
            // Calculate movement cost (diagonal costs more)
            bool isDiagonal = (neighbor.x != currentNode.x && neighbor.y != currentNode.y);
            float moveCost = isDiagonal ? 1.414f : 1.0f;
            float newGCost = currentNode.gCost + moveCost;
            
            GridNode* existingNode = findNodeInList(openList, neighbor.x, neighbor.y);
            if (existingNode == nullptr) {
                // New node, add to open list
                neighbor.gCost = newGCost;
                neighbor.hCost = heuristic(neighbor.x, neighbor.y, endX, endY);
                parentMap[{neighbor.x, neighbor.y}] = {currentNode.x, currentNode.y};
                gCostMap[{neighbor.x, neighbor.y}] = newGCost;
                openList.push_back(neighbor);
            } else if (newGCost < existingNode->gCost) {
                // Better path found, update node
                existingNode->gCost = newGCost;
                parentMap[{neighbor.x, neighbor.y}] = {currentNode.x, currentNode.y};
                gCostMap[{neighbor.x, neighbor.y}] = newGCost;
            }
        }
    }
    
    // No path found - log why
    std::cout << "[AStarPathfinder] No path found from (" << startX << ", " << startY 
              << ") to (" << endX << ", " << endY << ")" << std::endl;
    std::cout << "  Start walkable: " << (_grid->isWalkable(startX, startY) ? "YES" : "NO") << std::endl;
    std::cout << "  End walkable: " << (_grid->isWalkable(endX, endY) ? "YES" : "NO") << std::endl;
    std::cout << "  Open list size when exhausted: " << openList.size() << std::endl;
    std::cout << "  Closed set size: " << closedSet.size() << std::endl;
    
    return {};
}

std::pair<int, int> AStarPathfinder::getRandomWalkablePosition() const {
    if (!_grid) return {0, 0};
    
    // First, collect all walkable positions
    std::vector<std::pair<int, int>> walkablePositions;
    for (int x = 0; x < _grid->getWidth(); x++) {
        for (int y = 0; y < _grid->getHeight(); y++) {
            if (_grid->isWalkable(x, y)) {
                walkablePositions.emplace_back(x, y);
            }
        }
    }
    
    if (walkablePositions.empty()) {
        std::cout << "[AStarPathfinder] WARNING: No walkable positions found in grid!" << std::endl;
        return {0, 0};
    }
    
    // Log walkable area distribution
    if (walkablePositions.size() > 0) {
        int minX = walkablePositions[0].first, maxX = walkablePositions[0].first;
        int minY = walkablePositions[0].second, maxY = walkablePositions[0].second;
        for (const auto& pos : walkablePositions) {
            minX = std::min(minX, pos.first);
            maxX = std::max(maxX, pos.first);
            minY = std::min(minY, pos.second);
            maxY = std::max(maxY, pos.second);
        }
        std::cout << "[AStarPathfinder] Walkable area: Grid (" << minX << ", " << minY 
                  << ") to (" << maxX << ", " << maxY << "), Total walkable cells: " 
                  << walkablePositions.size() << std::endl;
    }
    
    // Randomly select from walkable positions
    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<size_t> dist(0, walkablePositions.size() - 1);
    
    return walkablePositions[dist(rng)];
}

