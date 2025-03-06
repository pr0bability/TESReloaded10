#include "ShadowsExterior.h"

void ShadowsExteriorEffect::UpdateConstants() {

	Constants.ShadowFade.x = 0; // Fade 1.0 == no shadows
	if (TheShaderManager->GameState.isExterior) {
		Constants.ShadowFade.x = smoothStep(0.5f, 0.1f, abs(TheShaderManager->GameState.dayLight - 0.5f)); // fade shadows to 0 at sunrise/sunset.  

		TimeGlobals* GameTimeGlobals = TimeGlobals::Get();
		float DaysPassed = GameTimeGlobals->GameDaysPassed ? GameTimeGlobals->GameDaysPassed->data : 1.0f;

		if(TheShaderManager->GameState.isDayTime < 0.5f) {
			// at night time, fade based on moonphase
			// moonphase goes from 0 to 8
			float MoonPhase = (fmod(DaysPassed, 8 * Tes->sky->firstClimate->phaseLength & 0x3F)) / (Tes->sky->firstClimate->phaseLength & 0x3F);

			float PI = 3.1416f; // use cos curve to fade moon light shadows strength
			MoonPhase = std::lerp(-PI, PI, MoonPhase / 8) - PI / 4; // map moonphase to 1/2PI/2PI + 1/2

			// map MoonVisibility to MinNightDarkness/1 range
			float nightMinDarkness = 1 - Settings.Exteriors.NightMinDarkness;
			float MoonVisibility = std::lerp((float)0.0, nightMinDarkness, (float)(cos(MoonPhase) * 0.5 + 0.5));
			Constants.ShadowFade.x = std::lerp(MoonVisibility, (float)1.0, Constants.ShadowFade.x);
		}

		if (TheShaderManager->GameState.isDayTimeChanged) {
			// pass the enabled/disabled property of the pointlight shadows to the shadowfade constant
			bool usePointLights = (TheShaderManager->GameState.isDayTime > 0.5) ? Settings.Exteriors.UsePointShadowsDay: Settings.Exteriors.UsePointShadowsNight;
			Constants.ShadowFade.z = usePointLights;
		}
		Constants.ShadowFade.y = Settings.Exteriors.Enabled && Enabled;
		Constants.ShadowFade.w = Constants.ShadowMapRadius.w; //furthest distance for point lights shadows

		// Update constants used by shadow shaders: x=quality, y=darkness
		Constants.Data.x = Settings.Exteriors.Quality;
		//if (Enabled) Constants.ShadowData->x = -1; // Disable the forward shadowing
		Constants.Data.y = Settings.Exteriors.Darkness;

		// Mode and format data. x=mode, y=bits per pixel
		Constants.FormatData.x = Settings.ShadowMaps.Mode;
		Constants.FormatData.y = Settings.ShadowMaps.FormatBits;
	}
	else {
		// pass the enabled/disabled property of the shadow maps to the shadowfade constant
		Constants.ShadowFade.y = TheShaderManager->Effects.ShadowsInteriors->Enabled;
		Constants.ShadowFade.z = 1; // z enables point lights
		Constants.ShadowFade.w = Settings.Interiors.DrawDistance; //furthest distance for point lights shadows

		// Update constants used by shadow shaders: x=quality, y=darkness
		Constants.Data.x = Settings.Interiors.Quality;
		//if (TheShaderManager->Effects.ShadowsInteriors->Enabled) Constants.Data.x = -1; // Disable the forward shadowing
		Constants.Data.y = Settings.Interiors.Darkness;
		Constants.Data.z = 1.0f / (float)Settings.Interiors.ShadowCubeMapSize;
	}
}

