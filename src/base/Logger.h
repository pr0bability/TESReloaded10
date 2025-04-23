#pragma once
#include <chrono>
#include <map>

typedef std::map<const char*, int> stateMap;

class Logger {
public:
	static void Initialize(const char* FileName);
	static void Log(char* Message, ...);
	static void Log(const char* Message, ...);
	static void Debug(char* Message, ...);
	static void Debug(const char* Message, ...);
	static void TraceRenderState();
	
//	static char			MessageBuffer[8192];
	static FILE*		LogFile;

};

class TimeLogger {
public:
	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;
	TimeLogger();
	~TimeLogger();

	float LogTime(const char* Name);
};
