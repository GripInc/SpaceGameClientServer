#include "manager/LoggerManager.h"

#include "OgreLogManager.h"

#include <Windows.h>
#include <iostream>
#include <chrono>
#include <iomanip>

LoggerManager* LoggerManager::mInstance = NULL;

const LoggerManager& LoggerManager::getInstance()
{
	if (!mInstance)
	{
		mInstance = new LoggerManager();
		Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
	}

	return *mInstance;
}

void LoggerManager::log(const std::string& _string, bool _writeOnStandardOutput, bool _addNewLine) const
{
	std::string toLog = _string + (_addNewLine ? "\n" : "");

#ifdef _DEBUG
	OutputDebugString(toLog.c_str());
#endif

	Ogre::LogManager::getSingleton().logMessage(toLog);

	if(_writeOnStandardOutput)
		std::cout << toLog << std::endl;
}

std::string LoggerManager::getMilliseconds() const
{
	tm localTime;
	std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();
	time_t now = std::chrono::system_clock::to_time_t(timePoint);
	localtime_s(&localTime, &now);

	const std::chrono::duration<double> timeSinceEpoch = timePoint.time_since_epoch();
	std::chrono::seconds::rep milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceEpoch).count() % 1000;

	std::ostringstream result;

	//result << (1900 + localTime.tm_year) << '-'
	//	<< std::setfill('0') << std::setw(2) << (localTime.tm_mon + 1) << '-'
	//	<< std::setfill('0') << std::setw(2) << localTime.tm_mday << ' '
	//	<< std::setfill('0') << std::setw(2) << localTime.tm_hour << ':'
	//	<< std::setfill('0') << std::setw(2) << localTime.tm_min << ':'
	//	<< std::setfill('0') << std::setw(2) << localTime.tm_sec << '.'
	//	<< std::setfill('0') << std::setw(3) << milliseconds
	//	<< std::endl;

	result << milliseconds;

	return result.str();
}

void LoggerManager::logE(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _addNewLine /* = false */) const
{
	log(getMilliseconds() + "::" + "ERROR IN : " + _logClass + "::" + _logFunction + (_log.empty() ? "" : (" : " + _log)), true, _addNewLine);
}

void LoggerManager::logW(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _addNewLine /* = false */) const
{
	log(getMilliseconds() + "::" + "WARNING IN : " + _logClass + "::" + _logFunction + (_log.empty() ? "" : (" : " + _log)), true, _addNewLine);
}

void LoggerManager::logI(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _forceStdOut, bool _addNewLine /* = false */) const
{
	log(getMilliseconds() + "::" + _logClass + "::" + _logFunction + (_log.empty() ? "" : (" : " + _log)), _forceStdOut | false, _addNewLine);
}