bool ShadowsExteriorEffect::UpdateSettingsFromQuality(int quality) {
	bool cascadeSettingsChanged = false;
	
	D3DFORMAT oldFormat = Settings.ShadowMaps.Format;
	int oldCascadeResolution = Settings.ShadowMaps.CascadeResolution;
	bool oldMSAA = Settings.ShadowMaps.MSAA;
	
	// Custom settings.
	if (quality < 0 || quality > 3) {
		Settings.ShadowMaps.Mode = std::clamp(TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ShadowMaps", "Mode"), 0, Modes-1);
		Settings.ShadowMaps.FormatBits = std::clamp(TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ShadowMaps", "Format"), 0, FormatBits-1);

		Settings.ShadowMaps.Distance = std::clamp(TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ShadowMaps", "Distance"), 0.001f, 1.0f);
		Settings.ShadowMaps.CascadeLambda = std::clamp(TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ShadowMaps", "CascadeLambda"), 0.0f, 1.0f);
		Settings.ShadowMaps.LimitFrequency = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ShadowMaps", "LimitFrequency");

		Settings.ShadowMaps.CascadeResolution = (std::clamp(TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ShadowMaps", "CascadeResolution"), 0, 2) + 2) * 512;

		Settings.ShadowMaps.MSAA = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ShadowMaps", "MSAA");

		// Mipmaps and anisotropy are disabled due to deferred shadows - derivatives are messed up and causing artifacts.
		// https://aras-p.info/blog/2010/01/07/screenspace-vs-mip-mapping/
		/*bool oldMips = Settings.ShadowMaps.Mipmaps;
		Settings.ShadowMaps.Mipmaps = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ShadowMaps", "Mipmaps");

		if (oldMips != Settings.ShadowMaps.Mipmaps)
			cascadeSettingsChanged = true;

		Settings.ShadowMaps.Anisotropy = (std::clamp(TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ShadowMaps", "Anisotropy"), 0, 2)) * 8;*/

		Settings.ShadowMaps.Prefilter = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ShadowMaps", "Prefilter");

		for (int shadowType = 0; shadowType <= MapOrtho; shadowType++) {
			char sectionName[256] = "Shaders.ShadowsExteriors.Forms";
			switch (shadowType) {
			case MapNear:
				strcat(sectionName, "Near");
				break;
			case MapMiddle:
				strcat(sectionName, "Middle");
				break;
			case MapFar:
				strcat(sectionName, "Far");
				break;
			case MapLod:
				strcat(sectionName, "Lod");
				break;
			case MapOrtho:
				strcat(sectionName, "Ortho");
				break;
			}
			ShadowMapSettings* ShadowMap = &ShadowMaps[shadowType];

			ShadowMap->Forms.AlphaEnabled = TheSettingManager->GetSettingI(sectionName, "AlphaEnabled");
			ShadowMap->Forms.Activators = TheSettingManager->GetSettingI(sectionName, "Activators");
			ShadowMap->Forms.Actors = TheSettingManager->GetSettingI(sectionName, "Actors");
			ShadowMap->Forms.Apparatus = TheSettingManager->GetSettingI(sectionName, "Apparatus");
			ShadowMap->Forms.Books = TheSettingManager->GetSettingI(sectionName, "Books");
			ShadowMap->Forms.Containers = TheSettingManager->GetSettingI(sectionName, "Containers");
			ShadowMap->Forms.Doors = TheSettingManager->GetSettingI(sectionName, "Doors");
			ShadowMap->Forms.Furniture = TheSettingManager->GetSettingI(sectionName, "Furniture");
			ShadowMap->Forms.Misc = TheSettingManager->GetSettingI(sectionName, "Misc");
			ShadowMap->Forms.Statics = TheSettingManager->GetSettingI(sectionName, "Statics");
			ShadowMap->Forms.Terrain = TheSettingManager->GetSettingI(sectionName, "Terrain");
			ShadowMap->Forms.Trees = TheSettingManager->GetSettingI(sectionName, "Trees");
			ShadowMap->Forms.Lod = TheSettingManager->GetSettingI(sectionName, "Lod");
			ShadowMap->Forms.MinRadius = TheSettingManager->GetSettingF(sectionName, "MinRadius");
			ShadowMap->Forms.OrigMinRadius = TheSettingManager->GetSettingF(sectionName, "MinRadius");
		};
	}
	else {
		for (int shadowType = 0; shadowType <= MapOrtho; shadowType++) {
			char sectionName[256] = "Shaders.ShadowsExteriors.Forms";
			switch (shadowType) {
			case MapNear:
				strcat(sectionName, "Near");
				break;
			case MapMiddle:
				strcat(sectionName, "Middle");
				break;
			case MapFar:
				strcat(sectionName, "Far");
				break;
			case MapLod:
				strcat(sectionName, "Lod");
				break;
			case MapOrtho:
				strcat(sectionName, "Ortho");
				break;
			}
			ShadowMapSettings* ShadowMap = &ShadowMaps[shadowType];

			ShadowMap->Forms.AlphaEnabled = (shadowType == MapOrtho) ? 0 : 1;
			ShadowMap->Forms.Activators = (shadowType < MapLod) ? 1 : 0;
			ShadowMap->Forms.Actors = (shadowType < MapLod) ? 1 : 0;
			ShadowMap->Forms.Apparatus = 0;
			ShadowMap->Forms.Books = (shadowType < MapFar) ? 1 : 0;
			ShadowMap->Forms.Containers = (shadowType < MapLod) ? 1 : 0;
			ShadowMap->Forms.Doors = (shadowType == MapOrtho) ? 0 : 1;
			ShadowMap->Forms.Furniture = (shadowType < MapLod) ? 1 : 0;
			ShadowMap->Forms.Misc = 1;
			ShadowMap->Forms.Statics = 1;
			ShadowMap->Forms.Terrain = 1;
			ShadowMap->Forms.Trees = 1;
			ShadowMap->Forms.Lod = (shadowType < MapFar || quality < 2) ? 0 : 1;
			ShadowMap->Forms.MinRadius = (MapFar <= shadowType && shadowType <= MapLod) ? 10.0f : 1.0f;
			ShadowMap->Forms.OrigMinRadius = (MapFar <= shadowType && shadowType <= MapLod) ? 10.0f : 1.0f;
		};

		Settings.ShadowMaps.CascadeLambda = 0.9f;
		Settings.ShadowMaps.LimitFrequency = 1;
		Settings.ShadowMaps.MSAA = 1;
		Settings.ShadowMaps.Prefilter = 1;

		switch (quality) {
		case 0:
			Settings.ShadowMaps.Mode = 0;
			Settings.ShadowMaps.FormatBits = 0;
			Settings.ShadowMaps.Distance = 0.01f;
			Settings.ShadowMaps.CascadeResolution = 1024;
			Settings.ShadowMaps.MSAA = 0;
			break;
		case 1:
			Settings.ShadowMaps.Mode = 0;
			Settings.ShadowMaps.FormatBits = 1;
			Settings.ShadowMaps.Distance = 0.01f;
			Settings.ShadowMaps.CascadeResolution = 1024;
			break;
		case 2:
			Settings.ShadowMaps.Mode = 2;
			Settings.ShadowMaps.FormatBits = 1;
			Settings.ShadowMaps.Distance = 0.015f;
			Settings.ShadowMaps.CascadeResolution = 2048;
			break;
		case 3:
			Settings.ShadowMaps.Mode = 2;
			Settings.ShadowMaps.FormatBits = 0;
			Settings.ShadowMaps.Distance = 0.02f;
			Settings.ShadowMaps.CascadeResolution = 2048;
			break;
		}
	}
	
	Settings.ShadowMaps.Format = Formats[Settings.ShadowMaps.Mode][Settings.ShadowMaps.FormatBits];

	if (oldFormat != Settings.ShadowMaps.Format)
		cascadeSettingsChanged = true;

	if (oldCascadeResolution != 0 && oldCascadeResolution != Settings.ShadowMaps.CascadeResolution)
		cascadeSettingsChanged = true;

	if (oldMSAA != Settings.ShadowMaps.MSAA)
		cascadeSettingsChanged = true;

	return cascadeSettingsChanged;
}

void ShadowsExteriorEffect::UpdateSettings() {

	Constants.ScreenSpaceData.x = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ScreenSpace", "Enabled") && Enabled;
	Constants.ScreenSpaceData.y = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "BlurRadius");
	Constants.ScreenSpaceData.z = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "RenderDistance");
	Constants.ScreenSpaceData.w = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "Darkness");

	// Generic exterior shadows settings
	Settings.Exteriors.Enabled = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "Enabled");
	Settings.Exteriors.Quality = std::clamp(TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "Quality"), 0, 4);
	Settings.Exteriors.Darkness = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "Darkness");
	Settings.Exteriors.NightMinDarkness = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "NightMinDarkness");
	Settings.Exteriors.OrthoRadius = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "OrthoRadius");
	Settings.Exteriors.OrthoMapResolution = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "OrthoMapResolution");
	Settings.Exteriors.ShadowMode = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "ShadowMode");
	Settings.Exteriors.UsePointShadowsDay = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "UsePointShadowsDay");
	Settings.Exteriors.UsePointShadowsNight = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "UsePointShadowsNight");

	// Shadow maps specific configuration.
	bool cascadeSettingsChanged = UpdateSettingsFromQuality(Settings.Exteriors.Quality);

	Settings.Interiors.Enabled = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Enabled");
	Settings.Interiors.Forms.AlphaEnabled = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "AlphaEnabled");
	Settings.Interiors.Forms.Activators = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Activators");
	Settings.Interiors.Forms.Actors = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Actors");
	Settings.Interiors.Forms.Apparatus = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Apparatus");
	Settings.Interiors.Forms.Books = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Books");
	Settings.Interiors.Forms.Containers = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Containers");
	Settings.Interiors.Forms.Doors = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Doors");
	Settings.Interiors.Forms.Furniture = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Furniture");
	Settings.Interiors.Forms.Misc = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Misc");
	Settings.Interiors.Forms.Statics = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Statics");
	Settings.Interiors.Forms.MinRadius = TheSettingManager->GetSettingF("Shaders.ShadowsInteriors.Main", "MinRadius");
	Settings.Interiors.Quality = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "Quality");
	Settings.Interiors.LightPoints = max(0, min(TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "LightPoints"), ShadowCubeMapsMax));
	Settings.Interiors.TorchesCastShadows = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "TorchesCastShadows");
	Settings.Interiors.ShadowCubeMapSize = TheSettingManager->GetSettingI("Shaders.ShadowsInteriors.Main", "ShadowCubeMapSize");
	Settings.Interiors.Darkness = TheSettingManager->GetSettingF("Shaders.ShadowsInteriors.Main", "Darkness");
	Settings.Interiors.LightRadiusMult = TheSettingManager->GetSettingF("Shaders.ShadowsInteriors.Main", "LightRadiusMult");
	Settings.Interiors.DrawDistance = TheSettingManager->GetSettingF("Shaders.ShadowsInteriors.Main", "DrawDistance");
	Settings.Interiors.UseCastShadowFlag = TheSettingManager->GetSettingF("Shaders.ShadowsInteriors.Main", "UseCastShadowFlag");
	Settings.Interiors.PlayerShadowFirstPerson = TheSettingManager->GetSettingF("Shaders.ShadowsInteriors.Main", "PlayerShadowFirstPerson");
	Settings.Interiors.PlayerShadowThirdPerson = TheSettingManager->GetSettingF("Shaders.ShadowsInteriors.Main", "PlayerShadowThirdPerson");

	bool isExterior = TheShaderManager->GameState.isExterior;

	// if the effect was turned off the buffer must be cleared
	if (!Enabled || (isExterior && !Settings.Exteriors.Enabled) || (!isExterior && !Settings.Interiors.Enabled)) clearShadowsBuffer();

	// If certain shadow map settings were changed, recreate the textures and surfaces.
	if (texturesInitialized)
		RecreateTextures(cascadeSettingsChanged, false, false);
}


