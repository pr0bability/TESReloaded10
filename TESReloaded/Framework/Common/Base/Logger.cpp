#include "Logger.h"
//char	Logger::MessageBuffer[8192];
FILE*	Logger::LogFile;

//#define debugmode

void Logger::Initialize(const char* FileName) {

	LogFile = _fsopen(FileName, "w", _SH_DENYWR);

}

void Logger::Log(char* Message, ...) {

	va_list Args;

	if (LogFile) {
		va_start(Args, Message);
		vfprintf_s(LogFile, Message, Args);
		va_end(Args);
//		fputs(MessageBuffer, LogFile);
		fputc('\n', LogFile);
		fflush(LogFile);
	}

}

void Logger::Log(const char* Message, ...) {

	va_list Args;

	if (LogFile) {
		va_start(Args, Message);
		vfprintf_s(LogFile, Message, Args);
		va_end(Args);
//		fputs(MessageBuffer, LogFile);
		fputc('\n', LogFile);
		fflush(LogFile);
	}

}

void Logger::Debug(char* Message, ...) {
#ifdef debugmode
	va_list Args;
	if (LogFile) {
		va_start(Args, Message);
		vfprintf_s(LogFile, Message, Args);
		va_end(Args);
		//		fputs(MessageBuffer, LogFile);
		fputc('\n', LogFile);
		fflush(LogFile);
	}
#endif // debugmode
}

void Logger::Debug(const char* Message, ...) {
#ifdef debugmode
	va_list Args;
	if (LogFile) {
		va_start(Args, Message);
		vfprintf_s(LogFile, Message, Args);
		va_end(Args);
		//		fputs(MessageBuffer, LogFile);
		fputc('\n', LogFile);
		fflush(LogFile);
	}
#endif // debugmode
}
