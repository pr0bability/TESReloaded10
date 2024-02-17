#include "Skin.h"

void SkinShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_SkinData", &Constants.SkinData);
	TheShaderManager->RegisterConstant("TESR_SkinColor", &Constants.SkinColor);
}

void SkinShaders::UpdateConstants() {}

void SkinShaders::UpdateSettings() {
	Constants.SkinData.x = TheSettingManager->GetSettingF("Shaders.Skin.Main", "Attenuation");
	Constants.SkinData.y = TheSettingManager->GetSettingF("Shaders.Skin.Main", "SpecularPower");
	Constants.SkinData.z = TheSettingManager->GetSettingF("Shaders.Skin.Main", "MaterialThickness");
	Constants.SkinData.w = TheSettingManager->GetSettingF("Shaders.Skin.Main", "RimScalar");

	Constants.SkinColor.x = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffRed");
	Constants.SkinColor.y = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffGreen");
	Constants.SkinColor.z = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffBlue");
}