void ShadowsExteriorEffect::clearShadowsBuffer() {
	// clear shadows buffer
	TheRenderManager->device->SetRenderTarget(0, Textures.ShadowPassSurface);
	TheRenderManager->device->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 1.0f, 0L);
}


void ShadowsExteriorEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_SmoothedSunDir", &Constants.SmoothedSunDir);
	TheShaderManager->RegisterConstant("TESR_ShadowData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_ShadowFormatData", &Constants.FormatData);
	TheShaderManager->RegisterConstant("TESR_ShadowScreenSpaceData", &Constants.ScreenSpaceData);
	TheShaderManager->RegisterConstant("TESR_OrthoData", &Constants.OrthoData);
	TheShaderManager->RegisterConstant("TESR_ShadowFade", &Constants.ShadowFade);
	TheShaderManager->RegisterConstant("TESR_ShadowRadius", &Constants.ShadowMapRadius);
	TheShaderManager->RegisterConstant("TESR_ShadowViewProjTransform", (D3DXVECTOR4*)&Constants.ShadowViewProj);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransform", (D3DXVECTOR4*)&Constants.ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowNearCenter", &ShadowMaps[MapNear].ShadowMapCascadeCenterRadius);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformNear", (D3DXVECTOR4*)&ShadowMaps[MapNear].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowMiddleCenter", &ShadowMaps[MapMiddle].ShadowMapCascadeCenterRadius);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformMiddle", (D3DXVECTOR4*)&ShadowMaps[MapMiddle].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowFarCenter", &ShadowMaps[MapFar].ShadowMapCascadeCenterRadius);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformFar", (D3DXVECTOR4*)&ShadowMaps[MapFar].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowLodCenter", &ShadowMaps[MapLod].ShadowMapCascadeCenterRadius);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformLod", (D3DXVECTOR4*)&ShadowMaps[MapLod].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformOrtho", (D3DXVECTOR4*)&ShadowMaps[MapOrtho].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowCubeMapLightPosition", &Constants.ShadowCubeMapLightPosition);
	TheShaderManager->RegisterConstant("TESR_ShadowLightPosition", (D3DXVECTOR4*)&Constants.ShadowLightPosition);
}

