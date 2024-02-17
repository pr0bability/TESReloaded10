#include "BloomLegacy.h"

void BloomLegacyEffect::UpdateConstants() {
	ValuesStruct* category = &Settings.Main;
	if (!TheShaderManager->GameState.isExterior) category = &Settings.Interiors;

	Constants.Data.x = category->Luminance;
	Constants.Data.y = category->MiddleGray;
	Constants.Data.z = category->WhiteCutOff;
	Constants.Values.x = category->BloomIntensity;
	Constants.Values.y = category->OriginalIntensity;
	Constants.Values.z = category->BloomSaturation;
	Constants.Values.w = category->OriginalSaturation;
}

void BloomLegacyEffect::UpdateSettings() {
	Settings.Main.Luminance = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Exteriors", "Luminance");
	Settings.Main.MiddleGray = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Exteriors", "MiddleGray");
	Settings.Main.WhiteCutOff = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Exteriors", "WhiteCutOff");
	Settings.Main.BloomIntensity = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Exteriors", "BloomIntensity");
	Settings.Main.OriginalIntensity = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Exteriors", "OriginalIntensity");
	Settings.Main.BloomSaturation = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Exteriors", "BloomSaturation");
	Settings.Main.OriginalSaturation = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Exteriors", "OriginalSaturation");

	Settings.Interiors.Luminance = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Interiors", "Luminance");
	Settings.Interiors.MiddleGray = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Interiors", "MiddleGray");
	Settings.Interiors.WhiteCutOff = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Interiors", "WhiteCutOff");
	Settings.Interiors.BloomIntensity = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Interiors", "BloomIntensity");
	Settings.Interiors.OriginalIntensity = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Interiors", "OriginalIntensity");
	Settings.Interiors.BloomSaturation = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Interiors", "BloomSaturation");
	Settings.Interiors.OriginalSaturation = TheSettingManager->GetSettingF("Shaders.BloomLegacy.Interiors", "OriginalSaturation");
}

void BloomLegacyEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_BloomData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_BloomValues", &Constants.Values);
}