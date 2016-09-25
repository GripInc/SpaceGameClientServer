#ifndef _APPLICATION_MASTER_H_
#define _APPLICATION_MASTER_H_

#include "Ogre.h"
#	ifdef _GAME_CLIENT
#include "controller/ClientGameController.h"
#	else
#include "controller/ServerGameController.h"
#	endif

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
	
#	ifdef _GAME_CLIENT
	ClientGameController mGameController;
#	else
	ServerGameController mGameController;
#	endif
};

#endif // #ifndef _APPLICATION_MASTER_H_