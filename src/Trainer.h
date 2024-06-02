#pragma once

#include <hacklib/Drawer.h>

#include "DebugMenu.h"
#include "EntityViewer.h"
#include "Hud.h"
#include "Input.h"
#include "IRenderable.h"

class Trainer : public std::enable_shared_from_this<Trainer>
{
public:
	~Trainer();
	void Initialize();
	void Draw();
	void InitOverlay();
	void Test();

private:
	void ApplyPatches() const;

	std::unique_ptr<DebugMenu> m_menu;
	std::unique_ptr<Hud> m_hud;
	std::unique_ptr<EntityViewer> m_entity_viewer;
	std::shared_ptr<hl::Drawer> m_drawer = std::make_shared<hl::Drawer>();

	std::vector<std::unique_ptr<IRenderable>> m_renderables;

	std::shared_ptr<Input> m_input = std::make_shared<Input>();
};
