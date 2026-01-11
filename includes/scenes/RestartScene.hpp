#ifndef VUURJONGEN_WATERMEISJE_GAME_RESTARTSCENE_HPP
#define VUURJONGEN_WATERMEISJE_GAME_RESTARTSCENE_HPP
#include "Network/NetworkSystem.h"
#include "Scenes/Scene.h"

class RestartScene : public Scene {
private:
    std::string _previousLevel;
    std::shared_ptr<NetworkSystem> _network;
    bool _isOnline;

public:
    RestartScene(std::shared_ptr<NetworkSystem> network);

    void setTargetLevel(std::string level) { _previousLevel = level; }

    void setOnline(bool online) { _isOnline = online; }

    void onInitialRender() override;
};

#endif //VUURJONGEN_WATERMEISJE_GAME_RESTARTSCENE_HPP
