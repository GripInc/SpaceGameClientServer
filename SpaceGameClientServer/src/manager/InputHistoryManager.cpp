#include "manager/InputHistoryManager.h"

#include "utils/StringUtils.h"
#include "manager/LoggerManager.h"

#include <assert.h>

namespace
{
	const std::string LOG_CLASS_TAG = "InputHistoryManager";
}

//Add one tick input
void InputHistoryManager::addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _clientInput)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "client GUID is : " + std::string(_id.ToString()) + "; _tick is : " + StringUtils::toStr(_tick), false);

	//Drop to old input
	if(_tick < mCurrentSectorTick - mMaxRewindAmount)
	{
		LoggerManager::getInstance().logW(LOG_CLASS_TAG, "addInput", "Drop input because it is too old");
		return;
	}

	if(mClientsInputByTick.empty()) //First time special case
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "First input ever added.", false);

		mClientsInputByTick[_tick][_id] = _clientInput;
	}
	else
	{
		std::map<SectorTick, ClientsInputMap>::iterator foundTick = mClientsInputByTick.find(_tick);
		if(foundTick != mClientsInputByTick.end())
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "Add input in existing map<GUID, InputState> for the tick " + StringUtils::toStr(_tick), false);

			ClientsInputMap& clientsInput = (*foundTick).second;
			clientsInput[_id] = _clientInput;
		}
		else
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "Creating and adding input in a new map<GUID, InputState> for the tick " + StringUtils::toStr(_tick), false);
			mClientsInputByTick[_tick][_id] = _clientInput;
		}
	}

	if(_tick < mOldestUnsimulatedTick)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "Setting mOldestUnsimulatedTick to _tick; mOldestUnsimulatedTick was : " + StringUtils::toStr(mOldestUnsimulatedTick), false);
		mOldestUnsimulatedTick = _tick;
	}
}

void InputHistoryManager::updateStartingTickIfNeeded(SectorTick _currentSectorTick)
{
	/*if(mStartingTick + mClientsInputByTick.size() - _currentSectorTick <= 1)
	{
		//We shift every element to left of half of history size
		size_t vectorSize = mClientsInputByTick.size();
		for(size_t i = mMaxRewindAmount; i < vectorSize; ++i)
		{
			mClientsInputByTick[i - mMaxRewindAmount] = mClientsInputByTick[i];
		}

		//Complete second half of history with copies of the last entry
		const std::map<RakNet::RakNetGUID, std::pair<InputState, bool> >& lastEntryToCopy = mClientsInputByTick[mMaxRewindAmount - 1];
		for(size_t i = mMaxRewindAmount; i < vectorSize; ++i)
		{
			mClientsInputByTick[i] = lastEntryToCopy;
		}

		mStartingTick += mMaxRewindAmount;

		if(mStartingTick > mOldestUnsimulatedTick)
		{
			mOldestUnsimulatedTick = mStartingTick;
			//TODO maybe log that the real oldest tick was dropped?
		}
	}*/
}

