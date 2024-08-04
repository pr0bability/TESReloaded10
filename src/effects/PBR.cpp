#include "PBR.h"

void PBRShaders::RegisterConstants() {
}


void PBRShaders::UpdateSettings() {
	Settings.Default.Saturation = TheSettingManager->GetSettingF("Shaders.PBR.Main", "Saturation");
	Settings.Default.Metallicness = TheSettingManager->GetSettingF("Shaders.PBR.Main", "Metallicness");
	Settings.Default.Roughness = TheSettingManager->GetSettingF("Shaders.PBR.Main", "Roughness");
	Settings.Default.LightScale = TheSettingManager->GetSettingF("Shaders.PBR.Main", "LightingScale");
	Settings.Default.AmbientScale = TheSettingManager->GetSettingF("Shaders.PBR.Main", "AmbientScale");

	Settings.Rain.Saturation = TheSettingManager->GetSettingF("Shaders.PBR.Rain", "Saturation");
	Settings.Rain.Metallicness = TheSettingManager->GetSettingF("Shaders.PBR.Rain", "Metallicness");
	Settings.Rain.Roughness = TheSettingManager->GetSettingF("Shaders.PBR.Rain", "Roughness");
	Settings.Rain.LightScale = TheSettingManager->GetSettingF("Shaders.PBR.Rain", "LightingScale");
	Settings.Rain.AmbientScale = TheSettingManager->GetSettingF("Shaders.PBR.Rain", "AmbientScale");

	Settings.Night.Saturation = TheSettingManager->GetSettingF("Shaders.PBR.Night", "Saturation");
	Settings.Night.Metallicness = TheSettingManager->GetSettingF("Shaders.PBR.Night", "Metallicness");
	Settings.Night.Roughness = TheSettingManager->GetSettingF("Shaders.PBR.Night", "Roughness");
	Settings.Night.LightScale = TheSettingManager->GetSettingF("Shaders.PBR.Night", "LightingScale");
	Settings.Night.AmbientScale = TheSettingManager->GetSettingF("Shaders.PBR.Night", "AmbientScale");

	Settings.NightRain.Saturation = TheSettingManager->GetSettingF("Shaders.PBR.NightRain", "Saturation");
	Settings.NightRain.Metallicness = TheSettingManager->GetSettingF("Shaders.PBR.NightRain", "Metallicness");
	Settings.NightRain.Roughness = TheSettingManager->GetSettingF("Shaders.PBR.NightRain", "Roughness");
	Settings.NightRain.LightScale = TheSettingManager->GetSettingF("Shaders.PBR.NightRain", "LightingScale");
	Settings.NightRain.AmbientScale = TheSettingManager->GetSettingF("Shaders.PBR.NightRain", "AmbientScale");
}

void PBRShaders::UpdateConstants() {
	// get max value between rain animator and puddle animator
	float rainFactor = max(TheShaderManager->Effects.WetWorld->Constants.Data.x, TheShaderManager->Effects.WetWorld->Constants.Data.z);

	Constants.ExtraData.z = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.Saturation, Settings.Night.Saturation, 0.0),
		TheShaderManager->GetTransitionValue(Settings.Rain.Saturation, Settings.NightRain.Saturation, 0.0), rainFactor);

	Constants.Data.x = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.Metallicness, Settings.Night.Metallicness, 0.0),
		TheShaderManager->GetTransitionValue(Settings.Rain.Metallicness, Settings.NightRain.Metallicness, 0.0), rainFactor);
	Constants.Data.y = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.Roughness, Settings.Night.Roughness, 0.0),
		TheShaderManager->GetTransitionValue(Settings.Rain.Roughness, Settings.NightRain.Roughness, 0.0), rainFactor);

	Constants.Data.z = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.LightScale, Settings.Night.LightScale, 0.0),
		TheShaderManager->GetTransitionValue(Settings.Rain.LightScale, Settings.NightRain.LightScale, 0.0), rainFactor);
	Constants.Data.w = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.AmbientScale, Settings.Night.AmbientScale, 0.0),
		TheShaderManager->GetTransitionValue(Settings.Rain.AmbientScale, Settings.NightRain.AmbientScale, 0.0), rainFactor);
};

