#include "Terrain.h"

void TerrainShaders::RegisterConstants() {
	TheShaderManager->ConstantsTable["TESR_TerrainData"] = &Constants.Data;
}


void TerrainShaders::UpdateSettings() {
	if (TheSettingManager->GetMenuShaderEnabled("Terrain")) {
		Constants.Data.x = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "DistantSpecular");
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "DistantNoise");
		Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "NearSpecular");
		Constants.Data.w = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "MiddleSpecular");
	}
}

void TerrainShaders::UpdateConstants() {};