void ShadowsExteriorEffect::RegisterTextures() {
	ULONG ShadowMapSize = Settings.ShadowMaps.CascadeResolution;
	ULONG ShadowCubeMapSize = Settings.Interiors.ShadowCubeMapSize;
	ULONG ShadowAtlasSize = ShadowMapSize * 2;

	TheTextureManager->InitTexture("TESR_ShadowAtlas", &ShadowAtlasTexture, &ShadowAtlasSurface, ShadowAtlasSize, ShadowAtlasSize, Settings.ShadowMaps.Format, Settings.ShadowMaps.Mipmaps);

	if (!Settings.ShadowMaps.MSAA)
		TheRenderManager->device->CreateDepthStencilSurface(ShadowAtlasSize, ShadowAtlasSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &ShadowAtlasDepthSurface, NULL);
	else {
		TheRenderManager->device->CreateRenderTarget(ShadowAtlasSize, ShadowAtlasSize, Settings.ShadowMaps.Format, D3DMULTISAMPLE_4_SAMPLES, 0, 0, &ShadowAtlasSurfaceMSAA, NULL);
		TheRenderManager->device->CreateDepthStencilSurface(ShadowAtlasSize, ShadowAtlasSize, D3DFMT_D24S8, D3DMULTISAMPLE_4_SAMPLES, 0, true, &ShadowAtlasDepthSurface, NULL);
	}

	for (int i = 0; i <= MapLod; i++) {
		ShadowMaps[i].ShadowMapViewPort = { i % 2 == 0 ? 0 : ShadowMapSize, i < 2 ? 0 : ShadowMapSize, ShadowMapSize, ShadowMapSize, 0.0f, 1.0f };
		ShadowMaps[i].ShadowMapInverseResolution = 1.0f / (float)ShadowMapSize;
	}

	TheShaderManager->CreateFrameVertex(ShadowAtlasSize, ShadowAtlasSize, &ShadowAtlasVertexBuffer);
	ShadowAtlasCascadeTexelSize = 1.0f / (float)ShadowAtlasSize;

	// ortho texture
	ULONG orthoMapRes = Settings.Exteriors.OrthoMapResolution;
	TheTextureManager->InitTexture("TESR_OrthoMapBuffer", &ShadowMapOrthoTexture, &ShadowMapOrthoSurface, orthoMapRes, orthoMapRes, D3DFMT_G32R32F);
	TheRenderManager->device->CreateDepthStencilSurface(orthoMapRes, orthoMapRes, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &ShadowMapOrthoDepthSurface, NULL);
	ShadowMaps[MapOrtho].ShadowMapViewPort = { 0, 0, orthoMapRes, orthoMapRes, 0.0f, 1.0f };
	ShadowMaps[MapOrtho].ShadowMapInverseResolution = 1.0f / (float)orthoMapRes;


	// initialize spot lights maps
	for (int i = 0; i < SpotLightsMax; i++) {
		std::string textureName = "TESR_ShadowSpotlightBuffer" + std::to_string(i);
		TheTextureManager->InitTexture(textureName.c_str(), &Textures.ShadowSpotlightTexture[i], &Textures.ShadowSpotlightSurface[i], ShadowCubeMapSize, ShadowCubeMapSize, D3DFMT_R32F);
	}


	// initialize point lights cubemaps
	for (int i = 0; i < ShadowCubeMapsMax; i++) {
		TheRenderManager->device->CreateCubeTexture(ShadowCubeMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &Textures.ShadowCubeMapTexture[i], NULL);
		for (int j = 0; j < 6; j++) {
			Textures.ShadowCubeMapTexture[i]->GetCubeMapSurface((D3DCUBEMAP_FACES)j, 0, &Textures.ShadowCubeMapSurface[i][j]);
		}
		std::string textureName = "TESR_ShadowCubeMapBuffer" + std::to_string(i);
		TheTextureManager->RegisterTexture(textureName.c_str(), (IDirect3DBaseTexture9**)&Textures.ShadowCubeMapTexture[i]);
	}
	// Create the stencil surface used for rendering cubemaps
	TheRenderManager->device->CreateDepthStencilSurface(ShadowCubeMapSize, ShadowCubeMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &Textures.ShadowCubeMapDepthSurface, NULL);

	//TheShadowManager->ShadowCubeMapViewPort = { 0, 0, ShadowCubeMapSize, ShadowCubeMapSize, 0.0f, 1.0f };
	//memset(TheShadowManager->ShadowCubeMapLights, NULL, sizeof(ShadowCubeMapLights));

	// Initialize shadow buffer
	TheTextureManager->InitTexture("TESR_PointShadowBuffer", &Textures.ShadowPassTexture, &Textures.ShadowPassSurface, TheRenderManager->width, TheRenderManager->height, D3DFMT_G16R16);

	texturesInitialized = true;
}


