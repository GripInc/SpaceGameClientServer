#ifndef _HARD_POINT_H_
#define _HARD_POINT_H_

#include <string>
#include "LinearMath/btVector3.h"

#include "model/Weapon.h"

class HardPoint
{
public:
	void init(int _index, const btVector3& _position, float _roll)
	{
		mIndex = _index;
		mPosition = _position;
		mRoll = _roll;
	}

	int getIndex() const { return mIndex; }
	const btVector3& getPosition() const { return mPosition; }
	float getRoll() const { return mRoll; }

	void attachWeapon(const WeaponSettings* _weaponSettings);
	void detachWeapon() { mEmpty = true; }
	bool isUsed() const { return !mEmpty; }

	const Weapon& getWeapon() const { return mWeapon; }
	Weapon& getWeapon() { return mWeapon; }

	void update(float _deltaTime);

protected:
	int mIndex = 0;
	btVector3 mPosition;
	float mRoll = 0.f;

	bool mEmpty = true;

	Weapon mWeapon;
};

#endif //_HARD_POINT_H_