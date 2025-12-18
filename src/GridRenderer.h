#ifndef VALIDATIEAPP_GRIDRENDERER_H
#define VALIDATIEAPP_GRIDRENDERER_H

#include "LevelGrid.h"
#include "GameObjects/Component/Component.h"
#include "SDL/Window.h"
#include <memory>
#include <string>

// Component that renders a LevelGrid - attach to a single GameObject
class GridRendererComponent : public Component {
private:
    LevelGrid* _grid;

public:
    explicit GridRendererComponent(LevelGrid* grid);
    ~GridRendererComponent() override = default;
    
    void update(float deltaTime) override {}
    void render(const std::unique_ptr<Window>& window) override;
};

#endif //VALIDATIEAPP_GRIDRENDERER_H

