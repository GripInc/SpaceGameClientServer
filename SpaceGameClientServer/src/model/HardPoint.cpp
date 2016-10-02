#include "model/HardPoint.h"

#include "model/GameSettings.h"

#include "utils/OgreBulletConvert.h"

void HardPoint::attachWeapon(const WeaponSettings* _weaponSettings)
{
	mWeapon.init(_weaponSettings);
	mEmpty = false;
}

void HardPoint::update(float _deltaTime)
{
	mWeapon.mElapsedTime += _deltaTime;
}