#include <subhook.h>

#include "Hooks.h"
#include "Game.h"

namespace Hooks {

subhook::Hook playgame_hook;
subhook::Hook endscene_hook;
subhook::Hook initengine_hook;

struct CallbackData
{
	HookType type;
	std::function<void()> callback;
};

// static CNktHookLib s_hook_lib;
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
    printf("test endscene\n");
        subhook::ScopedHookRemove remove(&endscene_hook);

	if (this_ == Game::PCDeviceManager::Get()->getDevice())
	{
		for (auto& cb : s_hook_callbacks)
		{
			if (cb.type == HookType::EndScene) {
                            printf("callback start\n");
				cb.callback();
                            printf("callback end\n");
                        }
		}
	}

	return EndScene_Orig(this_);
        // ((foo_func)endscene_hook.GetTrampoline())(this_);
        // return S_OK;
	// return EndScene_Orig(this_);
}

void GameFlowPlayGame_Hook()
{
    printf("test playgame\n");
        subhook::ScopedHookRemove remove(&playgame_hook);

	Game::Functions::GameFlowPlayGame();

	for (auto& cb : s_hook_callbacks)
	{
		if (cb.type == HookType::GameFlowPlayGame)
			cb.callback();
	}
}

bool InitEngine_Hook(float unk_zero)
{
    printf("test initengine\n");
        subhook::ScopedHookRemove remove(&initengine_hook);

	auto ret = Game::Functions::InitEngine(unk_zero);

	auto device = Game::PCDeviceManager::Get()->getDevice();
	auto vtab = *reinterpret_cast<std::size_t**>(device);
	auto endScene = EndScene_t(vtab[IDirect3DDevice9_EndScene]);

	SIZE_T id;
	// s_hook_lib.Hook(&id, reinterpret_cast<LPVOID*>(&EndScene_Orig), endScene, &EndScene_Hook);
        EndScene_Orig = endScene;
        endscene_hook.Install((void*)endScene, (void*)EndScene_Hook);

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

        playgame_hook.Install((void*)Game::Functions::GameFlowPlayGame, (void*)GameFlowPlayGame_Hook);
        initengine_hook.Install((void*)Game::Functions::InitEngine, (void*)InitEngine_Hook);
	// s_hook_lib.Hook(&id, &orig, Game::Functions::GameFlowPlayGame, &GameFlowPlayGame_Hook, NKTHOOKLIB_DisallowReentrancy);
	// s_hook_lib.Hook(&id, &orig, Game::Functions::InitEngine, &InitEngine_Hook, NKTHOOKLIB_DisallowReentrancy);

	return true;
}

void AddHookCallback(HookType type, std::function<void()> callback)
{
	s_hook_callbacks.push_back(CallbackData { type, callback });
}

} // namespace Hooks
