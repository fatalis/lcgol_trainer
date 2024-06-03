#include <map>

#include "Trainer.h"
#include "EntityViewer.h"
#include "Game.h"
#include "Hooks.h"
#include "Util.h"

#include "assert.h"

using namespace std::literals::string_literals;

Trainer::~Trainer()
{

}

void Trainer::Initialize()
{
	this->ApplyPatches();

	Hooks::Install();
	Hooks::AddHookCallback(Hooks::HookType::InitEngine, Util::weak_callback(shared_from_this(), std::bind(&Trainer::InitOverlay, this)));
	Hooks::AddHookCallback(Hooks::HookType::EndScene,   Util::weak_callback(shared_from_this(), std::bind(&Trainer::Draw,        this)));
}

void Trainer::Test()
{
	if (Game::Functions::GetCurrentGameState() != Game::GameState::PlayGame)
		return;

	//auto device = g_device;
	auto device = Game::PCDeviceManager::Get()->getDevice();

	IDirect3DStateBlock9* state;
	assert(SUCCEEDED(device->CreateStateBlock(D3DSBT_ALL, &state)));
	assert(SUCCEEDED(state->Capture()));

	IDirect3DVertexShader9* shader;
	assert(SUCCEEDED(device->GetVertexShader(&shader)));

	LPD3DXMESH mesh;
	assert(SUCCEEDED(D3DXCreateBox(device, 256.0f, 256.0f, 512.0f, &mesh, nullptr)));

	hl::Drawer drawer;
	drawer.SetDevice(device);
	// drawer.DrawCircle(256.0f, 256.0f, 256.0f, -1);

	std::vector<hl::VERTEX_3D_COL> boxVerts {
		{ -250.0f, -250.0f, 0 },
		{ -250.0f, 250.0f, 0 },
		{ 250.0f, -250.0f, 0 },
		{ 250.0f, 250.0f, 0 },
		{ -250.0f, -250.0f, 500 },
		{ -250.0f, 250.0f, 500 },
		{ 250.0f, -250.0f, 500 },
		{ 250.0f, 250.0f, 500 },
	};
	//std::vector<hl::VERTEX_3D_COL> boxVerts{
	//	{ -0.5f, -0.5f, 0 },
	//	{ -0.5f, 0.5f, 0 },
	//	{ 0.5f, -0.5f, 0 },
	//	{ 0.5f, 0.5f, 0 },
	//	{ -0.5f, -0.5f, 2 },
	//	{ -0.5f, 0.5f, 2 },
	//	{ 0.5f, -0.5f, 2 },
	//	{ 0.5f, 0.5f, 2 },
	//};
	std::vector<unsigned int> boxInds {
		0, 1, 3, 2, 0, 4, 5, 1, 5, 7, 3, 7, 6, 2, 6, 4,
	};

	auto m_vbBox = drawer.AllocVertexBuffer(boxVerts);
	auto m_ibBox = drawer.AllocIndexBuffer(boxInds);

	D3DXMATRIX world;
	const auto& pos = Game::PlayerEntityPtrArray::Get()->getPlayer1()->getPosition();
	D3DXMatrixTranslation(&world, pos.x, pos.y, pos.z);
	//D3DXMatrixIdentity(&world);
	drawer.Update(Game::Util::GetViewMatrix(), Game::Util::GetProjectionMatrix());
	//drawer.DrawPrimitive(m_vbBox, m_ibBox, D3DPT_LINESTRIP, world, -1);
	device->SetVertexShader(nullptr);
	drawer.DrawPrimitive(m_vbBox, m_ibBox, D3DPT_LINESTRIP, world, 0xff00ff00);

	drawer.ReleaseVertexBuffer(m_vbBox);
	drawer.ReleaseIndexBuffer(m_ibBox);

	device->SetFVF(D3DFVF_XYZ);
	assert(SUCCEEDED(device->SetTexture(0, nullptr))); //
	// assert(SUCCEEDED(mesh->DrawSubset(0)));
	//assert(SUCCEEDED(device->DrawPrimitiveUP(D3DPT_LINESTRIP, 8, boxVerts.data(), sizeof(hl::VERTEX_3D_COL))));
	// TODO: use DrawPrimativeUP

	mesh->Release();

	assert(SUCCEEDED(device->SetVertexShader(shader)));

	assert(SUCCEEDED(state->Apply()));
	state->Release();
}

