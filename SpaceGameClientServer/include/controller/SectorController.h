#ifndef _SECTOR_CONTROLLER_H_
#define _SECTOR_CONTROLLER_H_

#include <string>

class SectorView;
class Sector;

class SectorController
{
public:
	
	//DEBUG
	std::string mLastShotTarget;
	std::string mLastCollidedPart0;
	std::string mLastCollidedPart1;
	float mLastCollisionSpeed = 0.f;

	virtual void switchDisplayDebug() = 0;
	virtual void switchDisplay() = 0;

protected:
	SectorView* mSectorView = nullptr;
};

#endif //_SECTOR_CONTROLLER_H_