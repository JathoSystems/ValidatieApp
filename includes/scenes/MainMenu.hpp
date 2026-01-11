#ifndef VUURJONGEN_WATERMEISJE_MAINMENU_HPP
#define VUURJONGEN_WATERMEISJE_MAINMENU_HPP

#include "Engine/GameEngine.h"
#include "Scenes/Scene.h"

class MainMenu : public Scene {
public:
    MainMenu();

    void onInitialRender() override;
    void onExit() override {
        GameEngine::getInstance().getSystem<AudioSystem>()->stopMusic();
    }

};

#endif
