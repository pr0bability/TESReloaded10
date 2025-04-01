#include "Sky.h"

void SkyShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_SkyData", &Constants.SkyData);
	TheShaderManager->RegisterConstant("TESR_CloudData", &Constants.CloudData);
	TheShaderManager->RegisterConstant("TESR_SunsetColor", &Constants.SunsetColor);
}

void SkyShaders::UpdateConstants() {
	if (TheShaderManager->Shaders.Tonemapping->Enabled)
		Constants.SunsetColor.w = TheShaderManager->GetTransitionValue(Settings.SkyMultiplierDay, Settings.SkyMultiplierNight, 1.0);
	else
		Constants.SunsetColor.w = 1.0;
}

void SkyShaders::UpdateSettings() {

	useSunDiskColor = TheSettingManager->GetSettingF("Shaders.Sky.Main", "UseSunDiskColor");

	Settings.SkyMultiplierDay = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "SkyMultiplier");
	Settings.SkyMultiplierNight = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "SkyMultiplier");

	Constants.SkyData.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "AthmosphereThickness");
	Constants.SkyData.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunInfluence");
	Constants.SkyData.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunStrength");
	Constants.SkyData.w = TheSettingManager->GetSettingF("Shaders.Sky.Main", "StarStrength");

	Constants.CloudData.x = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "UseNormals");
	//Constants.CloudData.y = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "SphericalNormals"); // not used much so it's disabled
	Constants.CloudData.z = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Transparency");
	Constants.CloudData.w = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Brightness");

	Constants.CloudData.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "StarTwinkle");

	// only add sunset color boost in exteriors
	if (TheShaderManager->GameState.isExterior) {
		Constants.SunsetColor.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetR");
		Constants.SunsetColor.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetG");
		Constants.SunsetColor.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetB");
	}
	else {
		Constants.SunsetColor.x = 0;
		Constants.SunsetColor.y = 0;
		Constants.SunsetColor.z = 0;
	}

}