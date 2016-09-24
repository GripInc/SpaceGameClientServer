#include "model/GameSettings.h"

#include "utils/StringUtils.h"

#include <tinyxml2.h>
#include "utils/XMLHelper.h"

///Singleton
GameSettings* GameSettings::mInstance;

///Singleton
GameSettings& GameSettings::getInstance()
{
	if(!mInstance)
		mInstance = new GameSettings();

	return *mInstance;
}

void GameSettings::init(const std::string& _gameSettingsFilePath)
{
	mSectorsFilePath = _gameSettingsFilePath + "sectors.xml";
	deserializeSectors();
	
	mShipsFilePath = _gameSettingsFilePath + "ships.xml";
	deserializeShips();

	mEnginesFilePath = _gameSettingsFilePath + "engines.xml";
	deserializeEngines();
	
	mShotsFilePath = _gameSettingsFilePath + "shots.xml";
	deserializeShots();
	
	mWeaponsFilePath = _gameSettingsFilePath + "weapons.xml";
	deserializeWeapons();

	mReactorsFilePath = _gameSettingsFilePath + "reactors.xml";
	deserializeReactors();

	mAccumulatorsFilePath = _gameSettingsFilePath + "accumulators.xml";
	deserializeAccumulators();

	mShieldsFilePath = _gameSettingsFilePath + "shields.xml";
	deserializeShields();

	mPostCombustionsFilePath = _gameSettingsFilePath + "post_combustions.xml";
	deserializePostCombustions();

	mDirectionalsFilePath = _gameSettingsFilePath + "directionals.xml";
	deserializeDirectionals();
		
	mStationsFilePath = _gameSettingsFilePath + "stations.xml";
	deserializeStations();

	mStationScreensFilePath = _gameSettingsFilePath + "station_screens.xml";
	deserializeStationScreens();
}

///Sectors
bool GameSettings::deserializeSectors()
{
	tinyxml2::XMLDocument sectorsXML;
	sectorsXML.LoadFile(mSectorsFilePath.c_str());
	
	return mSectorsSettings.deserialize(sectorsXML.RootElement());
}

const SectorSettings* GameSettings::getSector(const std::string& _sectorName)
{
	return mSectorsSettings.getSectorSettings(_sectorName);
}

///Ships
bool GameSettings::deserializeShips()
{
	tinyxml2::XMLDocument shipsXML;
	shipsXML.LoadFile(mShipsFilePath.c_str());
	
	return mShipsSettings.deserialize(shipsXML.RootElement());
}

const ShipSettings* GameSettings::getShip(const std::string& _shipName)
{
	return mShipsSettings.getShipSettings(_shipName);
}

///Engines
bool GameSettings::deserializeEngines()
{
	tinyxml2::XMLDocument enginesXML;
	enginesXML.LoadFile(mEnginesFilePath.c_str());
	
	return mEnginesSettings.deserialize(enginesXML.RootElement());
}

const EngineSettings* GameSettings::getEngine(const std::string& _engineName)
{
	return mEnginesSettings.getEngineSettings(_engineName);
}

//Reactors
bool GameSettings::deserializeReactors()
{
	tinyxml2::XMLDocument reactorsXML;
	reactorsXML.LoadFile(mReactorsFilePath.c_str());
	
	return mReactorsSettings.deserialize(reactorsXML.RootElement());
}

const ReactorSettings* GameSettings::getReactor(const std::string& _reactorName)
{
	return mReactorsSettings.getReactorSettings(_reactorName);
}

///Shots
bool GameSettings::deserializeShots()
{
	tinyxml2::XMLDocument shotsXML;
	shotsXML.LoadFile(mShotsFilePath.c_str());
	
	return mShotsSettings.deserialize(shotsXML.RootElement());
}

const ShotSettings* GameSettings::getShot(const std::string& _shotName)
{
	return mShotsSettings.getShotSettings(_shotName);
}

//Weapons
bool GameSettings::deserializeWeapons()
{
	tinyxml2::XMLDocument weaponsXML;
	weaponsXML.LoadFile(mWeaponsFilePath.c_str());
	
	return mWeaponsSettings.deserialize(weaponsXML.RootElement());
}

const WeaponSettings* GameSettings::getWeapon(const std::string& _weaponName)
{
	return mWeaponsSettings.getWeaponSettings(_weaponName);
}

//Accumulators
bool GameSettings::deserializeAccumulators()
{
	tinyxml2::XMLDocument accumulatorsXML;
	accumulatorsXML.LoadFile(mAccumulatorsFilePath.c_str());
	
	return mAccumulatorsSettings.deserialize(accumulatorsXML.RootElement());
}

const AccumulatorSettings* GameSettings::getAccumulator(const std::string& _accumulatorName)
{
	return mAccumulatorsSettings.getAccumulatorSettings(_accumulatorName);
}

//Shields
bool GameSettings::deserializeShields()
{
	tinyxml2::XMLDocument shieldsXML;
	shieldsXML.LoadFile(mShieldsFilePath.c_str());
	
	return mShieldsSettings.deserialize(shieldsXML.RootElement());
}

const ShieldSettings* GameSettings::getShield(const std::string& _shieldName)
{
	return mShieldsSettings.getShieldSettings(_shieldName);
}

//PostCombustions
bool GameSettings::deserializePostCombustions()
{
	tinyxml2::XMLDocument postCombustionsXML;
	postCombustionsXML.LoadFile(mPostCombustionsFilePath.c_str());
	
	return mPostCombustionsSettings.deserialize(postCombustionsXML.RootElement());
}

const PostCombustionSettings* GameSettings::getPostCombustion(const std::string& _postCombustion)
{
	return mPostCombustionsSettings.getPostCombustionSettings(_postCombustion);
}

//Directionals
bool GameSettings::deserializeDirectionals()
{
	tinyxml2::XMLDocument directionalsXML;
	directionalsXML.LoadFile(mDirectionalsFilePath.c_str());
	
	return mDirectionalsSettings.deserialize(directionalsXML.RootElement());
}

const DirectionalSettings* GameSettings::getDirectional(const std::string& _directional)
{
	return mDirectionalsSettings.getDirectionalSettings(_directional);
}

//Stations
bool GameSettings::deserializeStations()
{
	tinyxml2::XMLDocument stationsXML;
	stationsXML.LoadFile(mStationsFilePath.c_str());
	
	return mStationsSettings.deserialize(stationsXML.RootElement());
}

const StationSettings* GameSettings::getStation(const std::string& _station)
{
	return mStationsSettings.getStationSettings(_station);
}

//StationScreens
bool GameSettings::deserializeStationScreens()
{
	tinyxml2::XMLDocument stationScreensXML;
	stationScreensXML.LoadFile(mStationScreensFilePath.c_str());
	
	return mStationScreensSettings.deserialize(stationScreensXML.RootElement());
}

const StationScreenSettings* GameSettings::getStationScreen(const std::string& _stationScreen)
{
	return mStationScreensSettings.getStationScreenSettings(_stationScreen);
}
