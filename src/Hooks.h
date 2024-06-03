#pragma once

#include "Trainer.h"

typedef HRESULT (WINAPI *EndScene_t)(IDirect3DDevice9* this_);

namespace Hooks {
    enum class HookType
    {
        InitEngine,
        GameFlowPlayGame,
        EndScene
    };

    bool Install();
    void AddHookCallback(HookType type, std::function<void()> callback);
}

