#include "Exposure.h"

void ExposureEffect::UpdateConstants() {
	TheShaderManager->avglumaRequired = true; // mark average luma calculation as necessary

	bool isExterior = TheShaderManager->isExterior;
	float transitionCurve = TheShaderManager->transitionCurve;

	if (TheSettingManager->SettingsChanged || TheShaderManager->isDayTimeChanged) {
		Constants.Data.x = TheSettingManager->GetSettingTransition("Shaders.Exposure", "MinBrightness", isExterior, transitionCurve);
		Constants.Data.y = TheSettingManager->GetSettingTransition("Shaders.Exposure", "MaxBrightness", isExterior, transitionCurve);
		Constants.Data.z = TheSettingManager->GetSettingTransition("Shaders.Exposure", "DarkAdaptSpeed", isExterior, transitionCurve);
		Constants.Data.w = TheSettingManager->GetSettingTransition("Shaders.Exposure", "LightAdaptSpeed", isExterior, transitionCurve);
	}
}

void ExposureEffect::UpdateSettings(){}

void ExposureEffect::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_ExposureData"] = &Constants.Data;
}