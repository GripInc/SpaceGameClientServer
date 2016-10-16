#ifndef _LOGGER_MANAGER_H_
#define _LOGGER_MANAGER_H_

#include <string>

class LoggerManager
{
public:
	static const LoggerManager& getInstance();

	void logE(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _addNewLine = false) const;
	void logW(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _addNewLine = false) const;
	void logI(const std::string& _logClass, const std::string& _logFunction, const std::string& _log, bool _forceStdOut, bool _addNewLine = false) const;

protected:
	LoggerManager() {}

	static LoggerManager* mInstance;

	void log(const std::string& _string, bool _writeOnStandardOutput, bool _addNewLine) const;
};

#endif //_LOGGER_MANAGER_H_