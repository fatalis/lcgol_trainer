#pragma once

#include <d3dx9.h>
#include "ImplementMember.h"

#include "Util.h"

#define IMPLFUNC(rettype, name, addr, ...) \
    public: \
        rettype name(EXPAND_ARGS(ARGS_FULL, __VA_ARGS__)) \
        { \
            return (\
                (rettype(__thiscall*)(uintptr_t EXPAND_ARGS(ARGS_TYPES, __VA_ARGS__))) \
                ((uintptr_t)addr)) \
                ((uintptr_t)this EXPAND_ARGS(ARGS_NAMES, __VA_ARGS__)); \
        }

//class Entity;

namespace Game {

enum class EntityID : short
{
	TriggerVolume = 232,
	Checkpoint = 233,
	TriggerPlane = 238,
};

enum class GameState
{
	NullAkaArrayStart,
	PlayGame,
	LoadLevel,
	PostLoad,
	Movie,
	SamplerDemo,
	Attract,
	MainMenu,
	StaticScreen,
	Quit,
	LoadWaitOnInput,
	FrontEndUIFlow,
	LevelCompletion
};

enum class CameraType : int {
	Transition = 3,
	Anchored = 4,
	Free = 5,
	ModelView = 6,
	Cinematic = 7,
	AutoSpline = 8,
	ObjectDebug = 9,
	Cheat = 10,
	Game = 11,
	Lara = 12,
	Vehicle = 13,
	Scope = 14,
	FollowSpline = 15,
};

class BaseCamera
{
	IMPLMEMBER(D3DXVECTOR3, Position, 0x40);
	IMPLMEMBER(D3DXMATRIX, ViewMatrix, 0x50);
	IMPLMEMBER(float, FOVRads, 0x130);
};
class CameraManager // size 70432d
{
public:
	//IMPLVTFUNC_OR(void, Update, 10);
	IMPLMEMBER(BaseCamera*, CurrentCamera, 0x44);
	//IMPLVTFUNC_OR(BaseCamera*, GetCurrentCamera, 56); // ords are -3 of what they appear to be?
	IMPLVTFUNC_OR(BaseCamera*, GetCameraByType, 57, CameraType, type);
	IMPLFUNC(void, SetCamera, 0x51FA00, BaseCamera*, camera);

	static CameraManager* Get(); // TODO: interface?	
};
class Player // size 8208d
{
	IMPLMEMBER(CameraManager*, CameraManager, 0x1E9C);
	//IMPLMEMBER(Entity*, PlayerEntity, 0x1E78);
	IMPLMEMBER(Util::Flags, Flags, 0x10);

	enum PlayerFlags
	{
		FlyMode = 4
	};

	static Player* Get();
};
class PlayerPtrArray
{
	IMPLMEMBER(Player*, Player1, 0);
	IMPLMEMBER(Player*, Player2, 4);
};
class PlayerManager // size 20d
{
	//IMPLMEMBER(int, PlayerArrayLength, 8);
	IMPLMEMBER(PlayerPtrArray*, PlayerPtrArray, 0xC);

	static PlayerManager* Get();
	static const int offset = 0x117F8EC;
};



class EntityBoundsData
{
	IMPLMEMBER(D3DXVECTOR3, Size, 0x34);
};
class EntityInstanceData
{
	IMPLMEMBER(EntityBoundsData*, BoundsData, 0x88);
	IMPLMEMBER(EntityID, ID, 0x50);
	IMPLMEMBER(short, Num, 0x52);
};
class EntityStaticInfo
{
	IMPLMEMBER(const char*, NamePtr, 0x60);
};
class Entity // size is 832d
{
	IMPLMEMBER(EntityStaticInfo*, StaticInfo, 0x10);
	IMPLMEMBER(D3DXVECTOR3, Position, 0x20);
	IMPLMEMBER(D3DXVECTOR3, Angle, 0x30);
	IMPLMEMBER(bool, IsActive, 0x48);
	IMPLMEMBER(D3DXVECTOR3, RenderSizeScale, 0x70);
	IMPLMEMBER(EntityInstanceData*, InstanceData, 0xF0);
	IMPLMEMBER(Player*, PlayerPtrWhenPlayer, 0x330);

	static const int size = 832;
};
class EntityList
{
public:
	static const int offset = 0xE8BFD0;
	static const int length = 1700;
};

class PlayerEntityPtrArray
{
	IMPLMEMBER(Entity*, Player1, 0);
	IMPLMEMBER(Entity*, Player2, 4);

	static PlayerEntityPtrArray* Get();
	static const int offset = 0xA4E668;
};
// static obj info first item is flags - 0x40000 may be "is player"


class PCDeviceManager
{
	IMPLMEMBER(D3DPRESENT_PARAMETERS, PresentParams, 0x1F8);
	IMPLMEMBER(IDirect3DDevice9*, Device, 0x20);

	static PCDeviceManager* Get();
	static const int offset = 0xC84554;
};

class GameFlowStatePlayGame
{
	IMPLVTFUNC_OR(void, Run, 3);

	static const int offset = 0x10EB400;
};

namespace Functions {
	namespace details {
		typedef GameState(*GetCurrentGameState_t)();
		typedef void(*GameFlowPlayGame_t)();
		typedef bool(*InitEngine_t)(float unk_zero);
	}
	static const auto GetCurrentGameState = reinterpret_cast<details::GetCurrentGameState_t>(0x60BD50);
	static const auto GameFlowPlayGame = reinterpret_cast<details::GameFlowPlayGame_t>(0x62E400);
	static const auto InitEngine = reinterpret_cast<details::InitEngine_t>(0x643FB0);
}

namespace Globals {
	static const auto g_main_window = reinterpret_cast<HWND*>(0xB9BA10);
	static const auto g_menu_state = reinterpret_cast<uint8_t*>(0x10A8E9F); // 6 when in in-game menus, 0 when not
	static const auto g_num_active_players = reinterpret_cast<int*>(0xA4E660);
}

namespace Util {
	static D3DXMATRIX GetProjectionMatrix()
	{
		D3DXMATRIX proj;
		auto fov = CameraManager::Get()->getCurrentCamera()->getFOVRads();
		const auto& pp = Game::PCDeviceManager::Get()->getPresentParams();
		D3DXMatrixPerspectiveFovLH(&proj, fov, static_cast<float>(pp.BackBufferWidth) / pp.BackBufferHeight, 1.0f, 100000.0f);
		return proj;
	}

	static D3DXMATRIX GetViewMatrix()
	{
		auto view = CameraManager::Get()->getCurrentCamera()->getViewMatrix();
		view._12 *= -1;
		view._22 *= -1;
		view._32 *= -1;
		view._42 *= -1;
		return view;
	}
}

} // namespace Game
