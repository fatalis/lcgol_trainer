#include "stdafx.h"

#include "Hooks.h"
#include "Game.h"

namespace Hooks {

struct CallbackData
{
	HookType type;
	std::function<void()> callback;
};

static CNktHookLib s_hook_lib;
static std::vector<CallbackData> s_hook_callbacks;

static EndScene_t EndScene_Orig;

enum IDirect3D9_VTable
{
	IDirect3D9_CreateDevice = 16
};

enum IDirect3DDevice9_VTable
{
	IDirect3DDevice9_EndScene = 42
};

HRESULT WINAPI EndScene_Hook(IDirect3DDevice9* this_)
{
	if (this_ == Game::PCDeviceManager::Get()->getDevice())
	{
		for (auto& cb : s_hook_callbacks)
		{
			if (cb.type == HookType::EndScene)
				cb.callback();
		}
	}

	return EndScene_Orig(this_);
}

void GameFlowPlayGame_Hook()
{
	Game::Functions::GameFlowPlayGame();

	for (auto& cb : s_hook_callbacks)
	{
		if (cb.type == HookType::GameFlowPlayGame)
			cb.callback();
	}
}

bool InitEngine_Hook(float unk_zero)
{
	auto ret = Game::Functions::InitEngine(unk_zero);

	auto device = Game::PCDeviceManager::Get()->getDevice();
	auto vtab = *reinterpret_cast<std::size_t**>(device);
	auto endScene = EndScene_t(vtab[IDirect3DDevice9_EndScene]);

	SIZE_T id;
	s_hook_lib.Hook(&id, reinterpret_cast<LPVOID*>(&EndScene_Orig), endScene, &EndScene_Hook);

	for (auto& cb : s_hook_callbacks)
	{
		if (cb.type == HookType::InitEngine)
			cb.callback();
	}

	return ret;
}

bool Install()
{
	SIZE_T id;
	LPVOID orig;

	s_hook_lib.Hook(&id, &orig, Game::Functions::GameFlowPlayGame, &GameFlowPlayGame_Hook, NKTHOOKLIB_DisallowReentrancy);
	s_hook_lib.Hook(&id, &orig, Game::Functions::InitEngine, &InitEngine_Hook, NKTHOOKLIB_DisallowReentrancy);

	return true;
}

void AddHookCallback(HookType type, std::function<void()> callback)
{
	s_hook_callbacks.push_back(CallbackData { type, callback });
}

} // namespace Hooks
