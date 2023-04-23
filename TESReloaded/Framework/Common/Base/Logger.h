#pragma once

class Logger {
public:
	static void Initialize(const char* FileName);
	static void Log(char* Message, ...);
	static void Log(const char* Message, ...);
	static void Debug(char* Message, ...);
	static void Debug(const char* Message, ...);
	
//	static char			MessageBuffer[8192];
	static FILE*		LogFile;

};
