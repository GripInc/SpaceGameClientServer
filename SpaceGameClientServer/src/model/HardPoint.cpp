#include "model/HardPoint.h"

#include "model/GameSettings.h"

#include "utils/OgreBulletConvert.h"

void HardPoint::attachWeapon(const WeaponSettings& _weaponSettings)
{
	mWeaponSettings = _weaponSettings;

	mShotSettings = *GameSettings::getInstance().getShot(_weaponSettings.mShotType);

	mWeaponSettings.mNoslePosition += mWeaponSettings.mHardPoint.mPosition;
}