void Trainer::Draw()
{
	if (Game::Functions::GetCurrentGameState() != Game::GameState::PlayGame)
		return;

	if (*Game::Globals::g_num_active_players == 0)
		return;

	// this->Test();

	m_input->Update();

#if 1
	if (m_input->WasPressed(Input::Button::Special))
	{
		// auto camera = Game::CameraManager::Get()->GetCameraByType(Game::CameraType::Free);
		// Game::CameraManager::Get()->SetCamera(camera);
	}
	else if (GetAsyncKeyState(VK_F2))
	{
		auto camera = Game::CameraManager::Get()->GetCameraByType(Game::CameraType::Game);
		Game::CameraManager::Get()->SetCamera(camera);
	}
	else if (GetAsyncKeyState(VK_F3))
	{
		// Game::Player::Get()->setFlags(16);
	}
	else if (GetAsyncKeyState(VK_F4))
	{
		// Game::Player::Get()->setFlags(0);
	}
	else if (GetAsyncKeyState(VK_F5))
	{
		Game::PCDeviceManager::Get()->getDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else if (GetAsyncKeyState(VK_F6))
	{
		Game::PCDeviceManager::Get()->getDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	else if (GetAsyncKeyState(VK_F7))
	{
		auto pos = Game::PlayerEntityPtrArray::Get()->getPlayer1()->getPosition();

		for (auto i = 0; i < Game::EntityList::length; i++)
		{
			const auto ent = reinterpret_cast<Game::Entity*>(Game::EntityList::offset + (Game::Entity::size * i));

			if (!ent->getIsActive())
				continue;

			const Game::EntityInstanceData* instance_data = ent->getInstanceData();
			if (!instance_data)
				continue;

			const Game::EntityStaticInfo* static_data = ent->getStaticInfo();
			if (!static_data)
				continue;

			if (std::string(static_data->getNamePtr()) == "shaman_melee")
			{
				ent->setIsActive(false);
			}
		}
	}
	else if (GetAsyncKeyState(VK_F8))
	{
		auto pos = Game::PlayerEntityPtrArray::Get()->getPlayer1()->getPosition();

		for (auto i = 0; i < Game::EntityList::length; i++)
		{
			Game::Entity* ent = reinterpret_cast<Game::Entity*>(Game::EntityList::offset + (Game::Entity::size * i));

			if (!ent->getIsActive())
				continue;

			const Game::EntityInstanceData* instance_data = ent->getInstanceData();
			if (!instance_data)
				continue;

			const Game::EntityStaticInfo* static_data = ent->getStaticInfo();
			if (!static_data)
				continue;

			if (std::string(static_data->getNamePtr()) == "shaman_melee")
			{
				ent->setIsActive(true);
			}
		}
	}
#endif

	if (!*Game::Globals::g_menu_state)
	{
		auto device = Game::PCDeviceManager::Get()->getDevice();

		IDirect3DStateBlock9* state;
		assert(SUCCEEDED(device->CreateStateBlock(D3DSBT_ALL, &state)));
		assert(SUCCEEDED(state->Capture()));

		assert(SUCCEEDED(device->SetVertexShader(nullptr)));

		auto view = Game::Util::GetViewMatrix();
		auto projection = Game::Util::GetProjectionMatrix();
		m_drawer->Update(view, projection);

		assert(SUCCEEDED(device->SetTransform(D3DTS_VIEW, &view)));
		assert(SUCCEEDED(device->SetTransform(D3DTS_PROJECTION, &projection)));

		for (auto& renderable : m_renderables)
			renderable->Draw();

		assert(SUCCEEDED(state->Apply()));
		state->Release();
	}
}

void Trainer::InitOverlay()
{
	m_drawer->SetDevice(Game::PCDeviceManager::Get()->getDevice());

	m_entity_viewer = std::make_shared<EntityViewer>(m_drawer);
	m_renderables.push_back(m_entity_viewer);

	m_renderables.push_back(std::make_shared<DebugMenu>(m_drawer, m_input, m_entity_viewer));
}

void Trainer::ApplyPatches() const
{
	static const std::map<std::size_t, std::string> patches = {
		// skip intro movies
		{ 0x78F160, "\x01"s }, // 0x02 skips final intro as well, but can't continue from save file for whatever reason

#ifdef _DEBUG
		// borderless windowed for debugging
		{ 0x5CE355, "\x86"s }, // windowed mode d3d device params
		{ 0x5CE36D, "\x8E"s }, // set backbuffer to D3DFMT_UNKNOWN
		{ 0x5CE34F, "\x8E"s }, // set FullScreen_RefreshRateInHz to 0
		{ 0x784AED, "\x00"s }, // disable WS_EX_TOPMOST
		{ 0x57BB16, "\x00"s }, // disable HWND_TOPMOST in SetWindowPos call
		{ 0xB9BA04, "\x01"s }, // enable cursor / don't call ClipCursor
		{ 0x7ED441, "\xEB"s }, // turn off multithreaded rendering
		// { 0x618CC5, "\xEB"s }, // don't create a D3DCREATE_PUREDEVICE
		// { 0x784B06, "\x83\xc4\x08\x90\x90\x90"s }, // don't ShowWindow() immediately so we can debug without it waiting for event loop
#endif

		// CEG memory hash checks
		{ 0x43F902, "\x60\x07\x42\x00\x90\x90"s }, { 0x459E42, "\xA0\xD3\x64\x00\x90\x90"s }, { 0x4682F2, "\xB0\x44\x81\x00\x90\x90"s },
		{ 0x4807D2, "\xE0\x57\x7A\x00\x90\x90"s }, { 0x4AD182, "\x20\xB1\x4C\x00\x90\x90"s }, { 0x4D3042, "\x10\x64\x44\x00\x90\x90"s },
		{ 0x4D6A12, "\x00\x0F\x63\x00\x90\x90"s }, { 0x526262, "\xA0\x9E\x75\x00\x90\x90"s }, { 0x58AF02, "\x00\xCD\x4B\x00\x90\x90"s },
		{ 0x5A3412, "\xB0\xC9\x71\x00\x90\x90"s }, { 0x5CC412, "\x30\xE2\x50\x00\x90\x90"s }, { 0x5CE532, "\x60\x1F\x7A\x00\x90\x90"s },
		{ 0x5EE592, "\xC0\xC6\x60\x00\x90\x90"s }, { 0x5FB8D2, "\x60\x11\x7B\x00\x90\x90"s }, { 0x607792, "\xE0\x6B\x6E\x00\x90\x90"s },
		{ 0x60BEC2, "\xA0\xC9\x77\x00\x90\x90"s }, { 0x60C922, "\xB0\x89\x56\x00\x90\x90"s }, { 0x640322, "\x40\x7F\x52\x00\x90\x90"s },
		{ 0x654C92, "\xC0\x44\x61\x00\x90\x90"s }, { 0x657A62, "\x70\xBB\x78\x00\x90\x90"s }, { 0x65DA32, "\x40\x15\x6A\x00\x90\x90"s },
		{ 0x67FC22, "\xE0\x93\x4E\x00\x90\x90"s }, { 0x69F9B2, "\xA0\xBB\x6C\x00\x90\x90"s }, { 0x6BE4F2, "\x80\x29\x48\x00\x90\x90"s },
		{ 0x6D6B62, "\x20\x8E\x51\x00\x90\x90"s }, { 0x6E9362, "\x80\xF1\x53\x00\x90\x90"s }, { 0x700FE2, "\x10\x0C\x44\x00\x90\x90"s },
		{ 0x7044D2, "\x10\xF3\x78\x00\x90\x90"s }, { 0x729012, "\xB0\xCC\x60\x00\x90\x90"s }, { 0x747562, "\x90\x8E\x6B\x00\x90\x90"s },
		{ 0x7588F2, "\xF0\xD9\x4D\x00\x90\x90"s }, { 0x75AA82, "\x50\x25\x40\x00\x90\x90"s }, { 0x7669D2, "\x60\x3F\x4C\x00\x90\x90"s },
		{ 0x77D462, "\x20\xDB\x4B\x00\x90\x90"s }, { 0x7BE002, "\x50\x2E\x6D\x00\x90\x90"s }, { 0x7C6142, "\xA0\xD1\x6B\x00\x90\x90"s },
		{ 0x7D0162, "\x10\x4D\x62\x00\x90\x90"s }, { 0x8074E2, "\x30\x5C\x69\x00\x90\x90"s },
	};

	for (auto& kv : patches)
	{
		Util::memcpy_protected(
			reinterpret_cast<void*>(kv.first),
			kv.second.c_str(),
			kv.second.length());
	}
}
