#include "ShadowsExterior.h"

void ShadowsExteriorEffect::UpdateConstants() {

	Constants.ShadowFade.x = 0; // Fade 1.0 == no shadows
	if (TheShaderManager->GameState.isExterior) {
		Constants.ShadowFade.x = smoothStep(0.3, 0, abs(TheShaderManager->GameState.dayLight - 0.5)); // fade shadows to 0 at sunrise/sunset.  

		TimeGlobals* GameTimeGlobals = TimeGlobals::Get();
		float DaysPassed = GameTimeGlobals->GameDaysPassed ? GameTimeGlobals->GameDaysPassed->data : 1.0f;

		if(TheShaderManager->GameState.isDayTime < 0.5) {
			// at night time, fade based on moonphase
			// moonphase goes from 0 to 8
			float MoonPhase = (fmod(DaysPassed, 8 * Tes->sky->firstClimate->phaseLength & 0x3F)) / (Tes->sky->firstClimate->phaseLength & 0x3F);

			float PI = 3.1416; // use cos curve to fade moon light shadows strength
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

void ShadowsExteriorEffect::UpdateSettings() {

	Constants.ScreenSpaceData.x = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ScreenSpace", "Enabled") && Enabled;
	Constants.ScreenSpaceData.y = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "BlurRadius");
	Constants.ScreenSpaceData.z = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "RenderDistance");
	Constants.ScreenSpaceData.w = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "Darkness");

	// Generic exterior shadows settings
	Settings.Exteriors.Enabled = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "Enabled");
	Settings.Exteriors.Quality = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "Quality");
	Settings.Exteriors.Darkness = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "Darkness");
	Settings.Exteriors.NightMinDarkness = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "NightMinDarkness");
	Settings.Exteriors.ShadowRadius = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "ShadowsRadius");
	Settings.Exteriors.ShadowMapResolution = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "ShadowMapResolution");
	Settings.Exteriors.ShadowMapFarPlane = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "ShadowMapFarPlane");
	Settings.Exteriors.ShadowMode = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "ShadowMode");
	Settings.Exteriors.BlurShadowMaps = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "BlurShadowMaps");
	Settings.Exteriors.UsePointShadowsDay = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "UsePointShadowsDay");
	Settings.Exteriors.UsePointShadowsNight = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "UsePointShadowsNight");

	//Shadows Cascade settings
	GetCascadeDepths();

	for (int shadowType = 0; shadowType <= MapOrtho; shadowType++) {
		char sectionName[256] = "Shaders.ShadowsExteriors.";
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
		ShadowMap->Forms.MinRadius = TheSettingManager->GetSettingI(sectionName, "MinRadius");
	};

	// get the list of excluded formIDs
	TheSettingManager->GetFormList("Shaders.ShadowsExteriors.ExcludedFormID", &Settings.Exteriors.ExcludedForms);

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

	TheSettingManager->GetFormList("Shaders.ShadowsInteriors.ExcludedFormID", &Settings.Interiors.ExcludedForms);

	bool isExterior = TheShaderManager->GameState.isExterior;

	// if the effect was turned off the buffer must be cleared
	if (!Enabled || (isExterior && !Settings.Exteriors.Enabled) || (!isExterior && !Settings.Interiors.Enabled)) clearShadowsBuffer();
}


void ShadowsExteriorEffect::clearShadowsBuffer() {
	// clear shadows buffer
	TheRenderManager->device->SetRenderTarget(0, Textures.ShadowPassSurface);
	TheRenderManager->device->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 0, 0), 1.0f, 0L);
}


void ShadowsExteriorEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_ShadowData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_ShadowScreenSpaceData", &Constants.ScreenSpaceData);
	TheShaderManager->RegisterConstant("TESR_OrthoData", &Constants.OrthoData);
	TheShaderManager->RegisterConstant("TESR_ShadowFade", &Constants.ShadowFade);
	TheShaderManager->RegisterConstant("TESR_ShadowRadius", &Constants.ShadowMapRadius);
	TheShaderManager->RegisterConstant("TESR_ShadowViewProjTransform", (D3DXVECTOR4*)&Constants.ShadowViewProj);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransform", (D3DXVECTOR4*)&Constants.ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformNear", (D3DXVECTOR4*)&ShadowMaps[MapNear].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformMiddle", (D3DXVECTOR4*)&ShadowMaps[MapMiddle].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformFar", (D3DXVECTOR4*)&ShadowMaps[MapFar].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformLod", (D3DXVECTOR4*)&ShadowMaps[MapLod].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowCameraToLightTransformOrtho", (D3DXVECTOR4*)&ShadowMaps[MapOrtho].ShadowCameraToLight);
	TheShaderManager->RegisterConstant("TESR_ShadowCubeMapLightPosition", &Constants.ShadowCubeMapLightPosition);
	TheShaderManager->RegisterConstant("TESR_ShadowLightPosition", (D3DXVECTOR4*)&Constants.ShadowLightPosition);
}

