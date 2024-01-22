#include "Lens.h"

void LensEffect::UpdateConstants() {
	Constants.Data.y = TheShaderManager->GetTransitionValue(Settings.MainThreshold, Settings.NightThreshold, Settings.InteriorThreshold);
}

void LensEffect::UpdateSettings(){
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Lens.Main", "DirtLensAmount");
	Settings.MainThreshold = TheSettingManager->GetSettingF("Shaders.Lens.Main", "ExteriorBloomTreshold");
	Settings.NightThreshold = TheSettingManager->GetSettingF("Shaders.Lens.Main", "NightBloomTreshold");
	Settings.InteriorThreshold = TheSettingManager->GetSettingF("Shaders.Lens.Main", "InteriorBloomTreshold");
}

void LensEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_LensData", &Constants.Data);
}

bool LensEffect::ShouldRender() 
{
	return !TheShaderManager->GameState.isUnderwater && Constants.Data.x > 0;
};
