#ifndef _GAME_SETTINGS_H_
#define _GAME_SETTINGS_H_

#include "model/deserialized/SectorsSettings.h"
#include "model/deserialized/ShipsSettings.h"
#include "model/deserialized/EnginesSettings.h"
#include "model/deserialized/ShotsSettings.h"
#include "model/deserialized/WeaponsSettings.h"
#include "model/deserialized/ReactorsSettings.h"
#include "model/deserialized/AccumulatorsSettings.h"
#include "model/deserialized/ShieldsSettings.h"
#include "model/deserialized/PostCombustionsSettings.h"
#include "model/deserialized/DirectionalsSettings.h"
#include "model/deserialized/StationsSettings.h"
#include "model/deserialized/StationScreensSettings.h"

/** Deserialize game files */
class GameSettings
{
public:
	///Singleton
	static GameSettings& getInstance();

	void init(const std::string& _gameSettingsFilePath);

	const SectorSettings* GameSettings::getSector(const std::string& _sectorName);
	const ShipSettings* getShip(const std::string& _shipName);
	const EngineSettings* getEngine(const std::string& _EngineName);
	const ShotSettings* getShot(const std::string& _shotName);
	const WeaponSettings* getWeapon(const std::string& _weaponName);
	const ReactorSettings* getReactor(const std::string& _reactorName);
	const AccumulatorSettings* getAccumulator(const std::string& _accumulatorName);
	const ShieldSettings* getShield(const std::string& _shieldName);
	const PostCombustionSettings* getPostCombustion(const std::string& _postCombustion);
	const DirectionalSettings* getDirectional(const std::string& _directional);
	const StationSettings* getStation(const std::string& _station);
	const StationScreenSettings* getStationScreen(const std::string& _stationScreen);

protected:
	///Singleton
	static GameSettings* mInstance;
	GameSettings() {}

	//Ships
	std::string mShipsFilePath;
	bool deserializeShips();
	ShipsSettings mShipsSettings;

	//Engines
	std::string mEnginesFilePath;
	bool deserializeEngines();
	EnginesSettings mEnginesSettings;

	//Reactors
	std::string mReactorsFilePath;
	bool deserializeReactors();
	ReactorsSettings mReactorsSettings;

	//Sectors
	std::string mSectorsFilePath;
	bool deserializeSectors();
	SectorsSettings mSectorsSettings;

	//Shots
	std::string mShotsFilePath;
	bool deserializeShots();
	ShotsSettings mShotsSettings;

	//Weapons
	std::string mWeaponsFilePath;
	bool deserializeWeapons();
	WeaponsSettings mWeaponsSettings;

	//Accumulators
	std::string mAccumulatorsFilePath;
	bool deserializeAccumulators();
	AccumulatorsSettings mAccumulatorsSettings;

	//Shields
	std::string mShieldsFilePath;
	bool deserializeShields();
	ShieldsSettings mShieldsSettings;

	//PostCombustions
	std::string mPostCombustionsFilePath;
	bool deserializePostCombustions();
	PostCombustionsSettings mPostCombustionsSettings;

	//Directionals
	std::string mDirectionalsFilePath;
	bool deserializeDirectionals();
	DirectionalsSettings mDirectionalsSettings;

	//Stations
	std::string mStationsFilePath;
	bool deserializeStations();
	StationsSettings mStationsSettings;

	//StationScreens
	std::string mStationScreensFilePath;
	bool deserializeStationScreens();
	StationScreensSettings mStationScreensSettings;
};

#endif //_GAME_SETTINGS_H_
