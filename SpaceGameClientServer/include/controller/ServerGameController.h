#ifndef _SERVER_GAME_CONTROLLER_H_
#define _SERVER_GAME_CONTROLLER_H_

#include "controller/GameController.h"
#include "model/InputState.h"

#include "RakNetTypes.h"
#include "SpaceGameTypes.h"

class ServerSectorController;
class CameraController;
class PlayerData;

class ServerGameController : public GameController
{
public:

	///Initialize game controller
	void init(const std::string& _playerDataFilePath, const std::string& _gameSettingsFilePath, Ogre::Root* _root, Ogre::RenderWindow* _renderWindow, Ogre::SceneManager* _sceneManager, NetworkLayer& _networkLayer);
	
	///Start game
	void startGame();

	void addPlayer(const RakNet::RakNetGUID& _id, PlayerData* _playerData);
	const PlayerData* getPlayerData(const RakNet::RakNetGUID& _clientId) const;
	PlayerData* getPlayerData(const RakNet::RakNetGUID& _clientId);

	void instantiateClientShip(const RakNet::RakNetGUID& _clientId, std::string& _outSector, Ogre::Vector3& _outPosition, Ogre::Quaternion& _outOrientation, UniqueId& _shipUniqueId, SectorTick& _sectorTick);

	//Add input for a client in a sector
	void addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _clientInput);

protected:
	///The sector controller
	ServerSectorController* mSectorController;

	///Basic camera controller.
	CameraController* mCameraController = nullptr;

	//Specialized functions (client or server) used in frameRenderingQueued
	virtual void processNetworkBuffer() override;
	virtual void updateSector() override;
	virtual void updateDebugPanel(Ogre::Real _timeSinceLastFrame) override;

	//Connected players
	std::map<RakNet::RakNetGUID, PlayerData*> mConnectedPlayers;

	virtual bool keyPressed(const OIS::KeyEvent &arg) override;
	virtual bool keyReleased(const OIS::KeyEvent &arg) override;
};

#endif //_SERVER_GAME_CONTROLLER_H_