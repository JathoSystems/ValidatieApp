#ifndef VALIDATIEAPP_GRIDMANAGER_H
#define VALIDATIEAPP_GRIDMANAGER_H

#include "LevelGrid.h"
#include <memory>
#include <unordered_map>
#include <string>

class GridManager {
private:
    static std::unordered_map<std::string, std::unique_ptr<LevelGrid>> _grids;

public:
    // Register a grid for a scene
    static void registerGrid(const std::string& sceneName, std::unique_ptr<LevelGrid> grid);
    
    // Get grid for a scene
    static LevelGrid* getGrid(const std::string& sceneName);
    
    // Remove grid for a scene
    static void unregisterGrid(const std::string& sceneName);
    
    // Clear all grids
    static void clear();
};

#endif //VALIDATIEAPP_GRIDMANAGER_H

