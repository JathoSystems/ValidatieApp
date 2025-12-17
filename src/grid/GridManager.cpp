#include "grid/GridManager.h"

std::unordered_map<std::string, std::unique_ptr<LevelGrid>> GridManager::_grids;

void GridManager::registerGrid(const std::string& sceneName, std::unique_ptr<LevelGrid> grid) {
    _grids[sceneName] = std::move(grid);
}

LevelGrid* GridManager::getGrid(const std::string& sceneName) {
    auto it = _grids.find(sceneName);
    if (it != _grids.end()) {
        return it->second.get();
    }
    return nullptr;
}

void GridManager::unregisterGrid(const std::string& sceneName) {
    _grids.erase(sceneName);
}

void GridManager::clear() {
    _grids.clear();
}
