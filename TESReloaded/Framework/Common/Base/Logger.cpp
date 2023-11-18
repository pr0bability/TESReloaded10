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

stateMap RENDERSTATETYPE;

void Logger::Initialize(const char* FileName) {

	LogFile = _fsopen(FileName, "w", _SH_DENYWR);

	RENDERSTATETYPE["D3DRS_ZENABLE"] = 7;
	RENDERSTATETYPE["D3DRS_FILLMODE"] = 8;
	RENDERSTATETYPE["D3DRS_SHADEMODE"] = 9;
	RENDERSTATETYPE["D3DRS_ZWRITEENABLE"] = 14;
	RENDERSTATETYPE["D3DRS_ALPHATESTENABLE"] = 15;
	RENDERSTATETYPE["D3DRS_LASTPIXEL"] = 16;
	RENDERSTATETYPE["D3DRS_SRCBLEND"] = 19;
	RENDERSTATETYPE["D3DRS_DESTBLEND"] = 20;
	RENDERSTATETYPE["D3DRS_CULLMODE"] = 22;
	RENDERSTATETYPE["D3DRS_ZFUNC"] = 23;
	RENDERSTATETYPE["D3DRS_ALPHAREF"] = 24;
	RENDERSTATETYPE["D3DRS_ALPHAFUNC"] = 25;
	RENDERSTATETYPE["D3DRS_DITHERENABLE"] = 26;
	RENDERSTATETYPE["D3DRS_ALPHABLENDENABLE"] = 27;
	RENDERSTATETYPE["D3DRS_FOGENABLE"] = 28;
	RENDERSTATETYPE["D3DRS_SPECULARENABLE"] = 29;
	RENDERSTATETYPE["D3DRS_FOGCOLOR"] = 34;
	RENDERSTATETYPE["D3DRS_FOGTABLEMODE"] = 35;
	RENDERSTATETYPE["D3DRS_FOGSTART"] = 36;
	RENDERSTATETYPE["D3DRS_FOGEND"] = 37;
	RENDERSTATETYPE["D3DRS_FOGDENSITY"] = 38;
	RENDERSTATETYPE["D3DRS_RANGEFOGENABLE"] = 48;
	RENDERSTATETYPE["D3DRS_STENCILENABLE"] = 52;
	RENDERSTATETYPE["D3DRS_STENCILFAIL"] = 53;
	RENDERSTATETYPE["D3DRS_STENCILZFAIL"] = 54;
	RENDERSTATETYPE["D3DRS_STENCILPASS"] = 55;
	RENDERSTATETYPE["D3DRS_STENCILFUNC"] = 56;
	RENDERSTATETYPE["D3DRS_STENCILREF"] = 57;
	RENDERSTATETYPE["D3DRS_STENCILMASK"] = 58;
	RENDERSTATETYPE["D3DRS_STENCILWRITEMASK"] = 59;
	RENDERSTATETYPE["D3DRS_TEXTUREFACTOR"] = 60;
	RENDERSTATETYPE["D3DRS_WRAP0"] = 128;
	RENDERSTATETYPE["D3DRS_WRAP1"] = 129;
	RENDERSTATETYPE["D3DRS_WRAP2"] = 130;
	RENDERSTATETYPE["D3DRS_WRAP3"] = 131;
	RENDERSTATETYPE["D3DRS_WRAP4"] = 132;
	RENDERSTATETYPE["D3DRS_WRAP5"] = 133;
	RENDERSTATETYPE["D3DRS_WRAP6"] = 134;
	RENDERSTATETYPE["D3DRS_WRAP7"] = 135;
	RENDERSTATETYPE["D3DRS_CLIPPING"] = 136;
	RENDERSTATETYPE["D3DRS_LIGHTING"] = 137;
	RENDERSTATETYPE["D3DRS_AMBIENT"] = 139;
	RENDERSTATETYPE["D3DRS_FOGVERTEXMODE"] = 140;
	RENDERSTATETYPE["D3DRS_COLORVERTEX"] = 141;
	RENDERSTATETYPE["D3DRS_LOCALVIEWER"] = 142;
	RENDERSTATETYPE["D3DRS_NORMALIZENORMALS"] = 143;
	RENDERSTATETYPE["D3DRS_DIFFUSEMATERIALSOURCE"] = 145;
	RENDERSTATETYPE["D3DRS_SPECULARMATERIALSOURCE"] = 146;
	RENDERSTATETYPE["D3DRS_AMBIENTMATERIALSOURCE"] = 147;
	RENDERSTATETYPE["D3DRS_EMISSIVEMATERIALSOURCE"] = 148;
	RENDERSTATETYPE["D3DRS_VERTEXBLEND"] = 151;
	RENDERSTATETYPE["D3DRS_CLIPPLANEENABLE"] = 152;
	RENDERSTATETYPE["D3DRS_POINTSIZE"] = 154;
	RENDERSTATETYPE["D3DRS_POINTSIZE_MIN"] = 155;
	RENDERSTATETYPE["D3DRS_POINTSPRITEENABLE"] = 156;
	RENDERSTATETYPE["D3DRS_POINTSCALEENABLE"] = 157;
	RENDERSTATETYPE["D3DRS_POINTSCALE_A"] = 158;
	RENDERSTATETYPE["D3DRS_POINTSCALE_B"] = 159;
	RENDERSTATETYPE["D3DRS_POINTSCALE_C"] = 160;
	RENDERSTATETYPE["D3DRS_MULTISAMPLEANTIALIAS"] = 161;
	RENDERSTATETYPE["D3DRS_MULTISAMPLEMASK"] = 162;
	RENDERSTATETYPE["D3DRS_PATCHEDGESTYLE"] = 163;
	RENDERSTATETYPE["D3DRS_DEBUGMONITORTOKEN"] = 165;
	RENDERSTATETYPE["D3DRS_POINTSIZE_MAX"] = 166;
	RENDERSTATETYPE["D3DRS_INDEXEDVERTEXBLENDENABLE"] = 167;
	RENDERSTATETYPE["D3DRS_COLORWRITEENABLE"] = 168;
	RENDERSTATETYPE["D3DRS_TWEENFACTOR"] = 170;
	RENDERSTATETYPE["D3DRS_BLENDOP"] = 171;
	RENDERSTATETYPE["D3DRS_POSITIONDEGREE"] = 172;
	RENDERSTATETYPE["D3DRS_NORMALDEGREE"] = 173;
	RENDERSTATETYPE["D3DRS_SCISSORTESTENABLE"] = 174;
	RENDERSTATETYPE["D3DRS_SLOPESCALEDEPTHBIAS"] = 175;
	RENDERSTATETYPE["D3DRS_ANTIALIASEDLINEENABLE"] = 176;
	RENDERSTATETYPE["D3DRS_MINTESSELLATIONLEVEL"] = 178;
	RENDERSTATETYPE["D3DRS_MAXTESSELLATIONLEVEL"] = 179;
	RENDERSTATETYPE["D3DRS_ADAPTIVETESS_X"] = 180;
	RENDERSTATETYPE["D3DRS_ADAPTIVETESS_Y"] = 181;
	RENDERSTATETYPE["D3DRS_ADAPTIVETESS_Z"] = 182;
	RENDERSTATETYPE["D3DRS_ADAPTIVETESS_W"] = 183;
	RENDERSTATETYPE["D3DRS_ENABLEADAPTIVETESSELLATION"] = 184;
	RENDERSTATETYPE["D3DRS_TWOSIDEDSTENCILMODE"] = 185;
	RENDERSTATETYPE["D3DRS_CCW_STENCILFAIL"] = 186;
	RENDERSTATETYPE["D3DRS_CCW_STENCILZFAIL"] = 187;
	RENDERSTATETYPE["D3DRS_CCW_STENCILPASS"] = 188;
	RENDERSTATETYPE["D3DRS_CCW_STENCILFUNC"] = 189;
	RENDERSTATETYPE["D3DRS_COLORWRITEENABLE1"] = 190;
	RENDERSTATETYPE["D3DRS_COLORWRITEENABLE2"] = 191;
	RENDERSTATETYPE["D3DRS_COLORWRITEENABLE3"] = 192;
	RENDERSTATETYPE["D3DRS_BLENDFACTOR"] = 193;
	RENDERSTATETYPE["D3DRS_SRGBWRITEENABLE"] = 194;
	RENDERSTATETYPE["D3DRS_DEPTHBIAS"] = 195;
	RENDERSTATETYPE["D3DRS_WRAP8"] = 198;
	RENDERSTATETYPE["D3DRS_WRAP9"] = 199;
	RENDERSTATETYPE["D3DRS_WRAP10"] = 200;
	RENDERSTATETYPE["D3DRS_WRAP11"] = 201;
	RENDERSTATETYPE["D3DRS_WRAP12"] = 202;
	RENDERSTATETYPE["D3DRS_WRAP13"] = 203;
	RENDERSTATETYPE["D3DRS_WRAP14"] = 204;
	RENDERSTATETYPE["D3DRS_WRAP15"] = 205;
	RENDERSTATETYPE["D3DRS_SEPARATEALPHABLENDENABLE"] = 206;
	RENDERSTATETYPE["D3DRS_SRCBLENDALPHA"] = 207;
	RENDERSTATETYPE["D3DRS_DESTBLENDALPHA"] = 208;
	RENDERSTATETYPE["D3DRS_BLENDOPALPHA"] = 209;
	RENDERSTATETYPE["D3DRS_FORCE_DWORD"] = 0x7fffffff;
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
		for (const auto& [settingName, i] : RENDERSTATETYPE) {
			if (RenderStateSettings[i].CurrentValue != RenderState->RenderStateSettings[i].CurrentValue) {
				Logger::Log("Different state between iterations: State %s was %u is %u", settingName, RenderStateSettings[i].CurrentValue, RenderState->RenderStateSettings[i].CurrentValue);
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