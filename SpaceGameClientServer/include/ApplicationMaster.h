#ifndef _APPLICATION_MASTER_H_
#define _APPLICATION_MASTER_H_

#include "Ogre.h"
#include "controller/GameController.h"
#include "network/NetworkLayer.h"

#define GAME_SETTINGS_FILE_PATH "../SpaceGameRessources/"
#define PLAYERS_DATA_FILE_PATH "../playersData/playersData.json"

class ApplicationMaster
{
public:
    ApplicationMaster();
    virtual ~ApplicationMaster();

	void startApplication();

protected:
	bool setup();
    bool configure(void);
    void setupResources(void);
    void loadResources(void);

    Ogre::Root* mRoot = nullptr;
    Ogre::RenderWindow* mWindow = nullptr;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

	bool mShutDown = false;

    NetworkLayer mNetworkLayer;
	
	GameController mGameController;
};

#endif // #ifndef _APPLICATION_MASTER_H_