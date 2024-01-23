#include "Exposure.h"

void ExposureEffect::UpdateConstants() {
	TheShaderManager->avglumaRequired = true; // mark average luma calculation as necessary

	if (TheSettingManager->SettingsChanged || TheShaderManager->GameState.isDayTimeChanged) {
		Constants.Data.x = TheShaderManager->GetTransitionValue(Settings.Main.MinBrightness, Settings.Night.MinBrightness, Settings.Interiors.MinBrightness);
		Constants.Data.y = TheShaderManager->GetTransitionValue(Settings.Main.MaxBrightness, Settings.Night.MaxBrightness, Settings.Interiors.MaxBrightness);
		Constants.Data.z = TheShaderManager->GetTransitionValue(Settings.Main.DarkAdaptSpeed, Settings.Night.DarkAdaptSpeed, Settings.Interiors.DarkAdaptSpeed);
		Constants.Data.w = TheShaderManager->GetTransitionValue(Settings.Main.LightAdaptSpeed, Settings.Night.LightAdaptSpeed, Settings.Interiors.LightAdaptSpeed);
	}
}

void ExposureEffect::UpdateSettings(){
	Settings.Main.MinBrightness = TheSettingManager->GetSettingF("Shaders.Exposure.Main", "MinBrightness");
	Settings.Main.MaxBrightness = TheSettingManager->GetSettingF("Shaders.Exposure.Main", "MaxBrightness");
	Settings.Main.DarkAdaptSpeed = TheSettingManager->GetSettingF("Shaders.Exposure.Main", "DarkAdaptSpeed");
	Settings.Main.LightAdaptSpeed = TheSettingManager->GetSettingF("Shaders.Exposure.Main", "LightAdaptSpeed");
													
	Settings.Night.MinBrightness = TheSettingManager->GetSettingF("Shaders.Exposure.Night", "MinBrightness");
	Settings.Night.MaxBrightness = TheSettingManager->GetSettingF("Shaders.Exposure.Night", "MaxBrightness");
	Settings.Night.DarkAdaptSpeed = TheSettingManager->GetSettingF("Shaders.Exposure.Night", "DarkAdaptSpeed");
	Settings.Night.LightAdaptSpeed = TheSettingManager->GetSettingF("Shaders.Exposure.Night", "LightAdaptSpeed");
													
	Settings.Interiors.MinBrightness = TheSettingManager->GetSettingF("Shaders.Exposure.Interiors", "MinBrightness");
	Settings.Interiors.MaxBrightness = TheSettingManager->GetSettingF("Shaders.Exposure.Interiors", "MaxBrightness");
	Settings.Interiors.DarkAdaptSpeed = TheSettingManager->GetSettingF("Shaders.Exposure.Interiors", "DarkAdaptSpeed");
	Settings.Interiors.LightAdaptSpeed = TheSettingManager->GetSettingF("Shaders.Exposure.Interiors", "LightAdaptSpeed");
}

void ExposureEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_ExposureData", &Constants.Data);
}