//Look backward through all input until each client has one, from _fromTick
void InputHistoryManager::getLastInputForAllClients(SectorTick _fromTick, ClientsInputMap& _outClientsInputMap) const
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "getLastInputForAllClients", "_fromTick is : " + StringUtils::toStr(_fromTick), false);

	if(mClientsInputByTick.empty())
	{
		LoggerManager::getInstance().logW(LOG_CLASS_TAG, "getLastInputForAllClients", "Trying to get last inputs while mClientsInputByTick is empty");
		return;
	}

	SectorTick oldestTick = (*mClientsInputByTick.begin()).first;
	if(_fromTick < oldestTick)
	{
		//No entry available
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "getLastInputForAllClients", "Trying to get last inputs from _fromTick while it is lower than mClientsInputByTick first recorded tick");
		assert(false);
	}

	//Find the last acceptable entry (more recent entries are not acceptable, that's why we don't use reverse iterator from end)
	std::map<SectorTick, ClientsInputMap>::const_iterator foundTick;
	do
	{
		foundTick = mClientsInputByTick.find(_fromTick);
		--_fromTick;
	}
	while(foundTick == mClientsInputByTick.end()); //Avoid infinite loop with condition _tick < oldestTick above

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "getLastInputForAllClients", "Found last acceptable entry at tick : " + StringUtils::toStr((*foundTick).first), false);

	//Used to log output
	std::string _outClientsInputMapToString = "";

	//First filling with first found tick history.
	//For all clients UIDs existing at first found tick, add it to map.
	ClientsInputMap::const_iterator clientsInputMapIt = (*foundTick).second.begin();
	const ClientsInputMap::const_iterator clientsInputMapItEnd = (*foundTick).second.end();
	for(; clientsInputMapIt != clientsInputMapItEnd; ++clientsInputMapIt)
	{
		_outClientsInputMap[(*clientsInputMapIt).first] = (*clientsInputMapIt).second;
		
		//Logging
		_outClientsInputMapToString += std::string((*clientsInputMapIt).first.ToString()) + ":" + StringUtils::toStr((*foundTick).first) + ";";
	}

	//Go through inputs backward. Each time a client that is not in output is found we add it.
	std::map<SectorTick, ClientsInputMap>::const_reverse_iterator clientsInputByTickIt(foundTick); //Converting it to reverse_it point to the next element in reverse view it = 5 -> reverseIt = 4
	const std::map<SectorTick, ClientsInputMap>::const_reverse_iterator clientsInputByTickItEnd = mClientsInputByTick.rend();
	for(; clientsInputByTickIt != clientsInputByTickItEnd; ++clientsInputByTickIt)
	{
		ClientsInputMap::const_iterator clientsInputMapIt = (*clientsInputByTickIt).second.begin();
		const ClientsInputMap::const_iterator clientsInputMapItEnd = (*clientsInputByTickIt).second.end();
		for(; clientsInputMapIt != clientsInputMapItEnd; ++clientsInputMapIt)
		{
			const ClientsInputMap::const_iterator foundId = _outClientsInputMap.find((*clientsInputMapIt).first);
			if(foundId == _outClientsInputMap.end())
			{
				_outClientsInputMap[(*clientsInputMapIt).first] = (*clientsInputMapIt).second;

				//Logging
				_outClientsInputMapToString += std::string((*clientsInputMapIt).first.ToString()) + ":" + StringUtils::toStr((*clientsInputByTickIt).first) + ";";
			}
		}
	}

	//Logging output
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "getLastInputForAllClients", "_outClientsInputMap is (GUID:SectorTick) : " + _outClientsInputMapToString, false);
}

//Return true if an input was inserted since last update() call
bool InputHistoryManager::getOldestUnsimulatedInput(ClientsInputMap& _outClientsInputMap, SectorTick& _outTick) const
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "getOldestUnsimulatedInput", "mOldestUnsimulatedTick is : " + StringUtils::toStr(mOldestUnsimulatedTick), false);

	//Unconsistent situation
	if(mClientsInputByTick.empty())
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "getOldestUnsimulatedInput", "Trying to get oldest input while mClientsInputByTick is empty. Returning now.", false);
		return false;
	}

	_outTick = mOldestUnsimulatedTick;
	getLastInputForAllClients(_outTick, _outClientsInputMap);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "getOldestUnsimulatedInput", "mCurrentSectorTick is : " + StringUtils::toStr(mCurrentSectorTick) + "; returning " + (mOldestUnsimulatedTick < mCurrentSectorTick ? "true" : "false"), false);

	return mOldestUnsimulatedTick < mCurrentSectorTick;
}

void InputHistoryManager::update(SectorTick _sectorTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "update", "BEFORE update : mCurrentSectorTick is : " + StringUtils::toStr(mCurrentSectorTick) + "; mOldestUnsimulatedTick is : " + StringUtils::toStr(mOldestUnsimulatedTick), false);

	mCurrentSectorTick = _sectorTick;
	mOldestUnsimulatedTick = mCurrentSectorTick;

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "update", "AFTER update : mCurrentSectorTick is : " + StringUtils::toStr(mCurrentSectorTick) + "; mOldestUnsimulatedTick is : " + StringUtils::toStr(mOldestUnsimulatedTick), false);

	if(mClientsInputByTick.empty())
		return;

	//TODO clean up history
}