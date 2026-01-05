#ifndef VUURJONGEN_WATERMEISJE_ROOMSELECTIONSCENE_HPP
#define VUURJONGEN_WATERMEISJE_ROOMSELECTIONSCENE_HPP

#include "Scenes/Scene.h"
#include <memory>

class NetworkSystem;
class Text;

class RoomSelectionScene : public Scene {
private:
    std::shared_ptr<NetworkSystem> _network;
    int _selectedLevel;
    int _currentLobbyIdInput;
    Text* _displayTextPtr = nullptr;

public:
    RoomSelectionScene(std::shared_ptr<NetworkSystem> network, int levelNumber);
    void updateLobbyIdDisplay();
};

#endif //VUURJONGEN_WATERMEISJE_ROOMSELECTIONSCENE_HPP