void ShadowsExteriorEffect::RegisterTextures() {
	int ShadowMapSize;
	ULONG ShadowCubeMapSize = Settings.Interiors.ShadowCubeMapSize;

	// initialize cascade shadowmaps
	std::vector<const char*>ShadowBufferNames = {
		"TESR_ShadowMapBufferNear",
		"TESR_ShadowMapBufferMiddle",
		"TESR_ShadowMapBufferFar",
		"TESR_ShadowMapBufferLod",
		"TESR_OrthoMapBuffer",
	};
	for (int i = 0; i <= ShadowManager::ShadowMapTypeEnum::MapOrtho; i++) {
		// create one texture per Exterior ShadowMap type
		float multiple = (i == ShadowManager::ShadowMapTypeEnum::MapLod && Settings.Exteriors.ShadowMapResolution <= 2048) ? 2.0f : 1.0f; // double the size of lod map only
		ShadowMapSize = Settings.Exteriors.ShadowMapResolution * multiple;

		TheTextureManager->InitTexture(ShadowBufferNames[i], &ShadowMaps[i].ShadowMapTexture, &ShadowMaps[i].ShadowMapSurface, ShadowMapSize, ShadowMapSize, D3DFMT_G32R32F);
		TheRenderManager->device->CreateDepthStencilSurface(ShadowMapSize, ShadowMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &ShadowMaps[i].ShadowMapDepthSurface, NULL);
	}

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

	// initialize the frame vertices for future shadow blurring
	for (int i = 0; i <= MapOrtho; i++) {
		float multiple = (i == ShadowManager::ShadowMapTypeEnum::MapLod && Settings.Exteriors.ShadowMapResolution <= 2048) ? 2.0f : 1.0f; // double the size of lod map only
		ULONG ShadowMapSize = Settings.Exteriors.ShadowMapResolution * multiple;

		if (i != MapOrtho) TheShaderManager->CreateFrameVertex(ShadowMapSize, ShadowMapSize, &ShadowMaps[i].BlurShadowVertexBuffer);
		ShadowMaps[i].ShadowMapViewPort = { 0, 0, ShadowMapSize, ShadowMapSize, 0.0f, 1.0f };
		ShadowMaps[i].ShadowMapInverseResolution = 1.0f / (float)ShadowMapSize;
	}
	//TheShadowManager->ShadowCubeMapViewPort = { 0, 0, ShadowCubeMapSize, ShadowCubeMapSize, 0.0f, 1.0f };
	//memset(TheShadowManager->ShadowCubeMapLights, NULL, sizeof(ShadowCubeMapLights));

	// Initialize shadow buffer
	TheTextureManager->InitTexture("TESR_PointShadowBuffer", &Textures.ShadowPassTexture, &Textures.ShadowPassSurface, TheRenderManager->width / 2, TheRenderManager->height / 2, D3DFMT_A8R8G8B8);
}


