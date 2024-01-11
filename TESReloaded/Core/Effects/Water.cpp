#include "Water.h"

void WaterShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_WaterCoefficients", &Constants.waterCoefficients);
	TheShaderManager->RegisterConstant("TESR_WaveParams", &Constants.waveParams);
	TheShaderManager->RegisterConstant("TESR_WaterVolume", &Constants.waterVolume);
	TheShaderManager->RegisterConstant("TESR_WaterSettings", &Constants.waterSettings);
	TheShaderManager->RegisterConstant("TESR_WaterDeepColor", &Constants.deepColor);
	TheShaderManager->RegisterConstant("TESR_WaterShallowColor", &Constants.shallowColor);
	TheShaderManager->RegisterConstant("TESR_WaterShorelineParams", &Constants.shorelineParams);
}


void WaterShaders::UpdateConstants() {

	// get water height based on player position
	Constants.waterSettings.x = Tes->GetWaterHeight(Player, WorldSceneGraph);
	Constants.waterSettings.z = TheShaderManager->GameState.isUnderwater;

	TESWaterForm* currentWater = Player->parentCell->GetWaterForm();
	RGBA* rgba = NULL;

	if (currentWater) {
		rgba = currentWater->GetDeepColor();
		Constants.deepColor.x = rgba->r / 255.0f;
		Constants.deepColor.y = rgba->g / 255.0f;
		Constants.deepColor.z = rgba->b / 255.0f;
		Constants.deepColor.w = rgba->a / 255.0f;
		rgba = currentWater->GetShallowColor();
		Constants.shallowColor.x = rgba->r / 255.0f;
		Constants.shallowColor.y = rgba->g / 255.0f;
		Constants.shallowColor.z = rgba->b / 255.0f;
		Constants.shallowColor.w = rgba->a / 255.0f;
	}

	// caustics strength
	Constants.waterVolume.x = causticsStrength * TheShaderManager->ShaderConst.sunGlare;
}

void WaterShaders::UpdateSettings() {
	//SettingsWaterStruct* sws = NULL;

	TESWaterForm* currentWater = Player->parentCell->GetWaterForm();
	const char* sectionName = "Shaders.Water.Default";
	if (currentWater) {
		UInt32 WaterType = currentWater->GetWaterType();
		if (WaterType == TESWaterForm::WaterType::kWaterType_Blood)
			sectionName = "Shaders.Water.Blood";
		else if (WaterType == TESWaterForm::WaterType::kWaterType_Lava)
			sectionName = "Shaders.Water.Lava";

		// world space specific settings. TODO Reimplement with Toml
		//else if (!(sws = TheSettingManager->GetSettingsWater(currentCell->GetEditorName())) && currentWorldSpace)
		//	sws = TheSettingManager->GetSettingsWater(currentWorldSpace->GetEditorName());
	}

	Constants.waterCoefficients.x = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_R");
	Constants.waterCoefficients.y = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_G");
	Constants.waterCoefficients.z = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_B");
	Constants.waterCoefficients.w = TheSettingManager->GetSettingF(sectionName, "inScattCoeff");
	Constants.waveParams.x = TheSettingManager->GetSettingF(sectionName, "choppiness");
	Constants.waveParams.y = TheSettingManager->GetSettingF(sectionName, "waveWidth");
	Constants.waveParams.z = TheSettingManager->GetSettingF(sectionName, "waveSpeed");
	Constants.waveParams.w = TheSettingManager->GetSettingF(sectionName, "reflectivity");
	Constants.waterSettings.y = TheSettingManager->GetSettingF(sectionName, "depthDarkness");
	causticsStrength = TheSettingManager->GetSettingF(sectionName, "causticsStrength"); // later modified by current sunglare
	Constants.waterVolume.y = TheSettingManager->GetSettingF(sectionName, "shoreFactor");
	Constants.waterVolume.z = TheSettingManager->GetSettingF(sectionName, "turbidity");
	Constants.waterVolume.w = TheSettingManager->GetSettingF(sectionName, "causticsStrengthS");
	Constants.shorelineParams.x = TheSettingManager->GetSettingF(sectionName, "shoreMovement");
}