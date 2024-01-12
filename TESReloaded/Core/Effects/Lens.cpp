#include "Lens.h"

void LensEffect::UpdateConstants() {

	if (TheShaderManager->GameState.isExterior) {
		Constants.Data.y = std::lerp(TheSettingManager->GetSettingF("Shaders.Lens.Main", "NightBloomTreshold"), TheSettingManager->GetSettingF("Shaders.Lens.Main", "ExteriorBloomTreshold"), TheShaderManager->GameState.transitionCurve);
	}
	else {
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Lens.Main", "InteriorBloomTreshold");
	}
}

void LensEffect::UpdateSettings(){
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Lens.Main", "DirtLensAmount");
}

void LensEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_LensData", &Constants.Data);
}

bool LensEffect::ShouldRender() 
{
	return !TheShaderManager->GameState.isUnderwater; 
};
