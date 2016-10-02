#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "model/deserialized/WeaponsSettings.h"
#include "model/GameSettings.h"

class ShotSettings;

class Weapon
{
public:
	void init(const WeaponSettings* _weaponSettings)
	{
		mWeaponSettings = _weaponSettings;
		mShotSettings = GameSettings::getInstance().getShot(_weaponSettings->getShotType());

		mHitPoints = mWeaponSettings->getHitPoints();
	}

	//mElapsedTime since last use
	float mElapsedTime = 0.f;

	const std::string& getMesh() const { return mWeaponSettings->getMesh(); }
	float getFireRate() const { return mWeaponSettings->getFireRate(); }
	const btVector3& getNoslePosition() const { return mWeaponSettings->getNoslePosition(); }
	const btVector3& getHardPointPosition() const { return mWeaponSettings->getHardPointSettings().getPosition(); }
	unsigned int getHitPoints() const { return mHitPoints; }
	unsigned int getBaseHitPoints() const { return mWeaponSettings->getHitPoints(); }
	float getConsumption() const { return mWeaponSettings->getConsumption(); }
	const btAlignedObjectArray<CollisionShapeSettings>& getCollisionShapes() const { return mWeaponSettings->getCollisionShapes(); }

	const ShotSettings* getShotSettings() const { return mShotSettings; }

protected:
	unsigned int mHitPoints = 0U;
	
	const WeaponSettings* mWeaponSettings;
	const ShotSettings* mShotSettings;
};

#endif //_WEAPON_H_
