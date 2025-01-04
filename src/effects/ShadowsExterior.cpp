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
	Settings.Exteriors.OrthoRadius = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "OrthoRadius");
	Settings.Exteriors.ShadowMapResolution = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "ShadowMapResolution");
	Settings.Exteriors.OrthoMapResolution = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", "OrthoMapResolution");
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
	ULONG ShadowCubeMapSize = Settings.Interiors.ShadowCubeMapSize;

	// initialize cascade shadowmaps
	std::vector<const char*>ShadowBufferNames = {
		"TESR_ShadowMapBufferNear",
		"TESR_ShadowMapBufferMiddle",
		"TESR_ShadowMapBufferFar",
		"TESR_ShadowMapBufferLod",
	};
	for (int i = 0; i <= MapLod; i++) {
		// create one texture per Exterior ShadowMap type
		float multiple = 1.0f; //(i == MapLod && Settings.Exteriors.ShadowMapResolution <= 2048) ? 2.0f : 1.0f; // double the size of lod map only
		ULONG ShadowMapSize = Settings.Exteriors.ShadowMapResolution * multiple;

		TheTextureManager->InitTexture(ShadowBufferNames[i], &ShadowMaps[i].ShadowMapTexture, &ShadowMaps[i].ShadowMapSurface, ShadowMapSize, ShadowMapSize, D3DFMT_G32R32F);
		TheRenderManager->device->CreateDepthStencilSurface(ShadowMapSize, ShadowMapSize, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &ShadowMaps[i].ShadowMapDepthSurface, NULL);
		
		// initialize the frame vertices for future shadow blurring
		TheShaderManager->CreateFrameVertex(ShadowMapSize, ShadowMapSize, &ShadowMaps[i].BlurShadowVertexBuffer);
		ShadowMaps[i].ShadowMapViewPort = { 0, 0, ShadowMapSize, ShadowMapSize, 0.0f, 1.0f };
		ShadowMaps[i].ShadowMapInverseResolution = 1.0f / (float)ShadowMapSize;
	}

	// ortho texture
	ULONG orthoMapRes = Settings.Exteriors.OrthoMapResolution;
	TheTextureManager->InitTexture("TESR_OrthoMapBuffer", &ShadowMaps[MapOrtho].ShadowMapTexture, &ShadowMaps[MapOrtho].ShadowMapSurface, orthoMapRes, orthoMapRes, D3DFMT_G32R32F);
	TheRenderManager->device->CreateDepthStencilSurface(orthoMapRes, orthoMapRes, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, true, &ShadowMaps[MapOrtho].ShadowMapDepthSurface, NULL);
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

	// Initialize shadow buffer vertex buffer
	TheShaderManager->CreateFrameVertex(TheRenderManager->width, TheRenderManager->height, &Textures.ShadowPassVertexBuffer);
}

/*
 * Calculate Z partitioning for shadow cascades.
 * Based on results on perspective aliasing.
 * 
 * Adapted from the practical split scheme for PSSM.
 * https://mynameismjp.wordpress.com/2013/09/10/shadow-maps/
 * https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus
 */
void ShadowsExteriorEffect::GetCascadeDepths() {
	// Ranges.
	float minZ = 10.0f;
	float maxZ = Settings.Exteriors.ShadowRadius;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	// Log versus uniform factor.
	float lambda = 0.9f;

	int cascadeCount = 4;
	float cascadeNum = 0.0f;

	for (int i = 0; i < cascadeCount; i++) {
		cascadeNum += 1.0f;

		float p = cascadeNum / cascadeCount;
		float log = minZ * powf(ratio, p);
		float uniform = minZ + range * p;
		float d = lambda * (log - uniform) + uniform;

		ShadowMaps[i].ShadowMapRadius = d;

		// For filtering objects occupying less than 10 texels in the shadow map.
		ShadowMaps[i].Forms.MinRadius = 10.0f * ShadowMaps[i].ShadowMapRadius / Settings.Exteriors.ShadowMapResolution;
	}

	// Get Near distance for each cascade
	ShadowMaps[MapNear].ShadowMapNear = minZ;
	ShadowMaps[MapMiddle].ShadowMapNear = ShadowMaps[MapNear].ShadowMapRadius;
	ShadowMaps[MapFar].ShadowMapNear = ShadowMaps[MapMiddle].ShadowMapRadius;
	ShadowMaps[MapLod].ShadowMapNear = ShadowMaps[MapFar].ShadowMapRadius;

	// Store Shadow map sizes in Constants to pass to the Shaders
	Constants.ShadowMapRadius.x = ShadowMaps[MapNear].ShadowMapRadius;
	Constants.ShadowMapRadius.y = ShadowMaps[MapMiddle].ShadowMapRadius;
	Constants.ShadowMapRadius.z = ShadowMaps[MapFar].ShadowMapRadius;
	Constants.ShadowMapRadius.w = ShadowMaps[MapLod].ShadowMapRadius;

	// ortho distance render
	ShadowMaps[MapOrtho].ShadowMapRadius = Settings.Exteriors.OrthoRadius;
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

void GetNiGeometryBounds(D3DXVECTOR3& min, D3DXVECTOR3& max, NiNode* node) {
	min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);
	
	if (!node || !node->IsGeometry()) return;

	NiBound* bound = node->GetWorldBound();

	if (!bound) return;

	D3DXVECTOR3 radius(bound->Radius, bound->Radius, bound->Radius);

	min = bound->Center.toD3DXVEC3() - radius;
	max = bound->Center.toD3DXVEC3() + radius;
}

void GetNiNodeBounds(D3DXVECTOR3& min, D3DXVECTOR3& max, NiNode* node) {
	min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

	D3DXVECTOR3 minTemp, maxTemp;

	if (!node) return;

	std::stack<NiAVObject*> containers;
	NiAVObject* child;
	NiAVObject* object;
	NiNode* Node, *Geo;

	if (!node->IsGeometry())
		containers.push(node);
	else {
		GetNiGeometryBounds(minTemp, maxTemp, node);
		D3DXVec3Minimize(&min, &min, &minTemp);
		D3DXVec3Maximize(&max, &max, &maxTemp);
	}

	// Gather geometry
	while (!containers.empty()) {
		object = containers.top();
		containers.pop();

		if (!object) continue;

		Node = object->IsNiNode();
		if (!Node || Node->m_flags & NiAVObject::NiFlags::APP_CULLED) continue; // culling containers

		if (Node->IsKindOf<NiSwitchNode>()) {
			// NiSwitchNode - only render active children (if exists) to the shadow map.
			NiSwitchNode* SwitchNode = static_cast<NiSwitchNode*>(Node);
			if (SwitchNode->m_iIndex < 0)
				continue;

			child = Node->m_children.data[SwitchNode->m_iIndex];
			if (!child->IsGeometry())
				containers.push(child);
			else {
				Geo = static_cast<NiNode*>(child);
				GetNiGeometryBounds(minTemp, maxTemp, Geo);
				D3DXVec3Minimize(&min, &min, &minTemp);
				D3DXVec3Maximize(&max, &max, &maxTemp);
			}
			continue;
		}

		for (int i = 0; i < Node->m_children.end; i++) {
			child = Node->m_children.data[i];
			if (!child || child->m_flags & NiAVObject::NiFlags::APP_CULLED) continue; // culling children

			if (child->IsFadeNode() && static_cast<BSFadeNode*>(child)->FadeAlpha < 0.75f) continue; // stop rendering fadenodes below a certain opacity
			if (!child->IsGeometry())
				containers.push(child);
			else {
				Geo = static_cast<NiNode*>(child);
				GetNiGeometryBounds(minTemp, maxTemp, Geo);
				D3DXVec3Minimize(&min, &min, &minTemp);
				D3DXVec3Maximize(&max, &max, &maxTemp);
			}
		}
	}
}

