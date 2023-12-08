#include "VolumetricFog.h"

void VolumetricFogEffect::UpdateConstants() {
	Constants.Data.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Exponent");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "ColorCoeff");
	Constants.Data.z = TheShaderManager->isExterior ? TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Amount") : TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "AmountInterior");
	Constants.Data.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "MaxDistance");
}