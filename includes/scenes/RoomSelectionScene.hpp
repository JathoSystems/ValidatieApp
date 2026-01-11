#ifndef VUURJONGEN_WATERMEISJE_ROOMSELECTIONSCENE_HPP
#define VUURJONGEN_WATERMEISJE_ROOMSELECTIONSCENE_HPP

#include "Scenes/Scene.h"
#include <memory>
#include <functional>

class NetworkSystem;
class Text;

class RoomSelectionScene : public Scene {
private:
    std::shared_ptr<NetworkSystem> _network;
    int _selectedLevel;
    int _currentLobbyIdInput;
    Text *_displayTextPtr = nullptr;
    std::function<std::unique_ptr<Scene>()> _levelFactory;

public:
    RoomSelectionScene(std::shared_ptr<NetworkSystem> network, int levelNumber,
                       std::function<std::unique_ptr<Scene>()> levelFactory);

    void updateLobbyIdDisplay();

    std::function<std::unique_ptr<Scene>()> getLevelFactory() const;
};

#endif //VUURJONGEN_WATERMEISJE_ROOMSELECTIONSCENE_HPP