/*
 * Recreate specific shadow maps, to be used after specific settings change.
 */
void ShadowsExteriorEffect::RecreateTextures(bool cascades, bool ortho, bool cubemaps) {
	if (cascades) {
		if (ShadowAtlasSurface) {
			ShadowAtlasSurface->Release();
			ShadowAtlasSurface = nullptr;
		}
		if (ShadowAtlasSurfaceMSAA) {
			ShadowAtlasSurfaceMSAA->Release();
			ShadowAtlasSurfaceMSAA = nullptr;
		}
		if (ShadowAtlasTexture) {
			ShadowAtlasTexture->Release();
			ShadowAtlasTexture = nullptr;
		};
		if (ShadowAtlasDepthSurface) {
			ShadowAtlasDepthSurface->Release();
			ShadowAtlasDepthSurface = nullptr;
		};
		if (ShadowAtlasVertexBuffer) {
			ShadowAtlasVertexBuffer->Release();
			ShadowAtlasVertexBuffer = nullptr;
		}

		ULONG ShadowMapSize = Settings.ShadowMaps.CascadeResolution;
		ULONG ShadowAtlasSize = ShadowMapSize * 2;

		TheTextureManager->InitTexture("TESR_ShadowAtlas", &ShadowAtlasTexture, &ShadowAtlasSurface, ShadowAtlasSize, ShadowAtlasSize, Settings.ShadowMaps.Format, Settings.ShadowMaps.Mipmaps);

		if (!Settings.ShadowMaps.MSAA)
			TheRenderManager->device->CreateDepthStencilSurface(ShadowAtlasSize, ShadowAtlasSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &ShadowAtlasDepthSurface, NULL);
		else {
			TheRenderManager->device->CreateRenderTarget(ShadowAtlasSize, ShadowAtlasSize, Settings.ShadowMaps.Format, D3DMULTISAMPLE_4_SAMPLES, 0, 0, &ShadowAtlasSurfaceMSAA, NULL);
			TheRenderManager->device->CreateDepthStencilSurface(ShadowAtlasSize, ShadowAtlasSize, D3DFMT_D24S8, D3DMULTISAMPLE_4_SAMPLES, 0, true, &ShadowAtlasDepthSurface, NULL);
		}

		for (int i = 0; i <= MapLod; i++) {
			ShadowMaps[i].ShadowMapViewPort = { i % 2 == 0 ? 0 : ShadowMapSize, i < 2 ? 0 : ShadowMapSize, ShadowMapSize, ShadowMapSize, 0.0f, 1.0f };
			ShadowMaps[i].ShadowMapInverseResolution = 1.0f / (float)ShadowMapSize;
		}

		TheShaderManager->CreateFrameVertex(ShadowAtlasSize, ShadowAtlasSize, &ShadowAtlasVertexBuffer);
		ShadowAtlasCascadeTexelSize = 1.0f / (float)ShadowAtlasSize;

		SunShadowsEffect* sunShadows = TheShaderManager->Effects.SunShadows;
		ShaderTextureValue* Sampler;
		for (UInt32 c = 0; c < sunShadows->TextureShaderValuesCount; c++) {
			Sampler = &sunShadows->TextureShaderValues[c];
			if (!memcmp(Sampler->Name, "TESR_ShadowAtlas", 15) && Sampler->Texture->Texture) {
				Sampler->Texture->Texture = nullptr;
			}
		}

		// Reset shadow manager frame counter.
		TheShadowManager->FrameCounter = 0;
	}
}


