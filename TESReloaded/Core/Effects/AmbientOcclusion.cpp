#include "AmbientOcclusion.h"

void AmbientOcclusionEffect::UpdateConstants() {
	const char* sectionName = NULL;

	sectionName = "Shaders.AmbientOcclusion.Exteriors";
	if (!TheShaderManager->isExterior) sectionName = "Shaders.AmbientOcclusion.Interiors";

	Constants.Enabled = TheSettingManager->GetSettingI(sectionName, "Enabled");
	if (Constants.Enabled) {
		Constants.AOData.x = TheSettingManager->GetSettingF(sectionName, "Samples");
		Constants.AOData.y = TheSettingManager->GetSettingF(sectionName, "StrengthMultiplier");
		Constants.AOData.z = TheSettingManager->GetSettingF(sectionName, "ClampStrength");
		Constants.AOData.w = TheSettingManager->GetSettingF(sectionName, "Range");
		Constants.Data.x = TheSettingManager->GetSettingF(sectionName, "AngleBias");
		Constants.Data.y = TheSettingManager->GetSettingF(sectionName, "LumThreshold");
		Constants.Data.z = TheSettingManager->GetSettingF(sectionName, "BlurDropThreshold");
		Constants.Data.w = TheSettingManager->GetSettingF(sectionName, "BlurRadiusMultiplier");
	}
}