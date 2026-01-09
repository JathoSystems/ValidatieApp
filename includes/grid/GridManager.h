#ifndef VALIDATIEAPP_GRIDMANAGER_H
#define VALIDATIEAPP_GRIDMANAGER_H

#include "grid/LevelGrid.h"
#include <memory>
#include <unordered_map>
#include <string>

class GridManager {
private:
    static std::unordered_map<std::string, std::unique_ptr<LevelGrid>> _grids;

public:
    static void registerGrid(const std::string& sceneName, std::unique_ptr<LevelGrid> grid);
    static LevelGrid* getGrid(const std::string& sceneName);
    static void unregisterGrid(const std::string& sceneName);
    static void clear();
};

#endif //VALIDATIEAPP_GRIDMANAGER_H