void GetCellBounds(D3DXVECTOR3& min, D3DXVECTOR3& max, TESObjectCELL* cell) {
	min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

	D3DXVECTOR3 minTemp, maxTemp;

	if (!cell || cell->IsInterior())
		return;

	GetNiNodeBounds(minTemp, maxTemp, cell->GetChildNode(TESObjectCELL::kCellNode_Land));
	D3DXVec3Minimize(&min, &min, &minTemp);
	D3DXVec3Maximize(&max, &max, &maxTemp);

	// if (ShadowsExteriors->Forms[ShadowMapType].Lod) RenderLod(Tes->landLOD, ShadowMapType); //Render terrain LOD

	TList<TESObjectREFR>::Entry* Entry = &cell->objectList.First;
	while (Entry) {
		NiNode* node = Entry->item->GetNode();

		if (node) {
			GetNiNodeBounds(minTemp, maxTemp, node);
			D3DXVec3Minimize(&min, &min, &minTemp);
			D3DXVec3Maximize(&max, &max, &maxTemp);
		}

		Entry = Entry->next;
	}
}

/*
 * Get scene AABB by iterating over shadow rendered geometry.
 * Testing.
 */
void ShadowsExteriorEffect::GetSceneBounds(D3DXVECTOR3& min, D3DXVECTOR3& max) {
	min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

	D3DXVECTOR3 minTemp, maxTemp;

	min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	max = D3DXVECTOR3(FLT_MIN, FLT_MIN, FLT_MIN);

	if (Player->GetWorldSpace()) {
		GridCellArray* CellArray = Tes->gridCellArray;
		UInt32 CellArraySize = CellArray->size * CellArray->size;

		for (UInt32 i = 0; i < CellArraySize; i++) {
			GetCellBounds(minTemp, maxTemp, CellArray->GetCell(i));
			D3DXVec3Minimize(&min, &min, &minTemp);
			D3DXVec3Maximize(&max, &max, &maxTemp);
		}
	}
	else {
		GetCellBounds(minTemp, maxTemp, Player->parentCell);
		D3DXVec3Minimize(&min, &min, &minTemp);
		D3DXVec3Maximize(&max, &max, &maxTemp);
	}

	Logger::Log("Scene min bounds: %f %f %f", min.x, min.y, min.z);
	Logger::Log("Scene max bounds: %f %f %f", max.x, max.y, max.z);
}


/*
 * Compute the Z near and far values for the shadow cascade based on the AABB points in light space and the light frustum.
 * https://learn.microsoft.com/en-us/windows/win32/dxtecharts/common-techniques-to-improve-shadow-depth-maps
 */
void ShadowsExteriorEffect::ComputeNearAndFar(float& nearPlane, float& farPlane, D3DXVECTOR3 minExtents, D3DXVECTOR3 maxExtents, D3DXVECTOR3* frustumCorners) {
	// Initialize the near and far planes
	nearPlane = FLT_MAX;
	farPlane = -FLT_MAX;

	struct Triangle {
		D3DXVECTOR3 pt[3];
		bool culled;
	};

	Triangle triangleList[16];
	INT iTriangleCnt = 1;

	triangleList[0].pt[0] = frustumCorners[0];
	triangleList[0].pt[1] = frustumCorners[1];
	triangleList[0].pt[2] = frustumCorners[2];
	triangleList[0].culled = false;

	// These are the indices used to tesselate an AABB into a list of triangles.
	static const INT iAABBTriIndexes[] =
	{
		0,1,2,  1,2,3,
		4,5,6,  5,6,7,
		0,2,4,  2,4,6,
		1,3,5,  3,5,7,
		0,1,4,  1,4,5,
		2,3,6,  3,6,7
	};

	INT iPointPassesCollision[3];

	// At a high level: 
	// 1. Iterate over all 12 triangles of the AABB.  
	// 2. Clip the triangles against each plane. Create new triangles as needed.
	// 3. Find the min and max z values as the near and far plane.

	//This is easier because the triangles are in camera spacing making the collisions tests simple comparisions.

	float fLightCameraOrthographicMinX = minExtents.x;
	float fLightCameraOrthographicMaxX = maxExtents.x;
	float fLightCameraOrthographicMinY = minExtents.y;
	float fLightCameraOrthographicMaxY = maxExtents.y;

	for (int AABBTriIter = 0; AABBTriIter < 12; ++AABBTriIter)
	{

		triangleList[0].pt[0] = frustumCorners[iAABBTriIndexes[AABBTriIter * 3 + 0]];
		triangleList[0].pt[1] = frustumCorners[iAABBTriIndexes[AABBTriIter * 3 + 1]];
		triangleList[0].pt[2] = frustumCorners[iAABBTriIndexes[AABBTriIter * 3 + 2]];
		iTriangleCnt = 1;
		triangleList[0].culled = false;

		// Clip each invidual triangle against the 4 frustums.  When ever a triangle is clipped into new triangles, 
		//add them to the list.
		for (int frustumPlaneIter = 0; frustumPlaneIter < 4; ++frustumPlaneIter)
		{

			float fEdge;
			int iComponent;

			if (frustumPlaneIter == 0)
			{
				fEdge = fLightCameraOrthographicMinX; // todo make float temp
				iComponent = 0;
			}
			else if (frustumPlaneIter == 1)
			{
				fEdge = fLightCameraOrthographicMaxX;
				iComponent = 0;
			}
			else if (frustumPlaneIter == 2)
			{
				fEdge = fLightCameraOrthographicMinY;
				iComponent = 1;
			}
			else
			{
				fEdge = fLightCameraOrthographicMaxY;
				iComponent = 1;
			}

			for (int triIter = 0; triIter < iTriangleCnt; ++triIter)
			{
				// We don't delete triangles, so we skip those that have been culled.
				if (!triangleList[triIter].culled)
				{
					int iInsideVertCount = 0;
					D3DXVECTOR3 tempOrder;
					// Test against the correct frustum plane.
					// This could be written more compactly, but it would be harder to understand.

					if (frustumPlaneIter == 0)
					{
						for (int triPtIter = 0; triPtIter < 3; ++triPtIter)
						{
							if (triangleList[triIter].pt[triPtIter].x >
								minExtents.x)
							{
								iPointPassesCollision[triPtIter] = 1;
							}
							else
							{
								iPointPassesCollision[triPtIter] = 0;
							}
							iInsideVertCount += iPointPassesCollision[triPtIter];
						}
					}
					else if (frustumPlaneIter == 1)
					{
						for (int triPtIter = 0; triPtIter < 3; ++triPtIter)
						{
							if (triangleList[triIter].pt[triPtIter].x <
								maxExtents.x)
							{
								iPointPassesCollision[triPtIter] = 1;
							}
							else
							{
								iPointPassesCollision[triPtIter] = 0;
							}
							iInsideVertCount += iPointPassesCollision[triPtIter];
						}
					}
					else if (frustumPlaneIter == 2)
					{
						for (int triPtIter = 0; triPtIter < 3; ++triPtIter)
						{
							if (triangleList[triIter].pt[triPtIter].y >
								minExtents.y)
							{
								iPointPassesCollision[triPtIter] = 1;
							}
							else
							{
								iPointPassesCollision[triPtIter] = 0;
							}
							iInsideVertCount += iPointPassesCollision[triPtIter];
						}
					}
					else
					{
						for (int triPtIter = 0; triPtIter < 3; ++triPtIter)
						{
							if (triangleList[triIter].pt[triPtIter].y <
								maxExtents.y)
							{
								iPointPassesCollision[triPtIter] = 1;
							}
							else
							{
								iPointPassesCollision[triPtIter] = 0;
							}
							iInsideVertCount += iPointPassesCollision[triPtIter];
						}
					}

					// Move the points that pass the frustum test to the begining of the array.
					if (iPointPassesCollision[1] && !iPointPassesCollision[0])
					{
						tempOrder = triangleList[triIter].pt[0];
						triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
						triangleList[triIter].pt[1] = tempOrder;
						iPointPassesCollision[0] = true;
						iPointPassesCollision[1] = false;
					}
					if (iPointPassesCollision[2] && !iPointPassesCollision[1])
					{
						tempOrder = triangleList[triIter].pt[1];
						triangleList[triIter].pt[1] = triangleList[triIter].pt[2];
						triangleList[triIter].pt[2] = tempOrder;
						iPointPassesCollision[1] = true;
						iPointPassesCollision[2] = false;
					}
					if (iPointPassesCollision[1] && !iPointPassesCollision[0])
					{
						tempOrder = triangleList[triIter].pt[0];
						triangleList[triIter].pt[0] = triangleList[triIter].pt[1];
						triangleList[triIter].pt[1] = tempOrder;
						iPointPassesCollision[0] = true;
						iPointPassesCollision[1] = false;
					}

					if (iInsideVertCount == 0)
					{ // All points failed. We're done,  
						triangleList[triIter].culled = true;
					}
					else if (iInsideVertCount == 1)
					{// One point passed. Clip the triangle against the Frustum plane
						triangleList[triIter].culled = false;

						// 
						D3DXVECTOR3 vVert0ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[0];
						D3DXVECTOR3 vVert0ToVert2 = triangleList[triIter].pt[2] - triangleList[triIter].pt[0];

						// Find the collision ratio.
						FLOAT fHitPointTimeRatio = fEdge - (iComponent == 0 ? triangleList[triIter].pt[0].x : triangleList[triIter].pt[0].y);
						// Calculate the distance along the vector as ratio of the hit ratio to the component.
						FLOAT fDistanceAlongVector01 = fHitPointTimeRatio / (iComponent == 0 ? vVert0ToVert1.x : vVert0ToVert1.y);
						FLOAT fDistanceAlongVector02 = fHitPointTimeRatio / (iComponent == 0 ? vVert0ToVert2.x : vVert0ToVert2.y);
						// Add the point plus a percentage of the vector.
						vVert0ToVert1 *= fDistanceAlongVector01;
						vVert0ToVert1 += triangleList[triIter].pt[0];
						vVert0ToVert2 *= fDistanceAlongVector02;
						vVert0ToVert2 += triangleList[triIter].pt[0];

						triangleList[triIter].pt[1] = vVert0ToVert2;
						triangleList[triIter].pt[2] = vVert0ToVert1;

					}
					else if (iInsideVertCount == 2)
					{ // 2 in  // tesselate into 2 triangles


						// Copy the triangle\(if it exists) after the current triangle out of
						// the way so we can override it with the new triangle we're inserting.
						triangleList[iTriangleCnt] = triangleList[triIter + 1];

						triangleList[triIter].culled = false;
						triangleList[triIter + 1].culled = false;

						// Get the vector from the outside point into the 2 inside points.
						D3DXVECTOR3 vVert2ToVert0 = triangleList[triIter].pt[0] - triangleList[triIter].pt[2];
						D3DXVECTOR3 vVert2ToVert1 = triangleList[triIter].pt[1] - triangleList[triIter].pt[2];

						// Get the hit point ratio.
						FLOAT fHitPointTime_2_0 = fEdge - (iComponent == 0 ? triangleList[triIter].pt[2].x : triangleList[triIter].pt[2].y);
						FLOAT fDistanceAlongVector_2_0 = fHitPointTime_2_0 / (iComponent == 0 ? vVert2ToVert0.x : vVert2ToVert0.y);
						// Calcaulte the new vert by adding the percentage of the vector plus point 2.
						vVert2ToVert0 *= fDistanceAlongVector_2_0;
						vVert2ToVert0 += triangleList[triIter].pt[2];

						// Add a new triangle.
						triangleList[triIter + 1].pt[0] = triangleList[triIter].pt[0];
						triangleList[triIter + 1].pt[1] = triangleList[triIter].pt[1];
						triangleList[triIter + 1].pt[2] = vVert2ToVert0;

						//Get the hit point ratio.
						FLOAT fHitPointTime_2_1 = fEdge - (iComponent == 0 ? triangleList[triIter].pt[2].x : triangleList[triIter].pt[2].y);
						FLOAT fDistanceAlongVector_2_1 = fHitPointTime_2_1 / (iComponent == 0 ? vVert2ToVert1.x : vVert2ToVert1.y);
						vVert2ToVert1 *= fDistanceAlongVector_2_1;
						vVert2ToVert1 += triangleList[triIter].pt[2];
						triangleList[triIter].pt[0] = triangleList[triIter + 1].pt[1];
						triangleList[triIter].pt[1] = triangleList[triIter + 1].pt[2];
						triangleList[triIter].pt[2] = vVert2ToVert1;
						// Cncrement triangle count and skip the triangle we just inserted.
						++iTriangleCnt;
						++triIter;


					}
					else
					{ // all in
						triangleList[triIter].culled = false;

					}
				}// end if !culled loop            
			}
		}
		for (int index = 0; index < iTriangleCnt; ++index)
		{
			if (!triangleList[index].culled)
			{
				// Set the near and far plan and the min and max z values respectivly.
				for (int vertind = 0; vertind < 3; ++vertind)
				{
					float fTriangleCoordZ = triangleList[index].pt[vertind].z;
					if (nearPlane > fTriangleCoordZ)
					{
						nearPlane = fTriangleCoordZ;
					}
					if (farPlane < fTriangleCoordZ)
					{
						farPlane = fTriangleCoordZ;
					}
				}
			}
		}
	}
	
	Logger::Log("%f %f", nearPlane, farPlane);


}


