#pragma once

#include "Menu.h"

class DebugMenu : public Menu
{
public:
	DebugMenu(std::shared_ptr<hl::Drawer> drawer, std::shared_ptr<Input> input);
};
