#include "Terrain.h"

void TerrainShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_TerrainData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_TerrainExtraData", &Constants.ExtraData);
}


void TerrainShaders::UpdateSettings() {
	Constants.ExtraData.x = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "UsePBR");
	Constants.ExtraData.y = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "LODNoiseScale");
	Constants.ExtraData.z = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "TerrainSaturation");

	if (Constants.ExtraData.x) {
		// use PBR
		Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "Metallicness");
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "Roughness");
	}
	else {
		Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "Fresnel");
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "Specular");
	}

	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "LightingScale");
	Constants.Data.w = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "AmbientScale");
}

void TerrainShaders::UpdateConstants() {};

