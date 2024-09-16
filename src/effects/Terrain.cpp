#include "Terrain.h"

void TerrainShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_TerrainData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_TerrainExtraData", &Constants.ExtraData);
	TheShaderManager->RegisterConstant("TESR_TerrainParallaxData", &ParallaxConstants.Data);
	TheShaderManager->RegisterConstant("TESR_TerrainParallaxExtraData", &ParallaxConstants.ExtraData);
}


void TerrainShaders::UpdateSettings() {
	usePBR = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "UsePBR");
	LodNoiseScale = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "LODNoiseScale");

	Settings.Default.Saturation = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "TerrainSaturation");
	Settings.Default.Metallicness = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "Metallicness");
	Settings.Default.Roughness = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "Roughness");
	Settings.Default.Fresnel = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "Fresnel");
	Settings.Default.Specular = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "Specular");
	Settings.Default.LightScale = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "LightingScale");
	Settings.Default.AmbientScale = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "AmbientScale");

	Settings.Rain.Saturation = TheSettingManager->GetSettingF("Shaders.Terrain.Rain", "TerrainSaturation");
	Settings.Rain.Metallicness = TheSettingManager->GetSettingF("Shaders.Terrain.Rain", "Metallicness");
	Settings.Rain.Roughness = TheSettingManager->GetSettingF("Shaders.Terrain.Rain", "Roughness");
	Settings.Rain.Fresnel = TheSettingManager->GetSettingF("Shaders.Terrain.Rain", "Fresnel");
	Settings.Rain.Specular = TheSettingManager->GetSettingF("Shaders.Terrain.Rain", "Specular");
	Settings.Rain.LightScale = TheSettingManager->GetSettingF("Shaders.Terrain.Rain", "LightingScale");
	Settings.Rain.AmbientScale = TheSettingManager->GetSettingF("Shaders.Terrain.Rain", "AmbientScale");

	Settings.Night.Saturation = TheSettingManager->GetSettingF("Shaders.Terrain.Night", "TerrainSaturation");
	Settings.Night.Metallicness = TheSettingManager->GetSettingF("Shaders.Terrain.Night", "Metallicness");
	Settings.Night.Roughness = TheSettingManager->GetSettingF("Shaders.Terrain.Night", "Roughness");
	Settings.Night.Fresnel = TheSettingManager->GetSettingF("Shaders.Terrain.Night", "Fresnel");
	Settings.Night.Specular = TheSettingManager->GetSettingF("Shaders.Terrain.Night", "Specular");
	Settings.Night.LightScale = TheSettingManager->GetSettingF("Shaders.Terrain.Night", "LightingScale");
	Settings.Night.AmbientScale = TheSettingManager->GetSettingF("Shaders.Terrain.Night", "AmbientScale");

	Settings.NightRain.Saturation = TheSettingManager->GetSettingF("Shaders.Terrain.NightRain", "TerrainSaturation");
	Settings.NightRain.Metallicness = TheSettingManager->GetSettingF("Shaders.Terrain.NightRain", "Metallicness");
	Settings.NightRain.Roughness = TheSettingManager->GetSettingF("Shaders.Terrain.NightRain", "Roughness");
	Settings.NightRain.Fresnel = TheSettingManager->GetSettingF("Shaders.Terrain.NightRain", "Fresnel");
	Settings.NightRain.Specular = TheSettingManager->GetSettingF("Shaders.Terrain.NightRain", "Specular");
	Settings.NightRain.LightScale = TheSettingManager->GetSettingF("Shaders.Terrain.NightRain", "LightingScale");
	Settings.NightRain.AmbientScale = TheSettingManager->GetSettingF("Shaders.Terrain.NightRain", "AmbientScale");

	ParallaxSettings.Enabled = TheSettingManager->GetSettingF("Shaders.Terrain.Parallax", "Enabled");
	ParallaxSettings.HighQuality = TheSettingManager->GetSettingF("Shaders.Terrain.Parallax", "HighQuality");
	ParallaxSettings.Shadows = TheSettingManager->GetSettingF("Shaders.Terrain.Parallax", "Shadows");
	ParallaxSettings.Height = TheSettingManager->GetSettingF("Shaders.Terrain.Parallax", "Height");
	ParallaxSettings.MaxDistance = TheSettingManager->GetSettingF("Shaders.Terrain.Parallax", "MaxDistance");
	ParallaxSettings.Range = TheSettingManager->GetSettingF("Shaders.Terrain.Parallax", "Range");
	ParallaxSettings.BlendRange = TheSettingManager->GetSettingF("Shaders.Terrain.Parallax", "BlendRange");
	ParallaxSettings.ShadowsFade = TheSettingManager->GetSettingF("Shaders.Terrain.Parallax", "ShadowsFade");
}

void TerrainShaders::UpdateConstants() {
	// get max value between rain animator and puddle animator
	float rainFactor = max(TheShaderManager->Effects.WetWorld->Constants.Data.x, TheShaderManager->Effects.WetWorld->Constants.Data.z);

	if (!TheShaderManager->GameState.isExterior) return;

	Constants.ExtraData.x = usePBR;
	Constants.ExtraData.y = LodNoiseScale;
	Constants.ExtraData.z = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.Saturation, Settings.Night.Saturation, 0.0),
		TheShaderManager->GetTransitionValue(Settings.Rain.Saturation, Settings.NightRain.Saturation, 0.0), rainFactor);

	if (usePBR) {
		Constants.Data.x = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.Metallicness, Settings.Night.Metallicness, 0.0),
			TheShaderManager->GetTransitionValue(Settings.Rain.Metallicness, Settings.NightRain.Metallicness, 0.0), rainFactor);
		Constants.Data.y = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.Roughness, Settings.Night.Roughness, 0.0),
			TheShaderManager->GetTransitionValue(Settings.Rain.Roughness, Settings.NightRain.Roughness, 0.0), rainFactor);
	}
	else {
		Constants.Data.x = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.Fresnel, Settings.Night.Fresnel, 0.0),
			TheShaderManager->GetTransitionValue(Settings.Rain.Fresnel, Settings.NightRain.Fresnel, 0.0), rainFactor);
		Constants.Data.y = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.Specular, Settings.Night.Specular, 0.0),
			TheShaderManager->GetTransitionValue(Settings.Rain.Specular, Settings.NightRain.Specular, 0.0), rainFactor);
	}

	Constants.Data.z = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.LightScale, Settings.Night.LightScale, 0.0),
		TheShaderManager->GetTransitionValue(Settings.Rain.LightScale, Settings.NightRain.LightScale, 0.0), rainFactor);
	Constants.Data.w = std::lerp(TheShaderManager->GetTransitionValue(Settings.Default.AmbientScale, Settings.Night.AmbientScale, 0.0),
		TheShaderManager->GetTransitionValue(Settings.Rain.AmbientScale, Settings.NightRain.AmbientScale, 0.0), rainFactor);

	ParallaxConstants.Data.x = ParallaxSettings.Enabled;
	ParallaxConstants.Data.y = ParallaxSettings.HighQuality;
	ParallaxConstants.Data.z = ParallaxSettings.Shadows;
	ParallaxConstants.Data.w = ParallaxSettings.Height;

	ParallaxConstants.ExtraData.x = ParallaxSettings.MaxDistance;
	ParallaxConstants.ExtraData.y = ParallaxSettings.Range;
	ParallaxConstants.ExtraData.z = ParallaxSettings.BlendRange;
	ParallaxConstants.ExtraData.w = ParallaxSettings.ShadowsFade;
};

