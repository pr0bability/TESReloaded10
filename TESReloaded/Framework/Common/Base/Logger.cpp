#include "Logger.h"
//char	Logger::MessageBuffer[8192];
FILE*	Logger::LogFile;

#include<chrono>

//#define debugmode
//#define logperf

TimeLogger::TimeLogger() {
#ifdef logperf
	start = std::chrono::system_clock::now();
	end = std::chrono::system_clock::now();
#endif
};


TimeLogger::~TimeLogger() {
};


/*
* Starts the animator by setting a target value and a duration to reach it.
*/
void TimeLogger::LogTime(const char* Name) {
#ifdef logperf
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	Logger::Log("%s ran in %f ms", Name, elapsed_seconds * 1000);
#endif
}



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

NiDX9RenderState::NiRenderStateSetting* RenderStateSettings;
void Logger::TraceRenderState() {
	NiDX9RenderState* RenderState = TheRenderManager->renderState;

	if(RenderStateSettings == nullptr){
		RenderStateSettings = (NiDX9RenderState::NiRenderStateSetting*)malloc(sizeof(NiDX9RenderState::NiRenderStateSetting) * 256);
		memcpy(RenderStateSettings, RenderState->RenderStateSettings, sizeof(NiDX9RenderState::NiRenderStateSetting) * 256);
	}
	else{
		bool print = false;
		for(size_t i = 0; i < 256; i++){
			if(RenderStateSettings[i].CurrentValue != RenderState->RenderStateSettings[i].CurrentValue){
				Logger::Log("Different state between iterations: State %u was %u is %u",i, RenderStateSettings[i].CurrentValue, RenderState->RenderStateSettings[i].CurrentValue);
				RenderStateSettings[i].CurrentValue = RenderState->RenderStateSettings[i].CurrentValue;
				print = true;

			}
		}
		if(print) Logger::Log("End");
	}

	//Logger::Log("==== Render State ====");

	//Logger::Log("Render State D3DRS_ZENABLE %i", RenderState->GetRenderState(D3DRS_ZENABLE));
	//Logger::Log("Render State D3DRS_FILLMODE %i", RenderState->GetRenderState(D3DRS_FILLMODE));
	//Logger::Log("Render State D3DRS_SHADEMODE %i", RenderState->GetRenderState(D3DRS_SHADEMODE));
	//Logger::Log("Render State D3DRS_ZWRITEENABLE %i", RenderState->GetRenderState(D3DRS_ZWRITEENABLE));
	//Logger::Log("Render State D3DRS_ALPHATESTENABLE %i", RenderState->GetRenderState(D3DRS_ALPHATESTENABLE));
	//Logger::Log("Render State D3DRS_LASTPIXEL %i", RenderState->GetRenderState(D3DRS_LASTPIXEL));
	//Logger::Log("Render State D3DRS_SRCBLEND %i", RenderState->GetRenderState(D3DRS_SRCBLEND));
	//Logger::Log("Render State D3DRS_DESTBLEND %i", RenderState->GetRenderState(D3DRS_DESTBLEND));
	//Logger::Log("Render State D3DRS_CULLMODE %i", RenderState->GetRenderState(D3DRS_CULLMODE));
	//Logger::Log("Render State D3DRS_ZFUNC %i", RenderState->GetRenderState(D3DRS_ZFUNC));
	//Logger::Log("Render State D3DRS_ALPHAREF %i", RenderState->GetRenderState(D3DRS_ALPHAREF));
	//Logger::Log("Render State D3DRS_ALPHAFUNC %i", RenderState->GetRenderState(D3DRS_ALPHAFUNC));
	//Logger::Log("Render State D3DRS_DITHERENABLE %i", RenderState->GetRenderState(D3DRS_DITHERENABLE));
	//Logger::Log("Render State D3DRS_ALPHABLENDENABLE %i", RenderState->GetRenderState(D3DRS_ALPHABLENDENABLE));
	//Logger::Log("Render State D3DRS_FOGENABLE %i", RenderState->GetRenderState(D3DRS_FOGENABLE));
	//Logger::Log("Render State D3DRS_SPECULARENABLE %i", RenderState->GetRenderState(D3DRS_SPECULARENABLE));
	//Logger::Log("Render State D3DRS_FOGCOLOR %i", RenderState->GetRenderState(D3DRS_FOGCOLOR));
	//Logger::Log("Render State D3DRS_FOGTABLEMODE %i", RenderState->GetRenderState(D3DRS_FOGTABLEMODE));
	//Logger::Log("Render State D3DRS_FOGSTART %i", RenderState->GetRenderState(D3DRS_FOGSTART));
	//Logger::Log("Render State D3DRS_FOGEND %i", RenderState->GetRenderState(D3DRS_FOGEND));
	//Logger::Log("Render State D3DRS_POINTSIZE %i", RenderState->GetRenderState(D3DRS_POINTSIZE));
	//Logger::Log("Render State D3DRS_STENCILENABLE %i", RenderState->GetRenderState(D3DRS_STENCILENABLE));
	//Logger::Log("Render State D3DRS_STENCILREF %i", RenderState->GetRenderState(D3DRS_STENCILREF));
	//Logger::Log("Render State D3DRS_STENCILFUNC %i", RenderState->GetRenderState(D3DRS_STENCILFUNC));	
	
}