/*
* Produce a smooth sun direction.
*/
D3DXVECTOR3 ShadowsExteriorEffect::CalculateSmoothedSunDir() {
	const float yawStepSize = D3DXToRadian(1.0f);		// 1.0° step for yaw (horizontal rotation)
	const float pitchStepSize = D3DXToRadian(0.25f);	// 0.25° step for pitch (vertical rotation)
	const float smoothingFactor = 0.1f;					// Smoothing strength (0 = no smoothing, 1 = instant)
	const float maxJumpAngle = D3DXToRadian(5.0f);		// If sun moves more than 5°, apply instantly

	D3DXVECTOR3 SunDir(TheShaderManager->ShaderConst.SunDir);

	float theta = atan2f(SunDir.y, SunDir.x);   // Yaw
	float phi = acosf(SunDir.z);				// Pitch

	theta = roundf(theta / yawStepSize) * yawStepSize;
	phi = roundf(phi / pitchStepSize) * pitchStepSize;

	D3DXVECTOR3 QuantizedSunDir;
	QuantizedSunDir.x = sinf(phi) * cosf(theta);
	QuantizedSunDir.y = sinf(phi) * sinf(theta);
	QuantizedSunDir.z = cosf(phi);
	D3DXVec3Normalize(&QuantizedSunDir, &QuantizedSunDir);

	D3DXVECTOR3 SmoothedSunDir(Constants.SmoothedSunDir);

	// Compute angle difference between smoothed and new direction
	float dotProduct = D3DXVec3Dot(&QuantizedSunDir, &SmoothedSunDir);
	dotProduct = max(-1.0f, min(1.0f, dotProduct)); // Clamp to avoid NaN
	float angleDifference = acosf(dotProduct); // Angle between old and new direction

	// Apply smoothing only if the change is small
	if (angleDifference < maxJumpAngle) {
		D3DXVec3Lerp(&SmoothedSunDir, &SmoothedSunDir, &QuantizedSunDir, smoothingFactor);
	}
	else {
		SmoothedSunDir = QuantizedSunDir;
	}

	Constants.SmoothedSunDir = D3DXVECTOR4(SmoothedSunDir, 0.0f);
	return SmoothedSunDir;
}


