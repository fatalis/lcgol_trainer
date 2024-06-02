#include <d3d9.h>
#include <d3dx9.h>
#include <cassert>

#include "EntityViewer.h"
#include "Game.h"

EntityViewer::EntityViewer(std::shared_ptr<hl::Drawer> drawer)
    : IRenderable(drawer)
{
	LPD3DXMESH box;
	assert(SUCCEEDED(D3DXCreateBox(drawer->GetDevice(), 1.0f, 1.0f, 1.0f, &box, nullptr)));
	m_box.reset(box);

	LPD3DXFONT font;
	assert(SUCCEEDED(D3DXCreateFont(drawer->GetDevice(), 12, 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
	       DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Fixedsys", &font)));
	m_font.reset(font);
}

void EntityViewer::DrawTrigger(const Game::Entity* ent, TriggerType type) const
{
	const auto& size = ent->getInstanceData()->getBoundsData()->getSize();
	const auto& pos = ent->getPosition();
	const auto& angle = ent->getAngle();

	//const auto& player_pos = Game::PlayerEntityPtrArray::Get()->getPlayer1()->getPosition();
	const auto& player_pos = Game::CameraManager::Get()->getCurrentCamera()->getPosition();

	D3DXVECTOR3 diff = pos - player_pos;
	const float max_dist = 15000.0f;
	float dist = D3DXVec3Length(&diff);
	if (dist > max_dist)
		return;
	float from_max = max_dist - dist;

	int alpha = (100 - 0)*(static_cast<int>(from_max) - 0) / (static_cast<int>(max_dist) - 0) + 0;
	alpha = alpha << 24;

	std::size_t str_hash = std::hash<void*>{}(const_cast<Game::Entity*>(ent));
	int color = (str_hash & 0xFFFFFF) | alpha;

	auto device = m_drawer->GetDevice();

	D3DXMATRIX scale, world, rotate, translate;
	D3DXMatrixTranslation(&translate, pos.x, pos.y, pos.z);
	if (type == TriggerType::Plane)
	{
		D3DXMatrixScaling(&scale, size.x * 2, 1.0f, size.z * 2);
		D3DXMatrixRotationYawPitchRoll(&rotate, 0.0f, angle.x, angle.z);
	}
	else
	{
		D3DXMatrixScaling(&scale, size.x * 2, size.y * 2, size.z * 2);
		D3DXMatrixRotationYawPitchRoll(&rotate, angle.y, angle.x, angle.z);
	}
	world = scale * rotate * translate;
	device->SetTransform(D3DTS_WORLD, &world);

	// TODO: revert
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CONSTANT);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CONSTANT);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	device->SetTextureStageState(0, D3DTSS_CONSTANT, color);

	m_box->DrawSubset(0);
}

void EntityViewer::DrawEntityName(const Game::Entity * ent) const
{
	if (!m_font)
		return;

	const auto& pos = ent->getPosition();
	const auto& static_info = ent->getStaticInfo();

	const auto& player_pos = Game::CameraManager::Get()->getCurrentCamera()->getPosition();

	D3DXVECTOR3 diff = pos - player_pos;
	const float max_dist = 10000.0f;
	float dist = D3DXVec3Length(&diff);
	if (dist > max_dist)
		return;
	float from_max = max_dist - dist;

	int alpha = (255 - 0)*(static_cast<int>(from_max) - 0) / (static_cast<int>(max_dist) - 0) + 0;
	alpha = alpha << 24;

	alpha = 0xFF000000;

	D3DXVECTOR3 screen_pos;
	m_drawer->Project(pos, screen_pos);
	if (m_drawer->IsOnScreen(screen_pos))
	{
		const auto& angle = ent->getAngle();
		char tmp[512];
		sprintf_s(tmp, sizeof(tmp), "%s %X", static_info->getNamePtr(), reinterpret_cast<unsigned int>(ent));
		//std::string message = Util::string_format("%s %.03f %.03f %.03f", static_info->getNamePtr(), angle.x, angle.y, angle.z);
		//m_drawer->DrawFont(m_font, screen_pos.x, screen_pos.y, -1, static_info->getNamePtr());
		RECT um { static_cast<LONG>(screen_pos.x), static_cast<LONG>(screen_pos.y), 0, 0 };
		m_font->DrawTextA(nullptr,tmp, -1, &um, DT_NOCLIP, 0x00FFFFFF | alpha);
		//m_drawer->ReleaseFont(m_font);
	}
}

void EntityViewer::Draw()
{
    printf("draw entity viewer\n");
	for (auto i = 0; i < Game::EntityList::length; i++)
	{
            printf("draw entity viewer %d\n", i);
		const Game::Entity* ent = reinterpret_cast<Game::Entity*>(Game::EntityList::offset + (Game::Entity::size * i));

		if (!ent->getIsActive())
			continue;

		const Game::EntityInstanceData* instance_data = ent->getInstanceData();
		if (!instance_data)
			continue;

		switch (instance_data->getID())
		{
		case Game::EntityID::TriggerVolume:
			this->DrawTrigger(ent, TriggerType::Volume);
			break;
		case Game::EntityID::TriggerPlane:
			this->DrawTrigger(ent, TriggerType::Plane);
			break;
		case Game::EntityID::Checkpoint:
			break;
		}

		this->DrawEntityName(ent);
	}
        printf("ent viewer done\n");
}
