#include "Specular.h"

void SpecularEffect::UpdateConstants() {

	float rainyPercent = TheShaderManager->Effects.Rain->Constants.RainAnimator.GetValue();


	// handle transition by interpolating previous and current weather settings
	Constants.Data.x = std::lerp(Settings.Exterior.SpecLumaTreshold, Settings.Rain.SpecLumaTreshold, rainyPercent);
	Constants.Data.y = std::lerp(Settings.Exterior.BlurMultiplier, Settings.Rain.BlurMultiplier, rainyPercent);
	Constants.Data.z = std::lerp(Settings.Exterior.Glossiness, Settings.Rain.Glossiness, rainyPercent);
	Constants.Data.w = std::lerp(Settings.Exterior.DistanceFade, Settings.Rain.DistanceFade, rainyPercent);
	Constants.EffectStrength.x = std::lerp(Settings.Exterior.SpecularStrength, Settings.Rain.SpecularStrength, rainyPercent);
	Constants.EffectStrength.y = std::lerp(Settings.Exterior.SkyTintStrength, Settings.Rain.SkyTintStrength, rainyPercent);
	Constants.EffectStrength.z = std::lerp(Settings.Exterior.FresnelStrength, Settings.Rain.FresnelStrength, rainyPercent);
	Constants.EffectStrength.w = std::lerp(Settings.Exterior.SkyTintSaturation, Settings.Rain.SkyTintSaturation, rainyPercent);


}

void SpecularEffect::UpdateSettings() {
	Settings.Exterior.SpecLumaTreshold = TheSettingManager->GetSettingF("Shaders.Specular.Exterior", "SpecLumaTreshold");
	Settings.Exterior.BlurMultiplier = TheSettingManager->GetSettingF("Shaders.Specular.Exterior", "BlurMultiplier");
	Settings.Exterior.Glossiness = TheSettingManager->GetSettingF("Shaders.Specular.Exterior", "Glossiness");
	Settings.Exterior.DistanceFade = TheSettingManager->GetSettingF("Shaders.Specular.Exterior", "DistanceFade");
	Settings.Exterior.SpecularStrength = TheSettingManager->GetSettingF("Shaders.Specular.Exterior", "SpecularStrength");
	Settings.Exterior.SkyTintStrength = TheSettingManager->GetSettingF("Shaders.Specular.Exterior", "SkyTintStrength");
	Settings.Exterior.FresnelStrength = TheSettingManager->GetSettingF("Shaders.Specular.Exterior", "FresnelStrength");
	Settings.Exterior.SkyTintSaturation = TheSettingManager->GetSettingF("Shaders.Specular.Exterior", "SkyTintSaturation");

	Settings.Rain.SpecLumaTreshold = TheSettingManager->GetSettingF("Shaders.Specular.Rain", "SpecLumaTreshold");
	Settings.Rain.BlurMultiplier = TheSettingManager->GetSettingF("Shaders.Specular.Rain", "BlurMultiplier");
	Settings.Rain.Glossiness = TheSettingManager->GetSettingF("Shaders.Specular.Rain", "Glossiness");
	Settings.Rain.DistanceFade = TheSettingManager->GetSettingF("Shaders.Specular.Rain", "DistanceFade");
	Settings.Rain.SpecularStrength = TheSettingManager->GetSettingF("Shaders.Specular.Rain", "SpecularStrength");
	Settings.Rain.SkyTintStrength = TheSettingManager->GetSettingF("Shaders.Specular.Rain", "SkyTintStrength");
	Settings.Rain.FresnelStrength = TheSettingManager->GetSettingF("Shaders.Specular.Rain", "FresnelStrength");
	Settings.Rain.SkyTintSaturation = TheSettingManager->GetSettingF("Shaders.Specular.Rain", "SkyTintSaturation");
}

void SpecularEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_SpecularData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_SpecularEffects", &Constants.EffectStrength);
}

bool SpecularEffect::ShouldRender() { 
	return TheShaderManager->GameState.isExterior && !TheShaderManager->GameState.isUnderwater; 
};