void ShadowsExteriorEffect::GetCascadeDepths() {
	float camFar = Settings.Exteriors.ShadowRadius;
	float logFactor = 0.9f;
	float camNear = 10.0f;
	int cascadeCount = 4;
	float cascadeNum = 0.0f;

	for (int i = 0; i < cascadeCount; i++) {
		// formula for cascade ratios adapted from https://www.asawicki.info/news_1283_cascaded_shadow_mapping
		cascadeNum += 1.0f;

		ShadowMaps[i].ShadowMapRadius = std::lerp(
			camNear + (cascadeNum / cascadeCount) * (camFar - camNear),
			camNear * powf(camFar / camNear, cascadeNum / cascadeCount),
			logFactor);

		// filtering objects occupying less than 10 pixels in the shadow map
		ShadowMaps[i].Forms.MinRadius = 10.0f * ShadowMaps[i].ShadowMapRadius / Settings.Exteriors.ShadowMapResolution;
	}

	// Get Near distance for each cascade
	ShadowMaps[MapNear].ShadowMapNear = camNear;
	ShadowMaps[MapMiddle].ShadowMapNear = ShadowMaps[MapNear].ShadowMapRadius * 0.9;
	ShadowMaps[MapFar].ShadowMapNear = ShadowMaps[MapMiddle].ShadowMapRadius * 0.9;
	ShadowMaps[MapLod].ShadowMapNear = ShadowMaps[MapFar].ShadowMapRadius * 0.9;

	// Store Shadow map sizes in Constants to pass to the Shaders
	Constants.ShadowMapRadius.x = ShadowMaps[MapNear].ShadowMapRadius;
	Constants.ShadowMapRadius.y = ShadowMaps[MapMiddle].ShadowMapRadius;
	Constants.ShadowMapRadius.z = ShadowMaps[MapFar].ShadowMapRadius;
	Constants.ShadowMapRadius.w = ShadowMaps[MapLod].ShadowMapRadius;

	// ortho distance render
	ShadowMaps[MapOrtho].ShadowMapRadius = ShadowMaps[MapFar].ShadowMapRadius;
}


D3DXMATRIX ShadowsExteriorEffect::GetOrthoViewProj(D3DXMATRIX View) {
	float FarPlane = Settings.Exteriors.ShadowMapFarPlane;
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


// calculates the minimum area viewproj matrix for a given cascade using cascade depth and frustum corners
D3DXMATRIX ShadowsExteriorEffect::GetCascadeViewProj(ShadowMapSettings* ShadowMap, D3DXMATRIX View) {
	D3DXMATRIX Proj;
	float FarPlane = Settings.Exteriors.ShadowMapFarPlane;
	float Radius = ShadowMap->ShadowMapRadius;
	NiCamera* Camera = WorldSceneGraph->camera;

	// calculating the size of the shadow cascade
	float znear = ShadowMap->ShadowMapNear;
	float zfar = ShadowMap->ShadowMapRadius;

	float w = Camera->Frustum.Right - Camera->Frustum.Left;
	float h = Camera->Frustum.Top - Camera->Frustum.Bottom;

	float ar = h / w;

	//Logger::Log("fov %f   %f   %f", WorldSceneGraph->cameraFOV, Player->GetFoV(false), Player->GetFoV(true));
	float fov = TheRenderManager->FOVData.z;
	float fovY = TheRenderManager->FOVData.w;

	float tanHalfHFOV = tanf(fov * 0.5f);
	float tanHalfVFOV = tanf(fovY * 0.5f);

	float xn = znear * tanHalfHFOV;
	float xf = zfar * tanHalfHFOV;
	float yn = znear * tanHalfVFOV;
	float yf = zfar * tanHalfVFOV;

	D3DXVECTOR4 frustrumPoints[8];

	// near face
	frustrumPoints[0] = D3DXVECTOR4(xn, yn, znear, 1.0);
	frustrumPoints[1] = D3DXVECTOR4(-xn, yn, znear, 1.0);
	frustrumPoints[2] = D3DXVECTOR4(xn, -yn, znear, 1.0);
	frustrumPoints[3] = D3DXVECTOR4(-xn, -yn, znear, 1.0);

	// far face
	frustrumPoints[4] = D3DXVECTOR4(xf, yf, zfar, 1.0);
	frustrumPoints[5] = D3DXVECTOR4(-xf, yf, zfar, 1.0);
	frustrumPoints[6] = D3DXVECTOR4(xf, -yf, zfar, 1.0);
	frustrumPoints[7] = D3DXVECTOR4(-xf, -yf, zfar, 1.0);

	// values of the final light frustrum
	float left = 0.0f;
	float right = 0.0f;
	float bottom = 0.0f;
	float top = 0.0f;

	// transform from camera to world then to light space
	D3DXMATRIX m = TheRenderManager->InvViewMatrix * View;
	D3DXVECTOR4 p;
	for (int i = 0; i < 8; i++) {
		D3DXVec4Transform(&p, &frustrumPoints[i], &m);

		// extend frustrum to include all corners
		if (p.x < left || left == 0.0f) left = p.x;
		if (p.x > right || right == 0.0f) right = p.x;
		if (p.y > top || top == 0.0f) top = p.y;
		if (p.y < bottom || bottom == 0.0f) bottom = p.y;
	}


	D3DXMatrixOrthoOffCenterRH(&Proj, left, right, bottom, top, FarPlane * 0.6f, 1.4f * FarPlane);
	return View * Proj;
}