void ShadowsExteriorEffect::GetCascadeDepths() {
	NiCamera* sceneCamera = WorldSceneGraph->camera;

	float nearClip = sceneCamera->Frustum.Near;
	float farClip = sceneCamera->Frustum.Far;
	float clipRange = farClip - nearClip;

	float minZ = nearClip + 10.0f;
	float maxZ = nearClip + Settings.ShadowMaps.Distance * clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	int cascadeCount = 4;

	for (int i = 0; i < cascadeCount; ++i) {
		float p = (i + 1) / static_cast<float>(cascadeCount);
		float log = minZ * std::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = Settings.ShadowMaps.CascadeLambda * (log - uniform) + uniform;
		ShadowMaps[i].ShadowMapRadius = (d - nearClip) / clipRange;
	}

	// Get Near distance for each cascade
	ShadowMaps[MapNear].ShadowMapNear = 10.0f / clipRange;
	ShadowMaps[MapMiddle].ShadowMapNear = ShadowMaps[MapNear].ShadowMapRadius;
	ShadowMaps[MapFar].ShadowMapNear = ShadowMaps[MapMiddle].ShadowMapRadius;
	ShadowMaps[MapLod].ShadowMapNear = ShadowMaps[MapFar].ShadowMapRadius;

	// Store absolute Shadow map splits in Constants to pass to the Shaders
	Constants.ShadowMapRadius.x = ShadowMaps[MapNear].ShadowMapRadius * clipRange;
	Constants.ShadowMapRadius.y = ShadowMaps[MapMiddle].ShadowMapRadius * clipRange;
	Constants.ShadowMapRadius.z = ShadowMaps[MapFar].ShadowMapRadius * clipRange;
	Constants.ShadowMapRadius.w = ShadowMaps[MapLod].ShadowMapRadius * clipRange;

	// ortho distance render
	ShadowMaps[MapOrtho].ShadowMapRadius = Settings.Exteriors.OrthoRadius;
}


D3DXMATRIX ShadowsExteriorEffect::GetOrthoViewProj(D3DXMATRIX View) {
	float FarPlane = 32768.0f;  // Hardcoded for ortho maps.
	float Radius = ShadowMaps[MapOrtho].ShadowMapRadius;

	// shift the covered area in the direction of the camera vector
	D3DXVECTOR4 Center = D3DXVECTOR4(TheRenderManager->CameraForward.x, TheRenderManager->CameraForward.y, 0.0, 1.0);
	D3DXVec4Normalize(&Center, &Center);
	Radius *= 2;
	Center.x *= Radius;
	Center.y *= Radius;
	D3DXVec4Transform(&Center, &Center, &View);

	D3DXMATRIX Proj;
	D3DXMatrixOrthoOffCenterRH(&Proj, Center.x - Radius, Center.x + Radius, Center.y - Radius, Center.y + Radius, FarPlane * 0.8f, 1.2f * FarPlane);
	return View * Proj;
}


// Banker round helper.
float BankerRound(float num) {
	float rounded = round(num);
	if (fabs(rounded - num) == 0.5f) {
		return 2.0 * round(0.5 * num);
	}
	return rounded;
}


// Round a vector using the banker round method.
void Vector3Round(D3DXVECTOR3* out, D3DXVECTOR3* in) {
	out->x = BankerRound(in->x);
	out->y = BankerRound(in->y);
	out->z = BankerRound(in->z);
}

void Vector4Round(D3DXVECTOR4* out, D3DXVECTOR4* in) {
	out->x = BankerRound(in->x);
	out->y = BankerRound(in->y);
	out->z = BankerRound(in->z);
	out->w = BankerRound(in->w);
}


