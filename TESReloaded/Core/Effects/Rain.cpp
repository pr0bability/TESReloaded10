#include "Rain.h"

void RainEffect::UpdateConstants() {
	if (TheShaderManager->isRainy && Constants.RainAnimator.switched == false) {
		// it just started raining
		Constants.RainAnimator.switched = true;
		Constants.RainAnimator.Start(0.05, 1);
	}
	else if (!TheShaderManager->isRainy && Constants.RainAnimator.switched) {
		// it just stopped raining
		Constants.RainAnimator.switched = false;
		Constants.RainAnimator.Start(0.07, 0);
	}

	Constants.Data.x = Constants.RainAnimator.GetValue();

	if (TheSettingManager->SettingsChanged) {
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "VerticalScale");
		Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Speed");
		Constants.Data.w = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Opacity");

		Constants.Aspect.x = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Refraction");
		Constants.Aspect.y = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Coloring");
		Constants.Aspect.z = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Bloom");
	}
}