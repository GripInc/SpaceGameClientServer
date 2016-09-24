#ifndef _STATION_VIEW_H_
#define _STATION_VIEW_H_

#include <string>

class ScreenSettings;

namespace Ogre
{
	class SceneManager;
}

class StationView
{
public:
	~StationView();

	void init(Ogre::SceneManager* _sceneManager);
	void createView(const ScreenSettings& _screen);

protected:
	Ogre::SceneManager* mSceneManager = nullptr;
};

#endif //_STATION_VIEW_H_
