#include "WetWorld.h"

void WetWorldEffect::UpdateConstants() {
	// Rain fall & puddles
	if (TheShaderManager->GameState.isRainy && Constants.PuddlesAnimator.switched == false) {
		// it just started raining
		Constants.PuddlesAnimator.switched = true;
		Constants.PuddlesAnimator.Start(increaseRate, 1);
	}
	else if (!TheShaderManager->GameState.isRainy && Constants.PuddlesAnimator.switched) {
		// it just stopped raining
		Constants.PuddlesAnimator.switched = false;
		Constants.PuddlesAnimator.Start(decreaseRate, 0);
	}
	Constants.Data.x = TheShaderManager->Effects.Rain->Constants.RainAnimator.GetValue();
	Constants.Data.y = TheShaderManager->GameState.isRainy;
	Constants.Data.z = Constants.PuddlesAnimator.GetValue();

	if (Constants.Data.x || Constants.Data.z) TheShaderManager->orthoRequired = true; // mark ortho map calculation as necessary
}

void WetWorldEffect::UpdateSettings() {
	increaseRate = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Increase");
	decreaseRate = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Decrease");

	Constants.Coeffs.x = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_R");
	Constants.Coeffs.y = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_G");
	Constants.Coeffs.z = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_B");
	Constants.Coeffs.w = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleSpecularMultiplier");
	Constants.Data.w = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Amount");
}

void WetWorldEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_WetWorldCoeffs", &Constants.Coeffs);
	TheShaderManager->RegisterConstant("TESR_WetWorldData", &Constants.Data);
}

bool WetWorldEffect::ShouldRender() { 
	return (Constants.Data.x || Constants.Data.z) && TheShaderManager->GameState.isExterior && !TheShaderManager->GameState.isUnderwater;
};