#include "BloomLegacy.h"

void BloomLegacyEffect::UpdateConstants() {
	const char* sectionName = "Shaders.BloomLegacy.Exteriors";
	if (!TheShaderManager->isExterior) sectionName = "Shaders.BloomLegacy.Interiors";

	Constants.Data.x = TheSettingManager->GetSettingF(sectionName, "Luminance");
	Constants.Data.y = TheSettingManager->GetSettingF(sectionName, "MiddleGray");
	Constants.Data.z = TheSettingManager->GetSettingF(sectionName, "WhiteCutOff");
	Constants.Values.x = TheSettingManager->GetSettingF(sectionName, "BloomIntensity");
	Constants.Values.y = TheSettingManager->GetSettingF(sectionName, "OriginalIntensity");
	Constants.Values.z = TheSettingManager->GetSettingF(sectionName, "BloomSaturation");
	Constants.Values.w = TheSettingManager->GetSettingF(sectionName, "OriginalSaturation");
}

void BloomLegacyEffect::UpdateSettings() {
}

void BloomLegacyEffect::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_BloomData"] = &Constants.Data;
	TheShaderManager->ConstantsTable["TESR_BloomValues"] = &Constants.Values;
}