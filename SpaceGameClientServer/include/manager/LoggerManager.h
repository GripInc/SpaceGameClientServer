#ifndef _LOGGER_MANAGER_H_
#define _LOGGER_MANAGER_H_

#include <string>

class LoggerManager
{
public:
	static const LoggerManager& getInstance();

	void logE(const std::string& _logClass, const std::string& _logFunction, const std::string& _log) const;
	void logW(const std::string& _logClass, const std::string& _logFunction, const std::string& _log) const;
	void logI(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _forceStdOut) const;

protected:
	LoggerManager() {}

	static LoggerManager* mInstance;

	void log(const std::string& _string, bool _writeOnStandardOutput) const;
};

#endif //_LOGGER_MANAGER_H_