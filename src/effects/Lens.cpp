#include "Lens.h"

void LensEffect::UpdateConstants() {
	Constants.Data.x = TheShaderManager->GetTransitionValue(Settings.Main.strength, Settings.Night.strength, Settings.Interiors.strength);
	Constants.Data.y = TheShaderManager->GetTransitionValue(Settings.Main.bloomExponent, Settings.Night.bloomExponent, Settings.Interiors.bloomExponent);
	Constants.Data.z = TheShaderManager->GetTransitionValue(Settings.Main.smudginess, Settings.Night.smudginess, Settings.Interiors.smudginess);
}

void LensEffect::UpdateSettings(){
	Settings.Main.bloomExponent = TheSettingManager->GetSettingF("Shaders.Lens.Main", "BloomExponent");
	Settings.Main.strength = TheSettingManager->GetSettingF("Shaders.Lens.Main", "Strength");
	Settings.Main.smudginess = TheSettingManager->GetSettingF("Shaders.Lens.Main", "Smudginess");
	Settings.Night.bloomExponent = TheSettingManager->GetSettingF("Shaders.Lens.Night", "BloomExponent");
	Settings.Night.strength = TheSettingManager->GetSettingF("Shaders.Lens.Night", "Strength");
	Settings.Night.smudginess = TheSettingManager->GetSettingF("Shaders.Lens.Night", "Smudginess");
	Settings.Interiors.bloomExponent = TheSettingManager->GetSettingF("Shaders.Lens.Interiors", "BloomExponent");
	Settings.Interiors.strength = TheSettingManager->GetSettingF("Shaders.Lens.Interiors", "Strength");
	Settings.Interiors.smudginess = TheSettingManager->GetSettingF("Shaders.Lens.Interiors", "Smudginess");
}

void LensEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_LensData", &Constants.Data);
}

bool LensEffect::ShouldRender() 
{
	return !TheShaderManager->GameState.isUnderwater && Constants.Data.x > 0;
};
