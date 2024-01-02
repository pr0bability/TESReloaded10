#include "Snow.h"

void SnowEffect::UpdateConstants() {
	// Snow fall
	if (TheShaderManager->isSnow && Constants.SnowAnimator.switched == false) {
		// it just started snowing
		TheShaderManager->Effects.WetWorld->Constants.PuddlesAnimator.Start(0.3, 0); // fade out any puddles if they exist
		Constants.SnowAnimator.switched = true;
		Constants.SnowAnimator.Initialize(0);
		Constants.SnowAnimator.Start(0.5, 1);
	}
	else if (!TheShaderManager->isSnow && Constants.SnowAnimator.switched) {
		// it just stopped snowing
		Constants.SnowAnimator.switched = false;
		Constants.SnowAnimator.Start(0.2, 0);
	}
	Constants.Data.x = Constants.SnowAnimator.GetValue();

	if (Constants.Data.x) TheShaderManager->orthoRequired = true; // mark ortho map calculation as necessary
	if (TheSettingManager->SettingsChanged) Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Snow.Main", "Speed");
}