#include "Specular.h"

void SpecularEffect::UpdateConstants() {

	float rainyPercent = TheShaderManager->Effects.Rain->Constants.RainAnimator.GetValue();
	const char* ext = "Shaders.Specular.Exterior";
	const char* rain = "Shaders.Specular.Rain";

	// handle transition by interpolating previous and current weather settings
	Constants.Data.x = std::lerp(TheSettingManager->GetSettingF(ext, "SpecLumaTreshold"), TheSettingManager->GetSettingF(rain, "SpecLumaTreshold"), rainyPercent);
	Constants.Data.y = std::lerp(TheSettingManager->GetSettingF(ext, "BlurMultiplier"), TheSettingManager->GetSettingF(rain, "BlurMultiplier"), rainyPercent);
	Constants.Data.z = std::lerp(TheSettingManager->GetSettingF(ext, "Glossiness"), TheSettingManager->GetSettingF(rain, "Glossiness"), rainyPercent);
	Constants.Data.w = std::lerp(TheSettingManager->GetSettingF(ext, "DistanceFade"), TheSettingManager->GetSettingF(rain, "DistanceFade"), rainyPercent);
	Constants.EffectStrength.x = std::lerp(TheSettingManager->GetSettingF(ext, "SpecularStrength"), TheSettingManager->GetSettingF(rain, "SpecularStrength"), rainyPercent);
	Constants.EffectStrength.y = std::lerp(TheSettingManager->GetSettingF(ext, "SkyTintStrength"), TheSettingManager->GetSettingF(rain, "SkyTintStrength"), rainyPercent);
	Constants.EffectStrength.z = std::lerp(TheSettingManager->GetSettingF(ext, "FresnelStrength"), TheSettingManager->GetSettingF(rain, "FresnelStrength"), rainyPercent);
	Constants.EffectStrength.w = std::lerp(TheSettingManager->GetSettingF(ext, "SkyTintSaturation"), TheSettingManager->GetSettingF(rain, "SkyTintSaturation"), rainyPercent);

}

void SpecularEffect::UpdateSettings() {}

void SpecularEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_SpecularData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_SpecularEffects", &Constants.EffectStrength);
}

bool SpecularEffect::ShouldRender() { 
	return TheShaderManager->GameState.isExterior && !TheShaderManager->GameState.isUnderwater; 
};
