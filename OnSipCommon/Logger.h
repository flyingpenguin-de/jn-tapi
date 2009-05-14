#ifndef LOGGER_H
#define LOGGER_H

#include <tchar.h>

class Logger
{
private:
	static int _consoleLevel;
	static int _win32Level;
	static bool _checkLevel(int level);
	static void _outputMsg(int log_level,const TCHAR *str);
	static void _output(const int log_level, const TCHAR * msg);
	static void _output(int level,va_list & argList, const TCHAR * szFormat);

public:
	enum LogLevel
	{
		LEVEL_NONE = 0,
		LEVEL_ERROR = 1,
		LEVEL_WARN  = 2,
		LEVEL_APP	= 3,
		LEVEL_DEBUG = 4,
		LEVEL_TRACE = 5
	};

	static void SetLevel(LogLevel level);
	static void SetConsoleLevel(LogLevel level);
	static void SetWin32Level(LogLevel level);
	static bool IsLevel(LogLevel level);
	static void log_trace(const TCHAR *format,...);
	static void log_debug(const TCHAR *format,...);
	static void log_app(const TCHAR *format,...);
	static void log_warn(const TCHAR *format,...);
	static void log_error(const TCHAR *format,...);
};

#endif