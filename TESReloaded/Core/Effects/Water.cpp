#include "Water.h"

void WaterShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_WaterDeepColor", &Constants.deepColor);
	TheShaderManager->RegisterConstant("TESR_WaterShallowColor", &Constants.shallowColor);
	TheShaderManager->RegisterConstant("TESR_WaterLODColor", &Constants.LODColor);
	TheShaderManager->RegisterConstant("TESR_WaterFog", &Constants.Fog);
	TheShaderManager->RegisterConstant("TESR_WaterCoefficients", &Constants.Default.waterCoefficients);
	TheShaderManager->RegisterConstant("TESR_WaveParams", &Constants.Default.waveParams);
	TheShaderManager->RegisterConstant("TESR_WaterVolume", &Constants.Default.waterVolume);
	TheShaderManager->RegisterConstant("TESR_WaterSettings", &Constants.Default.waterSettings);
	TheShaderManager->RegisterConstant("TESR_WaterShorelineParams", &Constants.Default.shorelineParams);
	TheShaderManager->RegisterConstant("TESR_PlacedWaterCoefficients", &Constants.Placed.waterCoefficients);
	TheShaderManager->RegisterConstant("TESR_PlacedWaveParams", &Constants.Placed.waveParams);
	TheShaderManager->RegisterConstant("TESR_PlacedWaterVolume", &Constants.Placed.waterVolume);
	TheShaderManager->RegisterConstant("TESR_PlacedWaterSettings", &Constants.Placed.waterSettings);
	TheShaderManager->RegisterConstant("TESR_PlacedWaterShorelineParams", &Constants.Placed.shorelineParams);
}


void WaterShaders::UpdateConstants() {

	TESWaterForm* currentWater = NULL;
	float height = Tes->GetWaterHeight(Player, WorldSceneGraph, &currentWater);

	// get water height based on player position
	Constants.Default.waterSettings.x = height;
	Constants.Default.waterSettings.z = TheShaderManager->GameState.isUnderwater;
	Constants.Placed.waterSettings.x = Constants.Default.waterSettings.x;
	Constants.Placed.waterSettings.z = Constants.Default.waterSettings.z;

	TESWorldSpace* worldSpace = Player->GetWorldSpace();
	if (worldSpace) {
		TESWaterForm* LODWater = worldSpace->waterFormLast;
		if (LODWater)
			Constants.LODColor = LODWater->GetShallowColor()->toD3DXVECTOR4();
	}

	if (currentWater) {
		Constants.deepColor = currentWater->GetDeepColor()->toD3DXVECTOR4();
		Constants.shallowColor = currentWater->GetShallowColor()->toD3DXVECTOR4();
		Constants.Fog.x = currentWater->properties.fogNearUW;
		Constants.Fog.y = currentWater->properties.fogFarUW;
		Constants.Fog.z = currentWater->properties.fogAmountUW;
		Constants.Fog.z = 1;
	}

	// caustics strength
	Constants.Default.waterVolume.x = Constants.Placed.waterVolume.x = causticsStrength * TheShaderManager->ShaderConst.sunGlare;
}

void WaterShaders::UpdateSettings() {
	//SettingsWaterStruct* sws = NULL;

	TESWaterForm* currentWater = Player->parentCell->GetWaterForm();
	const char* sectionName = "Shaders.Water.Default";
	if (!TheShaderManager->GameState.isExterior) sectionName = "Shaders.Water.Interiors";
	
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

	Constants.Default.waterCoefficients.x = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_R");
	Constants.Default.waterCoefficients.y = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_G");
	Constants.Default.waterCoefficients.z = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_B");
	Constants.Default.waterCoefficients.w = TheSettingManager->GetSettingF(sectionName, "inScattCoeff");
	causticsStrength = TheSettingManager->GetSettingF(sectionName, "causticsStrength"); // later modified by current sunglare
	Constants.Default.waveParams.x = TheSettingManager->GetSettingF(sectionName, "choppiness");
	Constants.Default.waveParams.y = TheSettingManager->GetSettingF(sectionName, "waveWidth");
	Constants.Default.waveParams.z = TheSettingManager->GetSettingF(sectionName, "waveSpeed");
	Constants.Default.waveParams.w = TheSettingManager->GetSettingF(sectionName, "reflectivity");
	Constants.Default.waterSettings.y = TheSettingManager->GetSettingF(sectionName, "depthDarkness");
	Constants.Default.waterVolume.y = TheSettingManager->GetSettingF(sectionName, "shoreFactor");
	Constants.Default.waterVolume.z = TheSettingManager->GetSettingF(sectionName, "turbidity");
	Constants.Default.waterVolume.w = TheSettingManager->GetSettingF(sectionName, "causticsStrengthS");
	Constants.Default.shorelineParams.x = TheSettingManager->GetSettingF(sectionName, "shoreMovement");
	Constants.Default.waterSettings.w = TheSettingManager->GetSettingF(sectionName, "refractionPower");


	Constants.Placed.waveParams.x = TheSettingManager->GetSettingF("Shaders.Water.Placed", "choppiness");
	Constants.Placed.waveParams.y = TheSettingManager->GetSettingF("Shaders.Water.Placed", "waveWidth");
	Constants.Placed.waveParams.z = TheSettingManager->GetSettingF("Shaders.Water.Placed", "waveSpeed");
	Constants.Placed.waveParams.w = TheSettingManager->GetSettingF("Shaders.Water.Placed", "reflectivity");
	Constants.Placed.waterSettings.y = TheSettingManager->GetSettingF("Shaders.Water.Placed", "depthDarkness");
	Constants.Placed.waterVolume.y = TheSettingManager->GetSettingF("Shaders.Water.Placed", "shoreFactor");
	Constants.Placed.waterVolume.z = TheSettingManager->GetSettingF("Shaders.Water.Placed", "turbidity");
	Constants.Placed.waterVolume.w = TheSettingManager->GetSettingF("Shaders.Water.Placed", "causticsStrengthS");
	Constants.Placed.shorelineParams.x = TheSettingManager->GetSettingF("Shaders.Water.Placed", "shoreMovement");
	Constants.Placed.waterSettings.w = TheSettingManager->GetSettingF("Shaders.Water.Placed", "refractionPower");
}