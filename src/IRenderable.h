#pragma once

#include <hacklib/Drawer.h>

class Trainer;

class IRenderable
{
public:
    IRenderable(std::shared_ptr<hl::Drawer> drawer) : m_drawer(drawer) { }
    virtual void Draw() = 0;

protected:
    std::shared_ptr<hl::Drawer> m_drawer;

};
