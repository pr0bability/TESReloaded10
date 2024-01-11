#include "Grass.h"

void GrassShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_GrassScale", &Constants.Scale);
}

void GrassShaders::UpdateSettings() {
	if (!Enabled) return;

	Constants.Scale.x = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleX");
	Constants.Scale.y = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleY");
	Constants.Scale.z = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleZ");

	switch (TheSettingManager->GetSettingI("Shaders.Grass.Main", "GrassDensity")) {
	case 1:
		*Pointers::Settings::MinGrassSize = 240;
		*Pointers::Settings::TexturePctThreshold = 0.3f;
		break;
	case 2:
		*Pointers::Settings::MinGrassSize = 240;
		*Pointers::Settings::TexturePctThreshold = 0.2f;
		break;
	case 3:
		*Pointers::Settings::MinGrassSize = 120;
		*Pointers::Settings::TexturePctThreshold = 0.3f;
		break;
	case 4:
		*Pointers::Settings::MinGrassSize = 120;
		*Pointers::Settings::TexturePctThreshold = 0.2f;
		break;
	case 5:
		*Pointers::Settings::MinGrassSize = 80;
		*Pointers::Settings::TexturePctThreshold = 0.3f;
		break;
	case 6:
		*Pointers::Settings::MinGrassSize = 80;
		*Pointers::Settings::TexturePctThreshold = 0.2f;
		break;
	case 7:
		*Pointers::Settings::MinGrassSize = 20;
		*Pointers::Settings::TexturePctThreshold = 0.3f;
		break;
	case 8:
		*Pointers::Settings::MinGrassSize = 20;
		*Pointers::Settings::TexturePctThreshold = 0.2f;
		break;
	default:
		break;
	}

	float minDistance = TheSettingManager->GetSettingF("Shaders.Grass.Main", "MinDistance");
	if (minDistance) *Pointers::Settings::GrassStartFadeDistance = minDistance;
	float maxDistance = TheSettingManager->GetSettingF("Shaders.Grass.Main", "MaxDistance");
	if (maxDistance) *Pointers::Settings::GrassEndDistance = maxDistance;

	if (TheSettingManager->GetSettingI("Shaders.Grass.Main", "WindEnabled")) {
		*Pointers::Settings::GrassWindMagnitudeMax = *Pointers::ShaderParams::GrassWindMagnitudeMax = TheSettingManager->GetSettingF("Shaders.Grass.Main", "WindCoefficient") * TheShaderManager->ShaderConst.windSpeed;
		*Pointers::Settings::GrassWindMagnitudeMin = *Pointers::ShaderParams::GrassWindMagnitudeMin = *Pointers::Settings::GrassWindMagnitudeMax * 0.5f;
	}

}

void GrassShaders::UpdateConstants() {}
