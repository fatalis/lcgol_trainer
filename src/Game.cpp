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

void Functions::DrawString(float x, float y, unsigned int color, const char* fmt, ...)
{
    float pos[2] = {x, y};
    Functions::DrawStringOrig(pos, color, 4, fmt);
}

} // namespace Game
