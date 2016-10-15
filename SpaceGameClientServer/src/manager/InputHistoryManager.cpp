#include "manager/InputHistoryManager.h"

#include "utils/StringUtils.h"
#include "manager/LoggerManager.h"

#include <assert.h>

namespace
{
	const std::string LOG_CLASS_TAG = "InputHistoryManager";
}

//Add one tick input
void InputHistoryManager::addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _newClientInput)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "client GUID is : " + std::string(_id.ToString()) + "; _tick is : " + StringUtils::toStr(_tick), false);

	//Drop too old input
	if (_tick < mCurrentSectorTick - mMaxRewindAmount)
	{
		LoggerManager::getInstance().logW(LOG_CLASS_TAG, "addInput", "Drop input because it is too old");
		return;
	}

	//Add the input and set it to certified
	ClientsInputMap* clientsInputMap = &mClientsInputByTick[_tick];
	InputState* inputState = &(*clientsInputMap)[_id];
	*inputState = _newClientInput;
	inputState->mCertified = true;

	//Packets are unordered
	//Each time we add an input it is tagged as "certified", and we fill the blanks with a copy of the input until current tick or a more recent certified input
	for (_tick += 1; _tick <= mCurrentSectorTick; ++_tick)
	{
		clientsInputMap = &mClientsInputByTick[_tick];

		inputState = &(*clientsInputMap)[_id];
		if (inputState->mCertified)
			break;
		else
		{
			*inputState = _newClientInput;
			inputState->mCertified = false;
		}
	}
}

void InputHistoryManager::getInput(SectorTick _tick, ClientsInputMap& _outClientsInputMap) const
{
	if(!mClientsInputByTick.empty())
		_outClientsInputMap = mClientsInputByTick.at(_tick);
}

void InputHistoryManager::update(SectorTick _sectorTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "update", "", false);

	mCurrentSectorTick = _sectorTick;

	if (mClientsInputByTick.empty())
		return;
	else
	{
		//Debug assert: make sure that an entry for _sectorTick - 1 exists
		assert(mClientsInputByTick.find(_sectorTick - 1) != mClientsInputByTick.end());

		//Create copy input based on input at _sectorTick - 1
		ClientsInputMap& clientsInputMapCopy = mClientsInputByTick[_sectorTick];
		clientsInputMapCopy = mClientsInputByTick[_sectorTick - 1];
		//Mark inputs as not certified
		for (std::map<RakNet::RakNetGUID, InputState>::reference idInputPair : clientsInputMapCopy)
		{
			idInputPair.second.mCertified = false;
		}
	}
}
