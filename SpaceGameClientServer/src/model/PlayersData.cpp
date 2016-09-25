#include <iostream>
#include <fstream>

#include "model/PlayersData.h"

///Singleton
PlayersData* PlayersData::mInstance;

///Singleton
PlayersData& PlayersData::getInstance()
{
	if(!mInstance)
		mInstance = new PlayersData();

	return *mInstance;
}

bool PlayersData::init(const std::string& _playerDataFilePath)
{
	mPlayerDataFilePath = _playerDataFilePath;

	//Read file
	std::string playersFileContent;
	std::string line;
	std::fstream fileStream(_playerDataFilePath, std::ios::in);
	while(getline(fileStream, line))
		playersFileContent += line;
	fileStream.close();

	//Fill playersData
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(playersFileContent, root);

	if(parsingSuccessful)
	{
		const Json::Value players = root["players"];
		for (unsigned int i = 0; i < players.size(); ++i)  // Iterates over the sequence elements.
		{
			PlayerData playerData;
			parsingSuccessful = parsingSuccessful && playerData.deserializeFromJsonNode(players[i]);
			mPlayersDataMap.insert(std::pair<std::string, PlayerData>(playerData.mPlayerId, playerData));
		}
	}

	return parsingSuccessful;
}

PlayerData* PlayersData::getPlayerData(const std::string& _playerId)
{
	std::map<std::string, PlayerData>::iterator foundPlayer = mPlayersDataMap.find(_playerId);
	if(foundPlayer != mPlayersDataMap.end())
	{
		return &(*foundPlayer).second;
	}

	//TODO log error
	return NULL;
}