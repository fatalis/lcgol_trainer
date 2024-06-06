#pragma once

#include <memory>
#include "EntityViewer.h"
#include "Menu.h"

class EntityViewer;

class DebugMenu : public Menu
{
public:
    DebugMenu(std::shared_ptr<hl::Drawer> drawer, std::shared_ptr<Input> input, std::shared_ptr<EntityViewer> entity_viewer, std::shared_ptr<Trainer>);
};
