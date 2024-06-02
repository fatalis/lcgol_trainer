#include <subhook.h>

#include "Hooks.h"
#include "Game.h"

namespace Hooks {

static subhook::Hook s_playgame_hook;
static subhook::Hook s_endscene_hook;
static subhook::Hook s_initengine_hook;

struct CallbackData
{
	HookType type;
	std::function<void()> callback;
};

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

typedef HRESULT (*foo_func)(IDirect3DDevice9* this_);
HRESULT WINAPI EndScene_Hook(IDirect3DDevice9* this_)
{
	subhook::ScopedHookRemove remove(&s_endscene_hook);

	if (this_ == Game::PCDeviceManager::Get()->getDevice())
	{
		for (auto& cb : s_hook_callbacks)
		{
			if (cb.type == HookType::EndScene) {
				cb.callback();
			}
		}
	}

	return EndScene_Orig(this_);
}

void GameFlowPlayGame_Hook()
{
	subhook::ScopedHookRemove remove(&s_playgame_hook);

	Game::Functions::GameFlowPlayGame();

	for (auto& cb : s_hook_callbacks)
	{
		if (cb.type == HookType::GameFlowPlayGame)
			cb.callback();
	}
}

bool InitEngine_Hook(float unk_zero)
{
	subhook::ScopedHookRemove remove(&s_initengine_hook);

	auto ret = Game::Functions::InitEngine(unk_zero);

	auto device = Game::PCDeviceManager::Get()->getDevice();
	auto vtab = *reinterpret_cast<std::size_t**>(device);
	auto endScene = EndScene_t(vtab[IDirect3DDevice9_EndScene]);

	EndScene_Orig = endScene;
	s_endscene_hook.Install((void*)endScene, (void*)EndScene_Hook);

	for (auto& cb : s_hook_callbacks)
	{
		if (cb.type == HookType::InitEngine)
			cb.callback();
	}

	return ret;
}

bool Install()
{
	s_playgame_hook.Install((void*)Game::Functions::GameFlowPlayGame, (void*)GameFlowPlayGame_Hook);
	s_initengine_hook.Install((void*)Game::Functions::InitEngine, (void*)InitEngine_Hook);

	return true;
}

void AddHookCallback(HookType type, std::function<void()> callback)
{
	s_hook_callbacks.push_back(CallbackData { type, callback });
}

} // namespace Hooks