// Generate the ViewProj matrix for a particular shadow cascade.
// Inspired by MJP's https://mynameismjp.wordpress.com/2013/09/10/shadow-maps/ article and code example.
D3DXMATRIX ShadowsExteriorEffect::GetCascadeViewProj(ShadowMapSettings* ShadowMap, D3DXVECTOR3* SunDir) {
	// Get z-range for this cascade.
	NiCamera* sceneCamera = WorldSceneGraph->camera;
	NiPoint3 cameraPosition = sceneCamera->m_worldTransform.pos;
	float zNear = ShadowMap->ShadowMapNear;
	float zFar = ShadowMap->ShadowMapRadius;

	// Calculate the frustum corners in world space (from a unit cube in projective space).
	D3DXMATRIX invViewProj = TheRenderManager->InvViewProjMatrix;

	float ndcNear = 1.0f ? TheRenderManager->IsReversedDepth() : 0.0f;
	float ndcFar = 1.0f - ndcNear;
	D3DXVECTOR3 frustumCorners[8] = {
		D3DXVECTOR3(-1.0f,  1.0f, ndcNear), // Near plane.
		D3DXVECTOR3( 1.0f,  1.0f, ndcNear),
		D3DXVECTOR3( 1.0f, -1.0f, ndcNear),
		D3DXVECTOR3(-1.0f, -1.0f, ndcNear),
		D3DXVECTOR3(-1.0f,  1.0f, ndcFar),  // Far plane.
		D3DXVECTOR3( 1.0f,  1.0f, ndcFar),
		D3DXVECTOR3( 1.0f, -1.0f, ndcFar),
		D3DXVECTOR3(-1.0f, -1.0f, ndcFar),
	};
	for (auto i = 0; i < 8; ++i) {
		D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &invViewProj);
	}

	// Get the corners of the current cascade slice of the view frustum.
	for (auto i = 0; i < 4; ++i)
	{
		D3DXVECTOR3 cornerRay = frustumCorners[i + 4] - frustumCorners[i];
		D3DXVECTOR3 nearCornerRay = cornerRay * zNear;
		D3DXVECTOR3 farCornerRay = cornerRay * zFar;
		frustumCorners[i + 4] = frustumCorners[i] + farCornerRay;
		frustumCorners[i] = frustumCorners[i] + nearCornerRay;
	}

	// Calculate the centroid of the view frustum slice.
	D3DXVECTOR3 frustumCenter(0.0f, 0.0f, 0.0f);
	for (auto i = 0; i < 8; ++i)
		frustumCenter = frustumCenter + frustumCorners[i];
	frustumCenter *= 1.0f / 8.0f;
	
	// Must be kept stable.
	D3DXVECTOR3 upDir(0.0f, 0.0f, 1.0f);

	D3DXVECTOR3 minExtents, maxExtents;

	// Calculate the radius of a bounding sphere surrounding the frustum corners
	float sphereRadius = 0.0f;
	for (auto i = 0; i < 8; ++i)
	{
		D3DXVECTOR3 centerToCorner = frustumCorners[i] - frustumCenter;
		float dist = D3DXVec3Length(&centerToCorner);
		sphereRadius = max(sphereRadius, dist);
	}
	sphereRadius = std::ceil(sphereRadius * 16.0f) / 16.0f;
	maxExtents = D3DXVECTOR3(sphereRadius, sphereRadius, sphereRadius);
	minExtents = -maxExtents;
	
	D3DXVECTOR3 cascadeExtents = maxExtents - minExtents;

	// Create a shadow frustum center by moving the view frustum slice center away from the camera.
	// Should make it so we can more easily use the full resolution, which is mostly wasted due to
	// stabilization.
	D3DXVECTOR3 shadowFrustumCenter = frustumCenter;
	//D3DXVec3Normalize(&shadowFrustumCenter, &frustumCenter);  // Get the direction from camera to the frustum center.
	//shadowFrustumCenter *= sphereRadius;  // Move the center so that the length is equal to the sphere radius.
	
	ShadowMap->ShadowMapCascadeCenterRadius.x = shadowFrustumCenter.x;
	ShadowMap->ShadowMapCascadeCenterRadius.y = shadowFrustumCenter.y;
	ShadowMap->ShadowMapCascadeCenterRadius.z = shadowFrustumCenter.z;
	ShadowMap->ShadowMapCascadeCenterRadius.w = sphereRadius;

	// Calculate correct bound size limit for current cascade.
	ShadowMap->Forms.MinRadius = ShadowMap->Forms.OrigMinRadius * sphereRadius * ShadowMap->ShadowMapInverseResolution;

	float nearPlane = 0.0f;  // Shadow casters are pancaked to near plane in the vertex shader.
	float farPlane = cascadeExtents.z;
	D3DXVECTOR3 shadowCameraPos = shadowFrustumCenter + D3DXVECTOR3(*SunDir) * -minExtents.z;
	
	D3DXMATRIX shadowView, shadowProj, shadowViewProj;

	D3DXMatrixLookAtRH(&shadowView, &shadowCameraPos, &shadowFrustumCenter, &upDir);
	D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, nearPlane, farPlane);
	shadowViewProj = shadowView * shadowProj;

	// Create the rounding matrix, by projecting the world-space origin and determining
	// the fractional offset in texel space.
	float sMapSize = Settings.ShadowMaps.CascadeResolution;
	// We are working in camera relative world space - camera position is our fixed point for stabilization.
	D3DXVECTOR4 shadowOrigin(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z, 1.0f);
	D3DXVec4Transform(&shadowOrigin, &shadowOrigin, &shadowViewProj);
	D3DXVec4Scale(&shadowOrigin, &shadowOrigin, sMapSize / 2.0f);
	D3DXVECTOR4 roundedOrigin, roundOffset;
	Vector4Round(&roundedOrigin, &shadowOrigin);
	D3DXVec4Subtract(&roundOffset, &roundedOrigin, &shadowOrigin);
	D3DXVec4Scale(&roundOffset, &roundOffset, 2.0f / sMapSize);

	shadowProj._41 = shadowProj._41 + roundOffset.x;
	shadowProj._42 = shadowProj._42 + roundOffset.y;

	shadowViewProj = shadowView * shadowProj;

	NiFrustum frustum(minExtents.x, maxExtents.x, maxExtents.y, minExtents.y, nearPlane, farPlane, true);
	TheCameraManager->SetFrustumPlanes(&ShadowMap->ShadowMapFrustumPlanes, &shadowViewProj, shadowCameraPos, frustum);
	ShadowMap->ShadowMapFrustumPlanes.SetActivePlaneState(62);

	return shadowViewProj;
}

