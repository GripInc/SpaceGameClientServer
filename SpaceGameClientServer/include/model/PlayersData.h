#ifndef _PLAYERS_DATA_H_
#define _PLAYERS_DATA_H_

#include "model/PlayerData.h"

/** Deserialize game files */
class PlayersData
{
public:
	///Singleton
	static PlayersData& getInstance();

	bool init(const std::string& _playerDataFilePath);

	PlayerData* getPlayerData(const std::string& _playerId);

protected:
	///Singleton
	static PlayersData* mInstance;
	PlayersData() {}

	//Players data
	std::map<std::string, PlayerData> mPlayersDataMap;

	//The players data file path
	 std::string mPlayerDataFilePath;
};

#endif //_PLAYERS_DATA_H_
