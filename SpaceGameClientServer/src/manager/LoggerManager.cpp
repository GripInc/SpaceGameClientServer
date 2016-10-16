#include "manager/LoggerManager.h"

#include <Windows.h>
#include <iostream>

LoggerManager* LoggerManager::mInstance = NULL;

const LoggerManager& LoggerManager::getInstance()
{
	if(!mInstance)
		mInstance = new LoggerManager();

	return *mInstance;
}

void LoggerManager::log(const std::string& _string, bool _writeOnStandardOutput, bool _addNewLine) const
{
	std::string toLog = _addNewLine ? "\n" : "" + _string;
	toLog += "\n";
#ifdef _DEBUG
	OutputDebugString(toLog.c_str());
#endif

	if(_writeOnStandardOutput)
		std::cout << toLog << std::endl;
}

void LoggerManager::logE(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _addNewLine /* = false */) const
{
	log("ERROR IN : " + _logClass + "::" + _logFunction + (_log.empty() ? "" : (" : " + _log)), true, _addNewLine);
}

void LoggerManager::logW(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _addNewLine /* = false */) const
{
	log("WARNING IN : " + _logClass + "::" + _logFunction + (_log.empty() ? "" : (" : " + _log)), true, _addNewLine);
}

void LoggerManager::logI(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _forceStdOut, bool _addNewLine /* = false */) const
{
	log(_logClass + "::" + _logFunction + (_log.empty() ? "" : (" : " + _log)), _forceStdOut | false, _addNewLine);
}
