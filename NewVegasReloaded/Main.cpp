#define WaitForDebugger 0
#define HookDevice 0

#include "../src/NewVegas/Hooks/Hooks.h"
#include "../src/core/Device/Hook.h"

extern "C" {

	static void MessageHandler(NVSEMessagingInterface::Message* msg) {
		switch (msg->type) {
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

			if (TheSettingManager->SettingsMain.Main.InvertedDepth)
				TheRenderManager->ToggleDepthDirection(true);

			break;
		}
	}

	bool NVSEPlugin_Query(const PluginInterface* Interface, PluginInfo* Info) {
		
		Info->InfoVersion = PluginInfo::kInfoVersion;
		Info->Name = "NewVegasReloaded";
		Info->Version = 4;
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
		CommandManager::Initialize(Interface);

		if (!Interface->IsEditor) {
			((NVSEMessagingInterface*)Interface->QueryInterface(kInterface_Messaging))->RegisterListener(Interface->GetPluginHandle(), "NVSE", MessageHandler);

			PluginVersion::CreateVersionString();
			SettingManager::Initialize();
			TheSettingManager->LoadSettings();
			AttachHooks();
		}
		return true;

	}

};

