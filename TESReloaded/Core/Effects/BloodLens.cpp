#include "BloodLens.h"

void BloodLensEffect::UpdateConstants() {
	if (TheShaderManager->GameState.isUnderwater) {
		Constants.Percent = 0.0f;
	}

	if (Constants.Percent > 0.0f) {
		Constants.Time.z = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "Time");
		if (Constants.Percent == 1.0f) {
			Constants.Time.w = 0.0f;
			srand(time(NULL));
			Constants.Params.x = (double)rand() / (RAND_MAX + 1) * (0.75f - 0.25f) + 0.25f; //from 0.25 to 0.75
			Constants.Params.y = (double)rand() / (RAND_MAX + 1) * (0.5f + 0.1f) - 0.1f; //from -0.1 to 0.5
			Constants.Params.z = (double)rand() / (RAND_MAX + 1) * (2.0f + 2.0f) - 2.0f; //from -2 to 2
		}
		Constants.Time.w += 1.0f;
		Constants.Percent = 1.0f - Constants.Time.w / Constants.Time.z;
		if (Constants.Percent < 0.0f)
			Constants.Percent = 0.0f;
	}

}

void BloodLensEffect::UpdateSettings() {
	Constants.Params.w = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "Intensity") * Constants.Percent;
	Constants.BloodColor.x = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorR");
	Constants.BloodColor.y = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorG");
	Constants.BloodColor.z = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorB");
}

void BloodLensEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_BloodLensParams", &Constants.Params);
	TheShaderManager->RegisterConstant("TESR_BloodLensColor", &Constants.BloodColor);
}

bool BloodLensEffect::ShouldRender() {
	return Constants.Percent > 0.0f; 
};