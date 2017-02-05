#include "manager/InputHistoryManager.h"

#include "utils/StringUtils.h"
#include "manager/LoggerManager.h"

#include <assert.h>

namespace
{
	const std::string LOG_CLASS_TAG = "InputHistoryManager";
}

bool InputHistoryManager::addInputs(const RakNet::RakNetGUID& _id, const std::list<InputState>& _clientInputs)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "START with cliend id " + std::string(_id.ToString()), false);

	std::map<RakNet::RakNetGUID, std::list<InputState> >::iterator foundClientEntry = mClientsInputs.find(_id);
	if (foundClientEntry == mClientsInputs.end())
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "Adding FIRST input", false);

		mClientsInputs[_id] = _clientInputs;
		mLastInputReceivedFromClient[_id] = _clientInputs.back().mTick;

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "END", false);
		return true;
	}
	else
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "Adding inputs", false);

		//Get last registered input tick for this client
		SectorTick lastInputTick = (*foundClientEntry).second.back().mTick;

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "Last registered input tick for this client:" + StringUtils::toStr(lastInputTick), false);

		//We check client inputs received is not a old message
		if (lastInputTick < _clientInputs.back().mTick)
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "Building inputStatesToAppend", false);

			std::list<InputState> inputStatesToAppend;

			for (std::list<InputState>::const_reference newInput : _clientInputs)
			{
				if (newInput.mTick > lastInputTick)
					inputStatesToAppend.push_back(newInput);
			}

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "inputStatesToAppend.size():" + StringUtils::toStr(inputStatesToAppend.size()), false);
			
			std::list<InputState>& inputListToFill = (*foundClientEntry).second;

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "inputListToFill.size() before adding:" + StringUtils::toStr(inputListToFill.size()), false);

			inputListToFill.splice(inputListToFill.end(), inputStatesToAppend);

			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "inputListToFill.size() after adding:" + StringUtils::toStr(inputListToFill.size()), false);

			mLastInputReceivedFromClient[_id] = _clientInputs.back().mTick;
		}
		else
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "Discarding old message: lastInputTick is " + StringUtils::toStr(lastInputTick) + "; _clientInputs.back().mTick is " + StringUtils::toStr(_clientInputs.back().mTick), false);

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInputs", "END", false);
		return false;
	}
}

void InputHistoryManager::getInputForTick(const RakNet::RakNetGUID& _id, SectorTick _sectorTick, InputState& _inputState) const
{
	const std::map<RakNet::RakNetGUID, std::list<InputState> >::const_iterator foundClientEntry = mClientsInputs.find(_id);
	if (foundClientEntry != mClientsInputs.end())
	{
		const std::list<InputState>& inputsList = (*foundClientEntry).second;

		//Don't bother look into list if first element is higher than wanted tick, as it input list is supposed to be ordered
		if (inputsList.front().mTick > _sectorTick)
			return;

		for (std::list<InputState>::const_reference input : inputsList)
		{
			if (input.mTick == _sectorTick)
			{
				_inputState = input;
				return;
			}
		}
	}
}

void InputHistoryManager::incrementNextInputToUse()
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "incrementNextInputToUse", "START", false);

	for (std::map<RakNet::RakNetGUID, SectorTick>::reference inputToUse : mNextInputToUse)
		inputToUse.second++;

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "incrementNextInputToUse", "END", false);
}

void InputHistoryManager::setFirstInputToUse(RakNet::RakNetGUID _id, SectorTick _sectorTick)
{
	mNextInputToUse[_id] = _sectorTick;
}
