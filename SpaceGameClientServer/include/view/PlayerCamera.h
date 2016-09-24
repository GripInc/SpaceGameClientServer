#ifndef _PLAYER_CAMERA_H_
#define _PLAYER_CAMERA_H_

#include <string>

namespace Ogre
{
	class SceneManager;
	class Camera;
	class SceneNode;
	class RenderWindow;
}

class PlayerCamera
{
public:
	///Singleton
	static PlayerCamera& getInstance();

	void init(Ogre::SceneManager* _sceneManager, Ogre::RenderWindow* _renderWindow);
	void createCamera(const std::string& _name);

	Ogre::SceneNode* getCameraNode() const { return mCameraNode; }

protected:
	///Singleton
	static PlayerCamera* mInstance;
	PlayerCamera() {}

	Ogre::RenderWindow* mRenderWindow = nullptr;
	Ogre::SceneManager* mSceneManager = nullptr;
	Ogre::Camera* mCamera = nullptr;
	Ogre::SceneNode* mCameraNode = nullptr;
};

#endif //_PLAYER_CAMERA_H_