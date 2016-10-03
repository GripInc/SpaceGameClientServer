#ifndef _INPUT_HISTORY_MANAGER_H_
#define _INPUT_HISTORY_MANAGER_H_

#include <map>
#include <vector>

#include "RakNetTypes.h"
#include "SpaceGameTypes.h"
#include "model/InputState.h"

typedef std::map<RakNet::RakNetGUID, InputState> ClientsInputMap;

class InputHistoryManager
{
public:
	void init(unsigned int _maxRewindAmount)
	{
		mMaxRewindAmount = _maxRewindAmount;
		mOldestUnsimulatedTick = 0;
		mCurrentSectorTick = 0;
	}

	//Add one tick input
	void addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _clientInput);

	void updateStartingTickIfNeeded(SectorTick _currentSectorTick);

	void getLastInputForAllClients(SectorTick _tick, ClientsInputMap& _outClientsInputMap) const;

	//Return true if an input was inserted since last update() call
	bool getOldestUnsimulatedInput(ClientsInputMap& _outClientsInputMap, SectorTick& _outTick) const;

	//Maintenance operations
	void update(SectorTick _sectorTick);

protected:
	//Do not modify it once setted
	unsigned int mMaxRewindAmount = 0;
	std::map<SectorTick, ClientsInputMap> mClientsInputByTick;
	SectorTick mOldestUnsimulatedTick;
	SectorTick mCurrentSectorTick;
};

#endif //_INPUT_HISTORY_MANAGER_H_