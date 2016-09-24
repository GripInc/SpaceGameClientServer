#ifndef UIController_H
#define UIController_H

#include <OISMouse.h>
#include <OISKeyboard.h>

#include "SdkTrays.h"

class GameController;

class UIController : public OgreBites::SdkTrayListener, public OIS::KeyListener, public OIS::MouseListener
{
public:

	UIController(GameController* _gameController, Ogre::RenderWindow* _renderWindow, OIS::Mouse* _mouse);

	~UIController();

	OgreBites::ParamsPanel* getDebugPanel() { return mDebugPanel; }

protected:
	GameController* mGameController = nullptr;

	OgreBites::SdkTrayManager* mTrayManager = nullptr;

	//DEBUG PANEL
	OgreBites::ParamsPanel* mDebugPanel = nullptr;

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
};

#endif //UIController_H