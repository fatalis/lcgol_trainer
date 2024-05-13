#include "stdafx.h"

#include "Game.h"

namespace Game {

CameraManager* CameraManager::Get()
{
	return Player::Get()->getCameraManager();
}

Player* Player::Get()
{
	return PlayerManager::Get()->getPlayerPtrArray()->getPlayer1();
}

PlayerManager* PlayerManager::Get() // TODO: interface?
{
	return *reinterpret_cast<PlayerManager**>(offset);
}

PlayerEntityPtrArray* PlayerEntityPtrArray::Get()
{
	return *reinterpret_cast<PlayerEntityPtrArray**>(offset);
}

PCDeviceManager* PCDeviceManager::Get()
{
	return *reinterpret_cast<PCDeviceManager**>(offset);
}

} // namespace Game
