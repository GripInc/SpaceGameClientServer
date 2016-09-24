#ifndef _STATION_CONTROLLER_H_
#define _STATION_CONTROLLER_H_

#include <OISMouse.h>
#include <OISKeyboard.h>

#include <SdkTrays.h>

class GameController;
class StationSettings;
class ScreenSettings;
class StationView;
class StationScreenSettings;

class StationController : public OgreBites::SdkTrayListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
	static const std::string TARGET_SPECIAL_LAUNCH;

	StationController(GameController* _gameController)
		: mGameController(_gameController)
	{}

	~StationController();

	//Init
	void init(const StationSettings& _stationSettings, Ogre::RenderWindow* _renderWindow, OIS::Mouse* _mouse, Ogre::SceneManager* _sceneManager);
	//Clean
	void clean();

protected:
	GameController* mGameController = nullptr;

	OgreBites::SdkTrayManager* mTrayManager = nullptr;
	OgreBites::Label* mNextScreenLabel = nullptr;

	//UI elements
	//TODO
	const ScreenSettings* mCurrentScreen = nullptr;
	const StationScreenSettings* mStationScreenSettings = nullptr;
	const StationSettings* mStationSettings = nullptr;

	//View
	StationView* mStationView = nullptr;

	//OIS
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);

	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	//virtual ~SdkTrayListener() {}
	virtual void buttonHit(OgreBites::Button* button) {}
	virtual void itemSelected(OgreBites::SelectMenu* menu) {}
	virtual void labelHit(OgreBites::Label* label) {}
	virtual void sliderMoved(OgreBites::Slider* slider) {}
	virtual void checkBoxToggled(OgreBites::CheckBox* box) {}
	virtual void okDialogClosed(const Ogre::DisplayString& message) {}
	virtual void yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit) {}

	//Navigation
	void gotoScreen(const std::string& _screen);
	bool isScreenAvailable(const std::string& _screen);
};

#endif //_STATION_CONTROLLER_H_