#ifndef _SECTOR_CONTROLLER_H_
#define _SECTOR_CONTROLLER_H_

#include <string>

#include "SpaceGameTypes.h"

class SectorView;
class Sector;

namespace Ogre
{
	class SceneManager;
}

class ISector;

class SectorController
{
public:
	
	void createSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate);

	//DEBUG
	std::string mLastShotTarget;
	std::string mLastCollidedPart0;
	std::string mLastCollidedPart1;
	float mLastCollisionSpeed = 0.f;

	virtual void switchDisplayDebug() = 0;
	virtual void switchDisplay() = 0;

protected:
	SectorView* mSectorView = nullptr;
	ISector* mCurrentSector = nullptr;

	SectorTick mSectorTick = 0;

	virtual void initSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate) = 0;
	virtual void instanciateSectorObjects() = 0;

	//void instanciateSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate, unsigned int _maxSectorTickRewindAmount);
	//void instanciateSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate, SectorTick _startingSectorTick);
};

#endif //_SECTOR_CONTROLLER_H_