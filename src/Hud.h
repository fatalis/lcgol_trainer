#pragma once

#include <hacklib/Drawer.h>

#include "IRenderable.h"

class Hud : public IRenderable
{
	using IRenderable::IRenderable;
	void Draw() override;
};
