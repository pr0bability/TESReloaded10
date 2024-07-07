#include "Snow.h"

void SnowEffect::UpdateConstants() {
	// Snow fall
	if (TheShaderManager->GameState.isSnow && Constants.SnowAnimator.switched == false) {
		// it just started snowing
		TheShaderManager->Effects.WetWorld->Constants.PuddlesAnimator.Start(0.3f, 0.f); // fade out any puddles if they exist
		Constants.SnowAnimator.switched = true;
		Constants.SnowAnimator.Initialize(0.f);
		Constants.SnowAnimator.Start(0.5f, 1.f);
	}
	else if (!TheShaderManager->GameState.isSnow && Constants.SnowAnimator.switched) {
		// it just stopped snowing
		Constants.SnowAnimator.switched = false;
		Constants.SnowAnimator.Start(0.2f, 0.f);
	}
	Constants.Data.x = Constants.SnowAnimator.GetValue();

	if (Constants.Data.x) TheShaderManager->orthoRequired = true; // mark ortho map calculation as necessary
}

void SnowEffect::UpdateSettings() {
	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Snow.Main", "Speed");
}

void SnowEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_SnowData", &Constants.Data);
}

bool SnowEffect::ShouldRender() {
	return Constants.Data.x > 0.0f &&
		TheShaderManager->GameState.isExterior &&
		!TheShaderManager->GameState.isUnderwater;
};