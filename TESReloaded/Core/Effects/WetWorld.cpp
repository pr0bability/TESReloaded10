#include "WetWorld.h"

void WetWorldEffect::UpdateConstants() {
	if (TheSettingManager->SettingsChanged) {
		Constants.Coeffs.x = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_R");
		Constants.Coeffs.y = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_G");
		Constants.Coeffs.z = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_B");
		Constants.Coeffs.w = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleSpecularMultiplier");
		Constants.Data.w = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Amount");
	}

	// Rain fall & puddles
	if (TheShaderManager->isRainy && Constants.PuddlesAnimator.switched == false) {
		// it just started raining
		Constants.PuddlesAnimator.switched = true;
		Constants.PuddlesAnimator.Start(TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Increase"), 1);
	}
	else if (!TheShaderManager->isRainy && Constants.PuddlesAnimator.switched) {
		// it just stopped raining
		Constants.PuddlesAnimator.switched = false;
		Constants.PuddlesAnimator.Start(TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "Decrease"), 0);
	}
	Constants.Data.x = TheShaderManager->Effects.Rain->Constants.RainAnimator.GetValue();
	Constants.Data.y = TheShaderManager->isRainy;
	Constants.Data.z = Constants.PuddlesAnimator.GetValue();

	if (Constants.Data.x || Constants.Data.z) TheShaderManager->orthoRequired = true; // mark ortho map calculation as necessary
}