#ifndef _SECTOR_VIEW_H_
#define _SECTOR_VIEW_H_

#include <string>

namespace Ogre
{
	class SceneManager;
}

class SectorView
{
public:
	SectorView(Ogre::SceneManager* _sceneManager)
		: mSceneManager(_sceneManager)
	{}

	~SectorView();

	void createView(const std::string& _sectorName);

protected:
	//Used for cleaning
	Ogre::SceneManager* mSceneManager = nullptr;
};

#endif //_SECTOR_VIEW_H_