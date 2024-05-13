#include "stdafx.h"

#include "Trainer.h"

static std::shared_ptr<Trainer> s_trainer;

/* RELEASE TODO:

hacklib cmake automatic
hacklib static link crt + fix Hack.cpp compile
use GetViewport instead of present params

*/
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
#ifndef _DEBUG
		if (GetAsyncKeyState(VK_ESCAPE))
			g_trainer = std::make_unique<Trainer>();
#else
		s_trainer = std::make_shared<Trainer>();
		s_trainer->Initialize();
#endif
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		s_trainer.reset(); // TODO: test shutdown
		break;
	}

	return TRUE;
}
