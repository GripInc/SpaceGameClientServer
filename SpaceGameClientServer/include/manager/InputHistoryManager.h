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
		mCurrentSectorTick = 0;
	}

	//Add one tick input
	void addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _newClientInput);

	void getInput(SectorTick _tick, ClientsInputMap& _outClientsInputMap) const;

	//Maintenance operations
	void update(SectorTick _sectorTick);

protected:
	//Do not modify it once setted
	unsigned int mMaxRewindAmount = 0;
	std::map<SectorTick, ClientsInputMap> mClientsInputByTick;
	SectorTick mCurrentSectorTick = 0;
};

#endif //_INPUT_HISTORY_MANAGER_H_
