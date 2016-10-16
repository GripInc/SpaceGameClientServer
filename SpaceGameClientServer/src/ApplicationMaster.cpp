#include "ApplicationMaster.h"
#include "BitStream.h"
#include "OgreOverlaySystem.h"

#include "utils/StringUtils.h"

ApplicationMaster::ApplicationMaster(void)
    : mRoot(0),
    mWindow(0),
    mResourcesCfg(""),
    mPluginsCfg(""),
    mShutDown(false)
{
}

ApplicationMaster::~ApplicationMaster(void)
{
    delete mRoot;
}

bool ApplicationMaster::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "Space Game Render Window");
		mWindow->setDeactivateOnFocusChange(false);
        return true;
    }
    else
    {
        return false;
    }
}

bool ApplicationMaster::setup(void)
{
    setupResources();

    bool carryOn = configure();
    if (!carryOn)
		return false;

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Load resources
    loadResources();

	return true;
};

/// Ressources ///
void ApplicationMaster::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile configFile;
    configFile.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator sectionIterator = configFile.getSectionIterator();

    Ogre::String sectionName, typeName, archetypeName;
    while (sectionIterator.hasMoreElements())
    {
        sectionName = sectionIterator.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = sectionIterator.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archetypeName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archetypeName, typeName, sectionName);
        }
    }
}

void ApplicationMaster::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
/// Ressources ///

void ApplicationMaster::startApplication()
{
#ifdef _DEBUG
    mResourcesCfg = "../resources_d.cfg";
    mPluginsCfg = "../plugins_d.cfg";
#else
    mResourcesCfg = "../resources.cfg";
    mPluginsCfg = "../plugins.cfg";
#endif

	mRoot = new Ogre::Root(mPluginsCfg);

	// Create the SceneManager
	Ogre::SceneManager* sceneManager = mRoot->createSceneManager(Ogre::ST_EXTERIOR_REAL_FAR);

	//Init overlay component
	Ogre::OverlaySystem* overlaySystem = new Ogre::OverlaySystem();
	sceneManager->addRenderQueueListener(overlaySystem);

	if (!setup())
		return;

	//Read network config
	std::map<std::string, std::string> networkConfig;
	std::ifstream configFile("../SpaceGameRessources/network_conf.txt");

	while (configFile)
	{
		std::string lineString;

		if (!getline(configFile, lineString))
			break;

		std::vector<std::string> splitedString = StringUtils::split(lineString, ';');

		if(splitedString.size() >= 2)
			networkConfig[splitedString[0]] = splitedString[1];
	}

	//Init network
	unsigned short serverPort;
	StringUtils::fromString(serverPort, networkConfig["port"]);
	unsigned int maxConnections;
	StringUtils::fromString(maxConnections, networkConfig["max_clients"]);

	mNetworkLayer.init(serverPort, maxConnections);

#	ifdef _GAME_CLIENT
	//Init gameController
	mGameController.init(GAME_SETTINGS_FILE_PATH, mRoot, mWindow, sceneManager, mNetworkLayer);
	mNetworkLayer.connect(networkConfig["ip"].c_str(), serverPort);
#	else
	//Init gameController
	mGameController.init(PLAYERS_DATA_FILE_PATH, GAME_SETTINGS_FILE_PATH, mRoot, mWindow, sceneManager, mNetworkLayer);
	mGameController.startGame();
#	endif
	
    mRoot->startRendering();
}
