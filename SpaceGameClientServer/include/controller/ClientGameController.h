#ifndef _CLIENT_GAME_CONTROLLER_H_
#define _CLIENT_GAME_CONTROLLER_H_

#include "controller/GameController.h"
#include "controller/ShipInputHandler.h"

#include "model/PlayerData.h"

#include "network/IConnectionReadyListener.h"

#include "OgreFrameListener.h"
#include "OgreVector3.h"

#include <string>

class StationController;
class ClientSectorController;

namespace RakNet
{
	class RakString;
}

namespace Ogre
{
	class RenderWindow;
	class Root;
}

class ClientGameController : public GameController, public IConnectionReadyListener
{
public:

	///Initialize game controller
	virtual void init(const std::string& _gameSettingsFilePath, Ogre::Root* _root, Ogre::RenderWindow* _renderWindow, Ogre::SceneManager* _sceneManager, NetworkLayer& _networkLayer) override;

	///Start a game with player data from server
	void startGame(const RakNet::RakString& _playerData);

	///Prepare switch
	void prepareSwitchToStationMode(const std::string& _stationName);
	void prepareSwitchToInSpaceMode(const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, const std::string& _sectorName, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, SectorTick _sectorTick);

	///Connection ready notification
	virtual void notifyIsConnected(bool _value);

	///Received sector update from server
	void receivedSectorState(RakNet::BitStream& _data) const;

protected:
	
	ClientSectorController* mSectorController;

	class SwitchToSpaceData
	{
	public:
		SwitchToSpaceData()
			: mPosition(Ogre::Vector3(0.f, 0.f, 0.f)),
			mOrientation(Ogre::Quaternion::IDENTITY)
		{}

		Ogre::Vector3 mPosition;
		Ogre::Quaternion mOrientation;
		std::string mSectorName;
		UniqueId mUniqueId = 0;
		RakNet::RakNetGUID mRakNetGUID;
		SectorTick mSectorTick = 0;
	};

	class SwitchToStationData
	{
	public:
		std::string mStationName;
	};

	SwitchToSpaceData* mSwitchToSpaceData = nullptr;
	SwitchToStationData* mSwitchToStationData = nullptr;

	///Switch from space to station mode
	void switchToStationMode(const std::string& _stationName);
	///Switch from station to space mode
	void switchToInSpaceMode(const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, const std::string& _sectorName, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, SectorTick _sectorTick);

	StationController* mStationController = nullptr;
	ShipInputHandler mShipInputHandler;

	PlayerData mPlayerData;

	//Specialized functions (client or server) used in frameRenderingQueued
	virtual void processNetworkBuffer() override;
	virtual void updateSector() override;
	virtual void updateSectorView() override;
	virtual void updateDebugPanel(Ogre::Real _timeSinceLastFrame) override;
	virtual void handleSwitching() override;

	virtual bool keyPressed(const OIS::KeyEvent &arg) override;
	virtual bool keyReleased(const OIS::KeyEvent &arg) override;
};

#endif //_CLIENT_GAME_CONTROLLER_H_