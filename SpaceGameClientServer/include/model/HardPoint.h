#ifndef _HARD_POINT_H_
#define _HARD_POINT_H_

#include <string>
#include "LinearMath/btVector3.h"

#include "model/deserialized/WeaponsSettings.h"
#include "model/deserialized/ShotsSettings.h"

class HardPoint
{
public:
	HardPoint(int _index, const btVector3& _position, float _roll)
		: mIndex(_index),
		mPosition(_position),
		mRoll(_roll)
	{}

	int getIndex() const { return mIndex; }
	const btVector3& getPosition() const { return mPosition; }
	float getRoll() const { return mRoll; }

	void attachWeapon(const WeaponSettings& _weaponSettings);
	void detachWeapon() { mWeaponSettings.mName = ""; }
	bool isUsed() { return mWeaponSettings.mName != ""; }

	const WeaponSettings& getWeaponSettings() const { return mWeaponSettings; }
	const ShotSettings& getShotSettings() const { return mShotSettings; }

	//mElapsedTime since last use
	float mElapsedTime = 0.f;

protected:
	int mIndex = 0;
	btVector3 mPosition;
	float mRoll = 0.f;

	WeaponSettings mWeaponSettings;
	ShotSettings mShotSettings;
};

#endif //_HARD_POINT_H_