#include "Sky.h"

void SkyShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_SkyData", &Constants.SkyData);
	TheShaderManager->RegisterConstant("TESR_CloudData", &Constants.CloudData);
	TheShaderManager->RegisterConstant("TESR_SunsetColor", &Constants.SunsetColor);
}

void SkyShaders::UpdateConstants() {
	if (TheShaderManager->Shaders.Tonemapping->Enabled && (TheSettingManager->SettingsChanged || TheShaderManager->GameState.isDayTimeChanged)) {
		Constants.SunsetColor.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "SkyMultiplier", TheShaderManager->GameState.isExterior, TheShaderManager->GameState.transitionCurve);
	}
	else {
		TheShaderManager->Shaders.Sky->Constants.SunsetColor.w = 1.0;
	}
}

void SkyShaders::UpdateSettings() {

	Constants.SkyData.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "AthmosphereThickness");
	Constants.SkyData.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunInfluence");
	Constants.SkyData.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunStrength");
	Constants.SkyData.w = TheSettingManager->GetSettingF("Shaders.Sky.Main", "StarStrength");

	Constants.CloudData.x = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "UseNormals");
	Constants.CloudData.y = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "SphericalNormals");
	Constants.CloudData.z = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Transparency");
	Constants.CloudData.w = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Brightness");

	// only add sunset color boost in exteriors
	if (TheShaderManager->GameState.isExterior) {
		Constants.SunsetColor.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetR");
		Constants.SunsetColor.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetG");
		Constants.SunsetColor.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetB");

		// TODO : fix sun culling for sun replacing?
		//if (TheSettingManager->GetMenuShaderEnabled("Sky")) {
		//	if (ShaderConst.SunAmount.z) WorldSky->sun->RootNode->m_flags |= ~NiAVObject::NiFlags::DISPLAY_OBJECT; // cull Sun node
		//	else WorldSky->sun->RootNode->m_flags &= NiAVObject::NiFlags::DISPLAY_OBJECT; // disable Sun node
		//}
	}
	else {
		Constants.SunsetColor.x = 0;
		Constants.SunsetColor.y = 0;
		Constants.SunsetColor.z = 0;
	}

}