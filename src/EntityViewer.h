#pragma once

#include <d3dx9.h>

#include "IRenderable.h"
#include "Util.h"

namespace Game {
    class Entity;
}

class EntityViewer : public IRenderable
{
public:
    EntityViewer(std::shared_ptr<hl::Drawer> drawer);

    void Draw() override;

    // encapsulate in IRenderable?
    bool m_visible = false;
private:
    enum class TriggerType
    {
        Volume,
        Plane,
        //Checkpoint // ?
    };

    void DrawTrigger(const Game::Entity* ent, TriggerType type) const;
    void DrawEntityName(const Game::Entity* ent) const;

    std::unique_ptr<ID3DXMesh, Util::COMDeleter> m_box;
    std::unique_ptr<ID3DXFont, Util::COMDeleter> m_font;
};