// calculates the minimum area viewproj matrix for a given cascade using cascade depth and frustum corners
D3DXMATRIX ShadowsExteriorEffect::GetCascadeViewProj(ShadowMapSettings* ShadowMap, D3DXMATRIX View, D3DXVECTOR4* SunDir) {
	float debug = TheShaderManager->Effects.Debug->Constants.DebugVar.x;

	if (debug == 0.0f)
		return GetCascadeViewProj_Math(ShadowMap, View);
	else if (debug == 0.1f)
		return GetCascadeViewProj_MLP(ShadowMap, View, SunDir);
	else if (debug == 0.2f)
		return GetCascadeViewProj_FixedWS(ShadowMap, View, SunDir);
	else if (debug == 0.3f)
		return GetCascadeViewProj_DX(ShadowMap, View, SunDir);
	else if (debug == 0.4f)
		return GetCascadeViewProj_ShaderX7(ShadowMap, View, SunDir);
	else
		return GetCascadeViewProj_Math(ShadowMap, View);
}


// calculates the minimum area viewproj matrix for a given cascade using cascade depth and frustum corners
D3DXMATRIX ShadowsExteriorEffect::GetCascadeViewProj_Math(ShadowMapSettings* ShadowMap, D3DXMATRIX View) {
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

float BankerRound(float num) {
	float rounded = round(num);

	if (fabs(rounded - num) == 0.5f) {
		return 2.0 * round(0.5 * num);
	}

	return rounded;
}

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

// calculates the minimum area viewproj matrix for a given cascade using cascade depth and frustum corners
D3DXMATRIX ShadowsExteriorEffect::GetCascadeViewProj_MLP(ShadowMapSettings* ShadowMap, D3DXMATRIX View, D3DXVECTOR4* SunDir) {
	// Get z-range for this cascade.
	NiCamera* sceneCamera = WorldSceneGraph->camera;
	float depthRange = (sceneCamera->Frustum.Far - sceneCamera->Frustum.Near);

	float zNear = ShadowMap->ShadowMapNear / depthRange;
	//float zNear = TheShaderManager->Effects.Debug->Constants.DebugVar.y == 0 ? ShadowMap->ShadowMapNear / depthRange : 0.0f;
	float zFar = ShadowMap->ShadowMapRadius / depthRange;

	Logger::Log("near: %f radius: %f depthRange: %f zNear: %f zFar: %f", ShadowMap->ShadowMapNear, ShadowMap->ShadowMapRadius, depthRange, zNear, zFar);
	
	// Calculate the frustum corners in world space (from a unit cube in projective space).
	D3DXMATRIX invViewProj = TheRenderManager->InvViewProjMatrix;
	
	D3DXVECTOR3 frustumCorners[8] = {
		D3DXVECTOR3(-1.0f,  1.0f, 0.0f),  // Near plane.
		D3DXVECTOR3( 1.0f,  1.0f, 0.0f),
		D3DXVECTOR3( 1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(-1.0f,  1.0f, 1.0f),  // Far plane.
		D3DXVECTOR3( 1.0f,  1.0f, 1.0f),
		D3DXVECTOR3( 1.0f, -1.0f, 1.0f),
		D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
	};

	for (auto i = 0; i < 8; ++i) {
		D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &invViewProj);
		//Logger::Log("Corner %d: %f %f %f", i, frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
	}

	D3DXVECTOR3 sun(*SunDir);
	Logger::Log("Sun %f %f %f", sun.x, sun.y, sun.z);

	float theta = atan2f(sun.y, sun.x);
	float phi = acosf(sun.z);
	theta = BankerRound(theta * 2400.0f) / 2400.0f;
	phi = BankerRound(phi * 2400.0f) / 2400.0f;
	

	sun.x = sinf(phi) * cosf(theta);
	sun.y = sinf(phi) * sinf(theta);
	sun.z = cosf(phi);

	D3DXVec3Normalize(&sun, &sun);
	Logger::Log("Sun %f %f %f", sun.x, sun.y, sun.z);

	// Get the corners of the current cascade slice of the view frustum.
	for (auto i = 0; i < 4; ++i)
	{
		D3DXVECTOR3 cornerRay = frustumCorners[i + 4] - frustumCorners[i];
		D3DXVECTOR3 nearCornerRay = cornerRay * zNear;
		D3DXVECTOR3 farCornerRay = cornerRay * zFar;
		frustumCorners[i + 4] = frustumCorners[i] + farCornerRay;
		frustumCorners[i] = frustumCorners[i] + nearCornerRay;
	}

	for (auto i = 0; i < 8; ++i) {
		Logger::Log("Corner %d: %f %f %f", i, frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
	}

	// Calculate the centroid of the view frustum slice.
	D3DXVECTOR3 frustumCenter(0.0f, 0.0f, 0.0f);
	for (auto i = 0; i < 8; ++i)
		frustumCenter = frustumCenter + frustumCorners[i];
	frustumCenter *= 1.0f / 8.0f;

	Logger::Log("Frustum center: %f %f %f", frustumCenter.x, frustumCenter.y, frustumCenter.z);

	D3DXVECTOR3 upDir(0.0f, 0.0f, 1.0f);
	
	D3DXVECTOR3 minExtents, maxExtents;
	if (TheShaderManager->Effects.Debug->Constants.DebugVar.y) {
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

		// Calculate the number of texels per unit of distance in world space.
		/*float texelsPerUnit = (sphereRadius * 2.0f) * ShadowMap->ShadowMapInverseResolution;

		frustumCenter.x = (float)roundf(frustumCenter.x * texelsPerUnit) / texelsPerUnit;
		frustumCenter.y = (float)roundf(frustumCenter.y * texelsPerUnit) / texelsPerUnit;

		Logger::Log("Frustum center: %f %f %f", frustumCenter.x, frustumCenter.y, frustumCenter.z);*/
	}
	else {
		// Create a temporary view matrix for the light
		D3DXVECTOR3 lightCameraPos = frustumCenter;
		D3DXVECTOR3 lookAt = frustumCenter - D3DXVECTOR3(*SunDir);
		D3DXMATRIX lightView;
		D3DXMatrixLookAtRH(&lightView, &lightCameraPos, &lookAt, &upDir);

		// Calculate an AABB around the frustum corners
		D3DXVECTOR3 mins(FLT_MAX, FLT_MAX, FLT_MAX);
		D3DXVECTOR3 maxes(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (auto i = 0; i < 8; ++i)
		{
			D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &lightView);
			D3DXVec3Minimize(&mins, &mins, &frustumCorners[i]);
			D3DXVec3Maximize(&maxes, &maxes, &frustumCorners[i]);
		}

		minExtents = mins;
		maxExtents = maxes;
	}

	float nearPlane = Settings.Exteriors.ShadowMapFarPlane * 0.6f;
	float farPlane = Settings.Exteriors.ShadowMapFarPlane * 1.6f;

	//ComputeNearAndFar(nearPlane, farPlane, minExtents, maxExtents, sceneCorners);

	Logger::Log("Near %f Far %f", nearPlane, farPlane);

	D3DXVECTOR3 cascadeExtents = maxExtents - minExtents;

	Logger::Log("minExtents %f %f %f", minExtents.x, minExtents.y, minExtents.z);
	Logger::Log("maxExtents %f %f %f", maxExtents.x, maxExtents.y, maxExtents.z);
	Logger::Log("cascadeExtents %f %f %f", cascadeExtents.x, cascadeExtents.y, cascadeExtents.z);

	D3DXVECTOR3 shadowCameraPos = frustumCenter + sun * Settings.Exteriors.ShadowMapFarPlane;
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * nearPlane;
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * (-minExtents.z);
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * (minExtents.z);

	D3DXMATRIX shadowView, shadowProj, shadowViewProj;
	
	D3DXMatrixLookAtRH(&shadowView, &shadowCameraPos, &frustumCenter, &upDir);

	//D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z);
	//D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, Settings.Exteriors.ShadowMapFarPlane * 0.6f, Settings.Exteriors.ShadowMapFarPlane * 1.6f);
	D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, nearPlane, farPlane);
	shadowViewProj = shadowView * shadowProj;

	Logger::Log("Shadow view 1: %f %f %f %f", shadowView._11, shadowView._12, shadowView._13, shadowView._14);
	Logger::Log("Shadow view 2: %f %f %f %f", shadowView._21, shadowView._22, shadowView._23, shadowView._24);
	Logger::Log("Shadow view 3: %f %f %f %f", shadowView._31, shadowView._32, shadowView._33, shadowView._34);
	Logger::Log("Shadow view 4: %f %f %f %f", shadowView._41, shadowView._42, shadowView._43, shadowView._44);

	Logger::Log("Shadow proj 1: %f %f %f %f", shadowProj._11, shadowProj._12, shadowProj._13, shadowProj._14);
	Logger::Log("Shadow proj 2: %f %f %f %f", shadowProj._21, shadowProj._22, shadowProj._23, shadowProj._24);
	Logger::Log("Shadow proj 3: %f %f %f %f", shadowProj._31, shadowProj._32, shadowProj._33, shadowProj._34);
	Logger::Log("Shadow proj 4: %f %f %f %f", shadowProj._41, shadowProj._42, shadowProj._43, shadowProj._44);

	Logger::Log("Shadow viewproj 1: %f %f %f %f", shadowViewProj._11, shadowViewProj._12, shadowViewProj._13, shadowViewProj._14);
	Logger::Log("Shadow viewproj 2: %f %f %f %f", shadowViewProj._21, shadowViewProj._22, shadowViewProj._23, shadowViewProj._24);
	Logger::Log("Shadow viewproj 3: %f %f %f %f", shadowViewProj._31, shadowViewProj._32, shadowViewProj._33, shadowViewProj._34);
	Logger::Log("Shadow viewproj 4: %f %f %f %f", shadowViewProj._41, shadowViewProj._42, shadowViewProj._43, shadowViewProj._44);

	if (TheShaderManager->Effects.Debug->Constants.DebugVar.y) {
		// Create the rounding matrix, by projecting the world-space origin and determining
		// the fractional offset in texel space.
		float sMapSize = 2048.0f; //ShadowMap->ShadowMapViewPort.Height;

		D3DXVECTOR4 shadowOrigin(-sceneCamera->m_worldTransform.pos.x, -sceneCamera->m_worldTransform.pos.y, -sceneCamera->m_worldTransform.pos.z, 1.0f);
		Logger::Log("Shadow origin WS: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);
		D3DXVec4Transform(&shadowOrigin, &shadowOrigin, &shadowViewProj);
		Logger::Log("Shadow origin PS: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);
		D3DXVec4Scale(&shadowOrigin, &shadowOrigin, sMapSize / 2.0f);
		Logger::Log("Shadow origin PS scaled: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);

		D3DXVECTOR4 roundedOrigin, roundOffset;
		Vector4Round(&roundedOrigin, &shadowOrigin);
		Logger::Log("Shadow origin PS rounded: (%f, %f, %f, %f)", roundedOrigin.x, roundedOrigin.y, roundedOrigin.z, roundedOrigin.w);
		D3DXVec4Subtract(&roundOffset, &roundedOrigin, &shadowOrigin);
		Logger::Log("Shadow origin diff: (%f, %f)", roundOffset.x, roundOffset.y);
		D3DXVec4Scale(&roundOffset, &roundOffset, 2.0f / sMapSize);
		Logger::Log("Shadow origin diff scaled: (%f, %f)", roundOffset.x, roundOffset.y);

		shadowProj._41 = shadowProj._41 + roundOffset.x;
		shadowProj._42 = shadowProj._42 + roundOffset.y;

		shadowViewProj = shadowView * shadowProj;

		shadowOrigin = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
		Logger::Log("Shadow origin WS: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);
		D3DXVec4Transform(&shadowOrigin, &shadowOrigin, &shadowViewProj);
		Logger::Log("Shadow origin PS: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);
		D3DXVec4Scale(&shadowOrigin, &shadowOrigin, sMapSize / 2.0f);
		Logger::Log("Shadow origin PS scaled: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);

		Vector4Round(&roundedOrigin, &shadowOrigin);
		Logger::Log("Shadow origin PS rounded: (%f, %f, %f, %f)", roundedOrigin.x, roundedOrigin.y, roundedOrigin.z, roundedOrigin.w);
		D3DXVec4Subtract(&roundOffset, &roundedOrigin, &shadowOrigin);
		Logger::Log("Shadow origin diff: (%f, %f)", roundOffset.x, roundOffset.y);
		D3DXVec4Scale(&roundOffset, &roundOffset, 2.0f / sMapSize);
		Logger::Log("Shadow origin diff scaled: (%f, %f)", roundOffset.x, roundOffset.y);

		shadowOrigin = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		Logger::Log("Shadow origin WS: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);
		D3DXVec4Transform(&shadowOrigin, &shadowOrigin, &shadowViewProj);
		Logger::Log("Shadow origin PS: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);
		D3DXVec4Scale(&shadowOrigin, &shadowOrigin, sMapSize / 2.0f);
		Logger::Log("Shadow origin PS scaled: (%f, %f, %f, %f)", shadowOrigin.x, shadowOrigin.y, shadowOrigin.z, shadowOrigin.w);

		Vector4Round(&roundedOrigin, &shadowOrigin);
		Logger::Log("Shadow origin PS rounded: (%f, %f, %f, %f)", roundedOrigin.x, roundedOrigin.y, roundedOrigin.z, roundedOrigin.w);
		D3DXVec4Subtract(&roundOffset, &roundedOrigin, &shadowOrigin);
		Logger::Log("Shadow origin diff: (%f, %f)", roundOffset.x, roundOffset.y);
		D3DXVec4Scale(&roundOffset, &roundOffset, 2.0f / sMapSize);
		Logger::Log("Shadow origin diff scaled: (%f, %f)", roundOffset.x, roundOffset.y);

		Logger::Log("Shadow viewproj 1: %f %f %f %f", shadowViewProj._11, shadowViewProj._12, shadowViewProj._13, shadowViewProj._14);
		Logger::Log("Shadow viewproj 2: %f %f %f %f", shadowViewProj._21, shadowViewProj._22, shadowViewProj._23, shadowViewProj._24);
		Logger::Log("Shadow viewproj 3: %f %f %f %f", shadowViewProj._31, shadowViewProj._32, shadowViewProj._33, shadowViewProj._34);
		Logger::Log("Shadow viewproj 4: %f %f %f %f", shadowViewProj._41, shadowViewProj._42, shadowViewProj._43, shadowViewProj._44);
	}

	return shadowViewProj;
}


// calculates the minimum area viewproj matrix for a given cascade using cascade depth and frustum corners
D3DXMATRIX ShadowsExteriorEffect::GetCascadeViewProj_FixedWS(ShadowMapSettings* ShadowMap, D3DXMATRIX View, D3DXVECTOR4* SunDir) {
	// Get z-range for this cascade.
	NiCamera* sceneCamera = WorldSceneGraph->camera;
	float depthRange = (sceneCamera->Frustum.Far - sceneCamera->Frustum.Near);

	float zNear = ShadowMap->ShadowMapNear / depthRange;
	//float zNear = TheShaderManager->Effects.Debug->Constants.DebugVar.y == 0 ? ShadowMap->ShadowMapNear / depthRange : 0.0f;
	float zFar = ShadowMap->ShadowMapRadius / depthRange;

	Logger::Log("near: %f radius: %f depthRange: %f zNear: %f zFar: %f", ShadowMap->ShadowMapNear, ShadowMap->ShadowMapRadius, depthRange, zNear, zFar);

	// inv view matrix taking into account camera position properly
	NiMatrix33* WorldRotate = &sceneCamera->m_worldTransform.rot;
	NiPoint3* WorldTranslate = &sceneCamera->m_worldTransform.pos;

	D3DXVECTOR4 cameraPos = WorldTranslate->toD3DXVEC4();

	NiPoint3 Forward = { 0.0f, 0.0f, 0.0f };
	NiPoint3 Up = { 0.0f, 0.0f, 0.0f };
	NiPoint3 Right = { 0.0f, 0.0f, 0.0f };

	Forward.x = WorldRotate->data[0][0];
	Forward.y = WorldRotate->data[1][0];
	Forward.z = WorldRotate->data[2][0];
	Up.x = WorldRotate->data[0][1];
	Up.y = WorldRotate->data[1][1];
	Up.z = WorldRotate->data[2][1];
	Right.x = WorldRotate->data[0][2];
	Right.y = WorldRotate->data[1][2];
	Right.z = WorldRotate->data[2][2];

	D3DXMATRIX invView;
	invView._11 = Right.x;
	invView._12 = Right.y;
	invView._13 = Right.z;
	invView._14 = 0.0f;
	invView._21 = Up.x;
	invView._22 = Up.y;
	invView._23 = Up.z;
	invView._24 = 0.0f;
	invView._31 = Forward.x;
	invView._32 = Forward.y;
	invView._33 = Forward.z;
	invView._34 = 0.0f;
	invView._41 = cameraPos.x;
	invView._42 = cameraPos.y;
	invView._43 = cameraPos.z;
	invView._44 = 1.0f;

	D3DXMATRIX cameraToAbsolute;
	D3DXMatrixTranslation(&cameraToAbsolute, cameraPos.x, cameraPos.y, cameraPos.z);

	// Calculate the frustum corners in world space (from a unit cube in projective space).
	D3DXMATRIX invViewProj = TheRenderManager->InvProjMatrix * invView;

	D3DXVECTOR3 frustumCorners[8] = {
		D3DXVECTOR3(-1.0f,  1.0f, 0.0f),  // Near plane.
		D3DXVECTOR3(1.0f,  1.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(-1.0f,  1.0f, 1.0f),  // Far plane.
		D3DXVECTOR3(1.0f,  1.0f, 1.0f),
		D3DXVECTOR3(1.0f, -1.0f, 1.0f),
		D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
	};

	for (auto i = 0; i < 8; ++i) {
		D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &invViewProj);
		//Logger::Log("Corner %d: %f %f %f", i, frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
	}

	Logger::Log("sunDir: %f %f %f", SunDir->x, SunDir->y, SunDir->z);

	// Get the corners of the current cascade slice of the view frustum.
	for (auto i = 0; i < 4; ++i)
	{
		D3DXVECTOR3 cornerRay = frustumCorners[i + 4] - frustumCorners[i];
		D3DXVECTOR3 nearCornerRay = cornerRay * zNear;
		D3DXVECTOR3 farCornerRay = cornerRay * zFar;
		frustumCorners[i + 4] = frustumCorners[i] + farCornerRay;
		frustumCorners[i] = frustumCorners[i] + nearCornerRay;
	}

	for (auto i = 0; i < 8; ++i) {
		Logger::Log("Corner %d: %f %f %f", i, frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
	}

	// Calculate the centroid of the view frustum slice.
	D3DXVECTOR3 frustumCenter(0.0f, 0.0f, 0.0f);
	for (auto i = 0; i < 8; ++i)
		frustumCenter = frustumCenter + frustumCorners[i];
	frustumCenter *= 1.0f / 8.0f;

	Logger::Log("Frustum center: %f %f %f", frustumCenter.x, frustumCenter.y, frustumCenter.z);

	D3DXVECTOR3 upDir(0.0f, 0.0f, 1.0f);

	D3DXVECTOR3 minExtents, maxExtents;
	if (TheShaderManager->Effects.Debug->Constants.DebugVar.y) {
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
	}
	else {
		// Create a temporary view matrix for the light
		D3DXVECTOR3 lightCameraPos = frustumCenter;
		D3DXVECTOR3 lookAt = frustumCenter - D3DXVECTOR3(*SunDir);
		D3DXMATRIX lightView;
		D3DXMatrixLookAtRH(&lightView, &lightCameraPos, &lookAt, &upDir);

		// Calculate an AABB around the frustum corners
		D3DXVECTOR3 mins(FLT_MAX, FLT_MAX, FLT_MAX);
		D3DXVECTOR3 maxes(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (auto i = 0; i < 8; ++i)
		{
			D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &lightView);
			D3DXVec3Minimize(&mins, &mins, &frustumCorners[i]);
			D3DXVec3Maximize(&maxes, &maxes, &frustumCorners[i]);
		}

		minExtents = mins;
		maxExtents = maxes;
	}

	float nearPlane = Settings.Exteriors.ShadowMapFarPlane * 0.6f;
	float farPlane = Settings.Exteriors.ShadowMapFarPlane * 1.6f;

	//ComputeNearAndFar(nearPlane, farPlane, minExtents, maxExtents, sceneCorners);

	Logger::Log("Near %f Far %f", nearPlane, farPlane);

	D3DXVECTOR3 cascadeExtents = maxExtents - minExtents;

	Logger::Log("minExtents %f %f %f", minExtents.x, minExtents.y, minExtents.z);
	Logger::Log("maxExtents %f %f %f", maxExtents.x, maxExtents.y, maxExtents.z);
	Logger::Log("cascadeExtents %f %f %f", cascadeExtents.x, cascadeExtents.y, cascadeExtents.z);

	D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * Settings.Exteriors.ShadowMapFarPlane;
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * nearPlane;
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * (-minExtents.z);
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * (minExtents.z);

	D3DXMATRIX shadowView, shadowProj, shadowViewProj;

	D3DXMatrixLookAtRH(&shadowView, &shadowCameraPos, &frustumCenter, &upDir);

	//D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z);
	//D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, Settings.Exteriors.ShadowMapFarPlane * 0.6f, Settings.Exteriors.ShadowMapFarPlane * 1.6f);
	D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, nearPlane, farPlane);
	shadowViewProj = shadowView * shadowProj;

	Logger::Log("Shadow view 1: %f %f %f %f", shadowView._11, shadowView._12, shadowView._13, shadowView._14);
	Logger::Log("Shadow view 2: %f %f %f %f", shadowView._21, shadowView._22, shadowView._23, shadowView._24);
	Logger::Log("Shadow view 3: %f %f %f %f", shadowView._31, shadowView._32, shadowView._33, shadowView._34);
	Logger::Log("Shadow view 4: %f %f %f %f", shadowView._41, shadowView._42, shadowView._43, shadowView._44);

	Logger::Log("Shadow proj 1: %f %f %f %f", shadowProj._11, shadowProj._12, shadowProj._13, shadowProj._14);
	Logger::Log("Shadow proj 2: %f %f %f %f", shadowProj._21, shadowProj._22, shadowProj._23, shadowProj._24);
	Logger::Log("Shadow proj 3: %f %f %f %f", shadowProj._31, shadowProj._32, shadowProj._33, shadowProj._34);
	Logger::Log("Shadow proj 4: %f %f %f %f", shadowProj._41, shadowProj._42, shadowProj._43, shadowProj._44);

	Logger::Log("Shadow viewproj 1: %f %f %f %f", shadowViewProj._11, shadowViewProj._12, shadowViewProj._13, shadowViewProj._14);
	Logger::Log("Shadow viewproj 2: %f %f %f %f", shadowViewProj._21, shadowViewProj._22, shadowViewProj._23, shadowViewProj._24);
	Logger::Log("Shadow viewproj 3: %f %f %f %f", shadowViewProj._31, shadowViewProj._32, shadowViewProj._33, shadowViewProj._34);
	Logger::Log("Shadow viewproj 4: %f %f %f %f", shadowViewProj._41, shadowViewProj._42, shadowViewProj._43, shadowViewProj._44);

	if (TheShaderManager->Effects.Debug->Constants.DebugVar.y) {
		// Create the rounding matrix, by projecting the world-space origin and determining
		// the fractional offset in texel space.
		float sMapSize = 2048.0f; //ShadowMap->ShadowMapViewPort.Height;

		D3DXVECTOR4 shadowOrigin;
		if (TheShaderManager->Effects.Debug->Constants.DebugVar.y == 0.1f) {
			shadowOrigin = D3DXVECTOR4(cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
		} 
		else if (TheShaderManager->Effects.Debug->Constants.DebugVar.y == 0.2f) {
			shadowOrigin = D3DXVECTOR4(frustumCenter.x, frustumCenter.y, frustumCenter.z, 1.0f);
		}
		else {
			float x = ceil(cameraPos.x / 1000.0f) * 1000.0f;
			float y = ceil(cameraPos.y / 1000.0f) * 1000.0f;
			float z = ceil(cameraPos.z / 1000.0f) * 1000.0f;
			shadowOrigin = D3DXVECTOR4(x, y, z, 1.0f);
		}
		D3DXVec4Transform(&shadowOrigin, &shadowOrigin, &shadowViewProj);
		D3DXVec4Scale(&shadowOrigin, &shadowOrigin, sMapSize / 2.0f);

		D3DXVECTOR4 roundedOrigin, roundOffset;
		Vector4Round(&roundedOrigin, &shadowOrigin);
		D3DXVec4Subtract(&roundOffset, &roundedOrigin, &shadowOrigin);
		D3DXVec4Scale(&roundOffset, &roundOffset, 2.0f / sMapSize);

		shadowProj._41 = shadowProj._41 + roundOffset.x;
		shadowProj._42 = shadowProj._42 + roundOffset.y;

		shadowViewProj = shadowView * shadowProj;

		Logger::Log("Shadow viewproj 1: %f %f %f %f", shadowViewProj._11, shadowViewProj._12, shadowViewProj._13, shadowViewProj._14);
		Logger::Log("Shadow viewproj 2: %f %f %f %f", shadowViewProj._21, shadowViewProj._22, shadowViewProj._23, shadowViewProj._24);
		Logger::Log("Shadow viewproj 3: %f %f %f %f", shadowViewProj._31, shadowViewProj._32, shadowViewProj._33, shadowViewProj._34);
		Logger::Log("Shadow viewproj 4: %f %f %f %f", shadowViewProj._41, shadowViewProj._42, shadowViewProj._43, shadowViewProj._44);
	}

	return cameraToAbsolute * shadowViewProj;
}


// calculates the minimum area viewproj matrix for a given cascade using cascade depth and frustum corners
D3DXMATRIX ShadowsExteriorEffect::GetCascadeViewProj_DX(ShadowMapSettings* ShadowMap, D3DXMATRIX View, D3DXVECTOR4* SunDir) {
	// Get z-range for this cascade.
	NiCamera* sceneCamera = WorldSceneGraph->camera;
	float depthRange = (sceneCamera->Frustum.Far - sceneCamera->Frustum.Near);

	float zNear = 0.0f;  // Near plane stays the same for each cascade -> fitting to scene.
	float zFar = ShadowMap->ShadowMapRadius / depthRange;

	Logger::Log("near: %f radius: %f depthRange: %f zNear: %f zFar: %f", ShadowMap->ShadowMapNear, ShadowMap->ShadowMapRadius, depthRange, zNear, zFar);

	// Calculate the frustum corners in world space (from a unit cube in projective space).
	D3DXMATRIX invViewProj = TheRenderManager->InvViewProjMatrix;
	D3DXMATRIX invView = TheRenderManager->InvViewMatrix;

	D3DXVECTOR3 vRightTop(sceneCamera->Frustum.Right, sceneCamera->Frustum.Top, 1.0f);
	D3DXVECTOR3 vLeftBottom(sceneCamera->Frustum.Left, sceneCamera->Frustum.Bottom, 1.0f);
	D3DXVECTOR3 vNear(zNear, zNear, zNear);
	D3DXVECTOR3 vFar(ShadowMap->ShadowMapRadius, ShadowMap->ShadowMapRadius, ShadowMap->ShadowMapRadius);
	D3DXVECTOR3 vRightTopNear(vRightTop.x * vNear.x, vRightTop.y * vNear.y, vRightTop.z * vNear.z);
	D3DXVECTOR3 vRightTopFar(vRightTop.x * vFar.x, vRightTop.y * vFar.y, vRightTop.z * vFar.z);
	D3DXVECTOR3 vLeftBottomNear(vLeftBottom.x * vNear.x, vLeftBottom.y * vNear.y, vLeftBottom.z * vNear.z);
	D3DXVECTOR3 vLeftBottomFar(vLeftBottom.x * vFar.x, vLeftBottom.y * vFar.y, vLeftBottom.z * vFar.z);

	D3DXVECTOR3 frustumCorners[8] = {
		vRightTopNear,  // Near plane.
		D3DXVECTOR3(vLeftBottomNear.x, vRightTopNear.y, vRightTopNear.z),
		vLeftBottomNear,
		D3DXVECTOR3(vRightTopNear.x, vLeftBottomNear.y, vRightTopNear.z),
		vRightTopFar,  // Far plane.
		D3DXVECTOR3(vLeftBottomFar.x, vRightTopFar.y, vRightTopFar.z),
		vLeftBottomFar,
		D3DXVECTOR3(vRightTopFar.x, vLeftBottomFar.y, vRightTopFar.z),
	};

	for (auto i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &invView);
	}

	//D3DXVECTOR3 frustumCorners[8] = {
	//	D3DXVECTOR3(-1.0f,  1.0f, 0.0f),  // Near plane.
	//	D3DXVECTOR3(1.0f,  1.0f, 0.0f),
	//	D3DXVECTOR3(1.0f, -1.0f, 0.0f),
	//	D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
	//	D3DXVECTOR3(-1.0f,  1.0f, 1.0f),  // Far plane.
	//	D3DXVECTOR3(1.0f,  1.0f, 1.0f),
	//	D3DXVECTOR3(1.0f, -1.0f, 1.0f),
	//	D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
	//};

	//for (auto i = 0; i < 8; ++i) {
	//	D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &invViewProj);
	//	//Logger::Log("Corner %d: %f %f %f", i, frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
	//}

	//Logger::Log("sunDir: %f %f %f", SunDir->x, SunDir->y, SunDir->z);

	//// Get the corners of the current cascade slice of the view frustum.
	//for (auto i = 0; i < 4; ++i)
	//{
	//	D3DXVECTOR3 cornerRay = frustumCorners[i + 4] - frustumCorners[i];
	//	D3DXVECTOR3 nearCornerRay = cornerRay * zNear;
	//	D3DXVECTOR3 farCornerRay = cornerRay * zFar;
	//	frustumCorners[i + 4] = frustumCorners[i] + farCornerRay;
	//	frustumCorners[i] = frustumCorners[i] + nearCornerRay;
	//}

	//for (auto i = 0; i < 8; ++i) {
	//	Logger::Log("Corner %d: %f %f %f", i, frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
	//}

	// Calculate the centroid of the view frustum slice.
	D3DXVECTOR3 frustumCenter(0.0f, 0.0f, 0.0f);
	for (auto i = 0; i < 8; ++i)
		frustumCenter = frustumCenter + frustumCorners[i];
	frustumCenter *= 1.0f / 8.0f;

	Logger::Log("Frustum center: %f %f %f", frustumCenter.x, frustumCenter.y, frustumCenter.z);

	D3DXVECTOR3 upDir(0.0f, 0.0f, 1.0f);

	D3DXVECTOR3 minExtents(FLT_MAX, FLT_MAX, FLT_MAX);
	D3DXVECTOR3 maxExtents(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// Create a temporary view matrix for the light
	D3DXVECTOR3 lightCameraPos = frustumCenter;
	D3DXVECTOR3 lookAt = frustumCenter - D3DXVECTOR3(*SunDir);
	D3DXMATRIX lightView;
	D3DXMatrixLookAtRH(&lightView, &lightCameraPos, &lookAt, &upDir);

	// Calculate an AABB around the frustum corners
	D3DXVECTOR3 temp;
	for (auto i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&temp, &frustumCorners[i], &lightView);
		D3DXVec3Minimize(&minExtents, &minExtents, &temp);
		D3DXVec3Maximize(&maxExtents, &maxExtents, &temp);
	}

	D3DXVECTOR3 diagonal = frustumCorners[0] - frustumCorners[6];
	D3DXVECTOR3 forwardDiagonal = frustumCorners[0] - frustumCorners[5];
	float diagLength = max(D3DXVec3Length(&diagonal), D3DXVec3Length(&forwardDiagonal));

	D3DXVECTOR3 boarderOffset = (D3DXVECTOR3(diagLength, diagLength, diagLength) - (maxExtents - minExtents)) * 0.5f;
	boarderOffset.z = 0.0f;

	maxExtents += boarderOffset;
	minExtents -= boarderOffset;

	// The world units per texel are used to snap the shadow the orthographic projection
	// to texel sized increments.  This keeps the edges of the shadows from shimmering.
	float worldUnitsPerTexel = diagLength / (float)ShadowMap->ShadowMapViewPort.Height;

	// We snap the camera to 1 pixel increments so that moving the camera does not cause the shadows to jitter.
	// This is a matter of integer dividing by the world space size of a texel
	minExtents.x = std::round(minExtents.x / worldUnitsPerTexel) * worldUnitsPerTexel;
	minExtents.y = std::round(minExtents.y / worldUnitsPerTexel) * worldUnitsPerTexel;
	maxExtents.x = std::round(maxExtents.x / worldUnitsPerTexel) * worldUnitsPerTexel;
	maxExtents.y = std::round(maxExtents.y / worldUnitsPerTexel) * worldUnitsPerTexel;

	Logger::Log("minExtents %f %f %f", minExtents.x, minExtents.y, minExtents.z);
	Logger::Log("maxExtents %f %f %f", maxExtents.x, maxExtents.y, maxExtents.z);

	D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * Settings.Exteriors.ShadowMapFarPlane;

	D3DXMATRIX shadowView, shadowProj, shadowViewProj;

	D3DXMatrixLookAtRH(&shadowView, &shadowCameraPos, &frustumCenter, &upDir);

	float nearPlane = Settings.Exteriors.ShadowMapFarPlane * 0.6f;
	float farPlane = Settings.Exteriors.ShadowMapFarPlane * 1.6f;

	Logger::Log("Near %f Far %f", nearPlane, farPlane);

	D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, nearPlane, farPlane);
	shadowViewProj = shadowView * shadowProj;

	return shadowViewProj;
}


// calculates the minimum area viewproj matrix for a given cascade using cascade depth and frustum corners
D3DXMATRIX ShadowsExteriorEffect::GetCascadeViewProj_ShaderX7(ShadowMapSettings* ShadowMap, D3DXMATRIX View, D3DXVECTOR4* SunDir) {
	// Get z-range for this cascade.
	NiCamera* sceneCamera = WorldSceneGraph->camera;
	float depthRange = (sceneCamera->Frustum.Far - sceneCamera->Frustum.Near);

	float zNear = ShadowMap->ShadowMapNear / depthRange;
	//float zNear = TheShaderManager->Effects.Debug->Constants.DebugVar.y == 0 ? ShadowMap->ShadowMapNear / depthRange : 0.0f;
	float zFar = ShadowMap->ShadowMapRadius / depthRange;

	Logger::Log("near: %f radius: %f depthRange: %f zNear: %f zFar: %f", ShadowMap->ShadowMapNear, ShadowMap->ShadowMapRadius, depthRange, zNear, zFar);

	// Calculate the frustum corners in world space (from a unit cube in projective space).
	D3DXMATRIX invViewProj = TheRenderManager->InvViewProjMatrix;

	D3DXVECTOR3 frustumCorners[8] = {
		D3DXVECTOR3(-1.0f,  1.0f, 0.0f),  // Near plane.
		D3DXVECTOR3(1.0f,  1.0f, 0.0f),
		D3DXVECTOR3(1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
		D3DXVECTOR3(-1.0f,  1.0f, 1.0f),  // Far plane.
		D3DXVECTOR3(1.0f,  1.0f, 1.0f),
		D3DXVECTOR3(1.0f, -1.0f, 1.0f),
		D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
	};

	for (auto i = 0; i < 8; ++i) {
		D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &invViewProj);
		//Logger::Log("Corner %d: %f %f %f", i, frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
	}

	Logger::Log("sunDir: %f %f %f", SunDir->x, SunDir->y, SunDir->z);

	// Get the corners of the current cascade slice of the view frustum.
	for (auto i = 0; i < 4; ++i)
	{
		D3DXVECTOR3 cornerRay = frustumCorners[i + 4] - frustumCorners[i];
		D3DXVECTOR3 nearCornerRay = cornerRay * zNear;
		D3DXVECTOR3 farCornerRay = cornerRay * zFar;
		frustumCorners[i + 4] = frustumCorners[i] + farCornerRay;
		frustumCorners[i] = frustumCorners[i] + nearCornerRay;
	}

	for (auto i = 0; i < 8; ++i) {
		Logger::Log("Corner %d: %f %f %f", i, frustumCorners[i].x, frustumCorners[i].y, frustumCorners[i].z);
	}

	// Calculate the centroid of the view frustum slice.
	D3DXVECTOR3 frustumCenter(0.0f, 0.0f, 0.0f);
	for (auto i = 0; i < 8; ++i)
		frustumCenter = frustumCenter + frustumCorners[i];
	frustumCenter *= 1.0f / 8.0f;

	Logger::Log("Frustum center: %f %f %f", frustumCenter.x, frustumCenter.y, frustumCenter.z);

	D3DXVECTOR3 upDir(0.0f, 0.0f, 1.0f);

	D3DXVECTOR3 minExtents, maxExtents;

	// Create a temporary view matrix for the light
	D3DXVECTOR3 lightCameraPos = frustumCenter;
	D3DXVECTOR3 lookAt = frustumCenter - D3DXVECTOR3(*SunDir);
	D3DXMATRIX lightView;
	D3DXMatrixLookAtRH(&lightView, &lightCameraPos, &lookAt, &upDir);

	// Calculate an AABB around the frustum corners
	D3DXVECTOR3 mins(FLT_MAX, FLT_MAX, FLT_MAX);
	D3DXVECTOR3 maxes(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (auto i = 0; i < 8; ++i)
	{
		D3DXVec3TransformCoord(&frustumCorners[i], &frustumCorners[i], &lightView);
		D3DXVec3Minimize(&mins, &mins, &frustumCorners[i]);
		D3DXVec3Maximize(&maxes, &maxes, &frustumCorners[i]);
	}

	minExtents = mins;
	maxExtents = maxes;

	// Try to stabilize with ShaderX7 method.
	float scaleX = 2.0f / (maxExtents.x - minExtents.x);
	float scaleY = 2.0f / (maxExtents.y - minExtents.y);

	float scaleQuantizer = 64.0f;

	scaleX = 1.0f / ceilf(1.0f / scaleX * scaleQuantizer) * scaleQuantizer;
	scaleY = 1.0f / ceilf(1.0f / scaleY * scaleQuantizer) * scaleQuantizer;

	// Calculate center offset.
	float offsetX = -0.5f * (maxExtents.x + minExtents.x) * scaleX;
	float offsetY = -0.5f * (maxExtents.y + minExtents.y) * scaleY;

	float inverseHalfRes = 2.0f * ShadowMap->ShadowMapInverseResolution;

	offsetX = ceilf(offsetX / inverseHalfRes) * inverseHalfRes;
	offsetY = ceilf(offsetY / inverseHalfRes) * inverseHalfRes;

	Logger::Log("Scale: %f %f", scaleX, scaleY);
	Logger::Log("Offset: %f %f", offsetX, offsetY);

	float nearPlane = Settings.Exteriors.ShadowMapFarPlane * 0.6f;
	float farPlane = Settings.Exteriors.ShadowMapFarPlane * 1.6f;

	//ComputeNearAndFar(nearPlane, farPlane, minExtents, maxExtents, sceneCorners);

	Logger::Log("Near %f Far %f", nearPlane, farPlane);

	D3DXVECTOR3 cascadeExtents = maxExtents - minExtents;

	Logger::Log("minExtents %f %f %f", minExtents.x, minExtents.y, minExtents.z);
	Logger::Log("maxExtents %f %f %f", maxExtents.x, maxExtents.y, maxExtents.z);
	Logger::Log("cascadeExtents %f %f %f", cascadeExtents.x, cascadeExtents.y, cascadeExtents.z);

	D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * Settings.Exteriors.ShadowMapFarPlane;
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * nearPlane;
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * (-minExtents.z);
	//D3DXVECTOR3 shadowCameraPos = frustumCenter + D3DXVECTOR3(*SunDir) * (minExtents.z);

	D3DXMATRIX shadowView, shadowProj, shadowViewProj;

	D3DXMatrixLookAtRH(&shadowView, &shadowCameraPos, &frustumCenter, &upDir);

	//D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z);
	//D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, Settings.Exteriors.ShadowMapFarPlane * 0.6f, Settings.Exteriors.ShadowMapFarPlane * 1.6f);
	D3DXMatrixOrthoOffCenterRH(&shadowProj, minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, nearPlane, farPlane);
	shadowViewProj = shadowView * shadowProj;

	Logger::Log("Shadow view 1: %f %f %f %f", shadowView._11, shadowView._12, shadowView._13, shadowView._14);
	Logger::Log("Shadow view 2: %f %f %f %f", shadowView._21, shadowView._22, shadowView._23, shadowView._24);
	Logger::Log("Shadow view 3: %f %f %f %f", shadowView._31, shadowView._32, shadowView._33, shadowView._34);
	Logger::Log("Shadow view 4: %f %f %f %f", shadowView._41, shadowView._42, shadowView._43, shadowView._44);

	Logger::Log("Shadow proj 1: %f %f %f %f", shadowProj._11, shadowProj._12, shadowProj._13, shadowProj._14);
	Logger::Log("Shadow proj 2: %f %f %f %f", shadowProj._21, shadowProj._22, shadowProj._23, shadowProj._24);
	Logger::Log("Shadow proj 3: %f %f %f %f", shadowProj._31, shadowProj._32, shadowProj._33, shadowProj._34);
	Logger::Log("Shadow proj 4: %f %f %f %f", shadowProj._41, shadowProj._42, shadowProj._43, shadowProj._44);

	Logger::Log("Shadow viewproj 1: %f %f %f %f", shadowViewProj._11, shadowViewProj._12, shadowViewProj._13, shadowViewProj._14);
	Logger::Log("Shadow viewproj 2: %f %f %f %f", shadowViewProj._21, shadowViewProj._22, shadowViewProj._23, shadowViewProj._24);
	Logger::Log("Shadow viewproj 3: %f %f %f %f", shadowViewProj._31, shadowViewProj._32, shadowViewProj._33, shadowViewProj._34);
	Logger::Log("Shadow viewproj 4: %f %f %f %f", shadowViewProj._41, shadowViewProj._42, shadowViewProj._43, shadowViewProj._44);

	D3DXMATRIX scale, offset;
	D3DXMatrixScaling(&scale, scaleX, scaleY, 1.0f);
	D3DXMatrixTranslation(&offset, offsetX, offsetY, 0.0f);

	return shadowViewProj * offset;
}

void ShadowsExteriorEffect::Render(IDirect3DDevice9* Device, IDirect3DSurface9* RenderTarget, IDirect3DSurface9* RenderedSurface, UINT techniqueIndex, bool ClearRenderTarget, IDirect3DSurface9* SourceBuffer) {
	Logger::Log("Shadow render viewproj 1: %f %f %f %f", ShadowMaps[0].ShadowCameraToLight._11, ShadowMaps[0].ShadowCameraToLight._12, ShadowMaps[0].ShadowCameraToLight._13, ShadowMaps[0].ShadowCameraToLight._14);
	Logger::Log("Shadow render viewproj 2: %f %f %f %f", ShadowMaps[0].ShadowCameraToLight._21, ShadowMaps[0].ShadowCameraToLight._22, ShadowMaps[0].ShadowCameraToLight._23, ShadowMaps[0].ShadowCameraToLight._24);
	Logger::Log("Shadow render viewproj 3: %f %f %f %f", ShadowMaps[0].ShadowCameraToLight._31, ShadowMaps[0].ShadowCameraToLight._32, ShadowMaps[0].ShadowCameraToLight._33, ShadowMaps[0].ShadowCameraToLight._34);
	Logger::Log("Shadow render viewproj 4: %f %f %f %f", ShadowMaps[0].ShadowCameraToLight._41, ShadowMaps[0].ShadowCameraToLight._42, ShadowMaps[0].ShadowCameraToLight._43, ShadowMaps[0].ShadowCameraToLight._44);

	EffectRecord::Render(Device, RenderTarget, RenderedSurface, techniqueIndex, ClearRenderTarget, SourceBuffer);
}
