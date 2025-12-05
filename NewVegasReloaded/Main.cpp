#define WaitForDebugger 0
#define HookDevice 0

#include "../src/NewVegas/Hooks/Hooks.h"
#include "../src/core/Device/Hook.h"

extern "C" {

	static void MessageHandler(NVSEMessagingInterface::Message* msg) {
		switch (msg->type) {
		case NVSEMessagingInterface::kMessage_PostLoad:
			if (!GetModuleHandle(L"LODFlickerFix.dll")) {
				MessageBox(NULL, L"LOD Flicker Fix not found.\nNew Vegas Reloaded cannot be used without it, please install it.", L"New Vegas Reloaded", MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}

			if (!GetModuleHandle(L"VanillaPlusTerrain.dll")) {
				MessageBox(NULL, L"Vanilla Plus Terrain not found.\nNew Vegas Reloaded cannot be used without it, please install it.", L"New Vegas Reloaded", MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}

			break;
		case NVSEMessagingInterface::kMessage_PreLoadGame:
			TheSettingManager->GameLoading = true;
			break;
		case NVSEMessagingInterface::kMessage_PostLoadGame:
			TheSettingManager->GameLoading = false;
			TheShaderManager->InitializeConstants();
			break;
		case NVSEMessagingInterface::kMessage_DeferredInit:
			CombineDepthEffect* depthEffect = TheShaderManager->Effects.CombineDepth;

			if (!depthEffect->johnnyguitar) {
				depthEffect->johnnyguitar = GetModuleHandle(L"johnnyguitar.dll");
			}

			if (depthEffect->johnnyguitar) {
				Logger::Log("JG found, initializing functions");
				depthEffect->JG_SetClipDist = (bool(__cdecl*)(float))GetProcAddress(depthEffect->johnnyguitar, "JGSetViewmodelClipDistance");
				depthEffect->JG_GetClipDist = (float(__cdecl*)())GetProcAddress(depthEffect->johnnyguitar, "JGGetViewmodelClipDistance");
			}
			else {
				Logger::Log("JG not found");
			}

			if (GetModuleHandle(L"VanillaPlusAO.dll")) {
				TheShaderManager->Effects.AmbientOcclusion->bNVAOLoaded = true;
			}			

			HMODULE hRTM = GetModuleHandle(L"RealTimeMenus.dll");
			if (hRTM) {
				Logger::Log("Real Time Menus found, initializing functions");
				TheGameMenuManager->IsLiveMenu = (GameMenuManager::MenuPauseState(__cdecl*)(uint32_t, bool, bool))GetProcAddress(hRTM, "IsLiveMenu");
			}

			// Make sure all SLS vertex shaders pass and update EyePosition.
			ShadowLightShader::EnableEyePositionForAllPasses();

			break;
		}
	}

	static void ShaderLoaderHandler(NVSEMessagingInterface::Message* msg) {
		if (msg->type == 0) {
			// Make sure all SLS vertex shaders pass and update EyePosition.
			ShadowLightShader::EnableEyePositionForAllPasses();

			// Reload all effects.
			TheShaderManager->EffectReloadQueued = true;
		}
	}

	bool NVSEPlugin_Query(const PluginInterface* Interface, PluginInfo* Info) {
		
		Info->InfoVersion = PluginInfo::kInfoVersion;
		Info->Name = "NewVegasReloaded";
		Info->Version = 433;
		return true;

	}

	bool NVSEPlugin_Load(const PluginInterface* Interface) {

#if _DEBUG
	#if WaitForDebugger
		while (!IsDebuggerPresent()) Sleep(10);
	#endif
#endif
#if HookDevice
		AttachDeviceHooks();
#endif

		Logger::Initialize("NewVegasReloaded.log");

		PluginVersion::CreateVersionString();
		Logger::Log(PluginVersion::VersionString);

		CommandManager::Initialize(Interface);

		if (!Interface->IsEditor) {
			((NVSEMessagingInterface*)Interface->QueryInterface(kInterface_Messaging))->RegisterListener(Interface->GetPluginHandle(), "NVSE", MessageHandler);
			((NVSEMessagingInterface*)Interface->QueryInterface(kInterface_Messaging))->RegisterListener(Interface->GetPluginHandle(), "Shader Loader", ShaderLoaderHandler);
			
			SettingManager::Initialize();
			TheSettingManager->LoadSettings();
			AttachHooks();
		}
		return true;

	}

};

