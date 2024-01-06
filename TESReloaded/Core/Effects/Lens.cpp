#include "Lens.h"

void LensEffect::UpdateConstants() {

	if (TheShaderManager->isExterior) {
		Constants.Data.y = std::lerp(TheSettingManager->GetSettingF("Shaders.Lens.Main", "NightBloomTreshold"), TheSettingManager->GetSettingF("Shaders.Lens.Main", "ExteriorBloomTreshold"), TheShaderManager->transitionCurve);
	}
	else {
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Lens.Main", "InteriorBloomTreshold");
	}
}

void LensEffect::UpdateSettings(){
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Lens.Main", "DirtLensAmount");
}

void LensEffect::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_LensData"] = &Constants.Data;
}