#include "Rain.h"

void RainEffect::UpdateConstants() {
	Constants.Data.x = TheShaderManager->ShaderConst.Animators.RainAnimator.GetValue();

	if (TheSettingManager->SettingsChanged) {
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "VerticalScale");
		Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Speed");
		Constants.Data.w = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Opacity");

		Constants.Aspect.x = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Refraction");
		Constants.Aspect.y = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Coloring");
		Constants.Aspect.z = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Bloom");
	}
}