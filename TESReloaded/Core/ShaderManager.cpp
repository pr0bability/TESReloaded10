#define RESZ_CODE 0x7FA05000
#define BSVertexShaders ((BSShader**)0x011F9548)
#define BSPixelShaders ((BSShader**)0x011F9548)
#define ShadowLightVertexShaders ((NiD3DVertexShader**)0x011FDE5C)
#define ShadowLightPixelShaders ((NiD3DPixelShader**)0x011FDB08)

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

#define pows(a, b)          (pow(abs(a), b) * sgn(a)) // no more pow/abs warning!

/**
* Initializes the Shader Manager Singleton.
* The Shader Manager creates and holds onto the Effects activated in the Settings Manager, and sets the constants.
*/
void ShaderManager::Initialize() {

	auto timer = TimeLogger();

	Logger::Log("Starting the shaders manager...");
	TheShaderManager = new ShaderManager();

	TheShaderManager->FrameVertex = NULL;

	// initializing the list of effect names
	TheShaderManager->EffectsNames["AvgLuma"] = (EffectRecord**)&TheShaderManager->Effects.AvgLuma;
	TheShaderManager->EffectsNames["AmbientOcclusion"] = (EffectRecord**)&TheShaderManager->Effects.AmbientOcclusion;
	TheShaderManager->EffectsNames["BloodLens"] = (EffectRecord**)&TheShaderManager->Effects.BloodLens;
	TheShaderManager->EffectsNames["BloomLegacy"] = (EffectRecord**)&TheShaderManager->Effects.BloomLegacy;
	TheShaderManager->EffectsNames["Coloring"] = (EffectRecord**)&TheShaderManager->Effects.Coloring;
	TheShaderManager->EffectsNames["Cinema"] = (EffectRecord**)&TheShaderManager->Effects.Cinema;
	TheShaderManager->EffectsNames["DepthOfField"] = (EffectRecord**)&TheShaderManager->Effects.DepthOfField;
	TheShaderManager->EffectsNames["Debug"] = (EffectRecord**)&TheShaderManager->Effects.Debug;
	TheShaderManager->EffectsNames["Exposure"] = (EffectRecord**)&TheShaderManager->Effects.Exposure;
	TheShaderManager->EffectsNames["GodRays"] = (EffectRecord**)&TheShaderManager->Effects.GodRays;
	TheShaderManager->EffectsNames["ImageAdjust"] = (EffectRecord**)&TheShaderManager->Effects.ImageAdjust;
	TheShaderManager->EffectsNames["Lens"] = (EffectRecord**)&TheShaderManager->Effects.Lens;
	TheShaderManager->EffectsNames["LowHF"] = (EffectRecord**)&TheShaderManager->Effects.LowHF;
	TheShaderManager->EffectsNames["MotionBlur"] = (EffectRecord**)&TheShaderManager->Effects.MotionBlur;
	TheShaderManager->EffectsNames["Normals"] = (EffectRecord**)&TheShaderManager->Effects.Normals;
	TheShaderManager->EffectsNames["Pretonemapper"] = (EffectRecord**)&TheShaderManager->Effects.Pretonemapper;
	TheShaderManager->EffectsNames["Precipitations"] = (EffectRecord**)&TheShaderManager->Effects.Rain;
	TheShaderManager->EffectsNames["Sharpening"] = (EffectRecord**)&TheShaderManager->Effects.Sharpening;
	TheShaderManager->EffectsNames["ShadowsExteriors"] = (EffectRecord**)&TheShaderManager->Effects.ShadowsExteriors;
	TheShaderManager->EffectsNames["ShadowsInteriors"] = (EffectRecord**)&TheShaderManager->Effects.ShadowsInteriors;
	TheShaderManager->EffectsNames["PointShadows"] = (EffectRecord**)&TheShaderManager->Effects.PointShadows;
	TheShaderManager->EffectsNames["PointShadows2"] = (EffectRecord**)&TheShaderManager->Effects.PointShadows2;
	TheShaderManager->EffectsNames["SunShadows"] = (EffectRecord**)&TheShaderManager->Effects.SunShadows;
	TheShaderManager->EffectsNames["Specular"] = (EffectRecord**)&TheShaderManager->Effects.Specular;
	TheShaderManager->EffectsNames["Snow"] = (EffectRecord**)&TheShaderManager->Effects.Snow;
	TheShaderManager->EffectsNames["SnowAccumulation"] = (EffectRecord**)&TheShaderManager->Effects.SnowAccumulation;
	TheShaderManager->EffectsNames["Underwater"] = (EffectRecord**)&TheShaderManager->Effects.Underwater;
	TheShaderManager->EffectsNames["VolumetricFog"] = (EffectRecord**)&TheShaderManager->Effects.VolumetricFog;
	TheShaderManager->EffectsNames["WaterLens"] = (EffectRecord**)&TheShaderManager->Effects.WaterLens;
	TheShaderManager->EffectsNames["WetWorld"] = (EffectRecord**)&TheShaderManager->Effects.WetWorld;

	TheShaderManager->ShaderNames["Tonemapping"] = &TheShaderManager->Shaders.Tonemapping;
	TheShaderManager->ShaderNames["POM"] = &TheShaderManager->Shaders.POM;
	TheShaderManager->ShaderNames["Water"] = &TheShaderManager->Shaders.Water;
	TheShaderManager->ShaderNames["Sky"] = &TheShaderManager->Shaders.Sky;
	TheShaderManager->ShaderNames["Skin"] = &TheShaderManager->Shaders.Skin;
	TheShaderManager->ShaderNames["Grass"] = &TheShaderManager->Shaders.Grass;
	TheShaderManager->ShaderNames["Terrain"] = &TheShaderManager->Shaders.Terrain;
	TheShaderManager->ShaderNames["ExtraShaders"] = &TheShaderManager->Shaders.ExtraShaders;

	// Initialize all effects
	TheShaderManager->CreateEffects();

	memset(TheShaderManager->WaterVertexShaders, NULL, sizeof(WaterVertexShaders));
	memset(TheShaderManager->WaterPixelShaders, NULL, sizeof(WaterPixelShaders));
	TheShaderManager->InitializeConstants();
	TheShaderManager->CreateFrameVertex(TheRenderManager->width, TheRenderManager->height, &TheShaderManager->FrameVertex);

	TheShaderManager->PreviousCell = nullptr;
	TheShaderManager->IsMenuSwitch = false;

	//setup map of constant names
	TheShaderManager->ConstantsTable["TESR_WorldTransform"] = (D3DXVECTOR4*)&TheRenderManager->worldMatrix;
	TheShaderManager->ConstantsTable["TESR_ViewTransform"] = (D3DXVECTOR4*)&TheRenderManager->viewMatrix;
	TheShaderManager->ConstantsTable["TESR_ProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->projMatrix;
	TheShaderManager->ConstantsTable["TESR_InvProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->InvProjMatrix;
	TheShaderManager->ConstantsTable["TESR_WorldViewProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->WorldViewProjMatrix;
	TheShaderManager->ConstantsTable["TESR_InvViewProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->InvViewProjMatrix;
	TheShaderManager->ConstantsTable["TESR_ViewProjectionTransform"] = (D3DXVECTOR4*)&TheRenderManager->ViewProjMatrix;
	TheShaderManager->ConstantsTable["TESR_ShadowRadius"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowMapRadius;
	TheShaderManager->ConstantsTable["TESR_ShadowCubeMapBlend"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowCubeMapBlend;
	TheShaderManager->ConstantsTable["TESR_ShadowWorldTransform"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowWorld;
	TheShaderManager->ConstantsTable["TESR_ShadowViewProjTransform"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowViewProj;
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransform"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight;
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformNear"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[0];
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformMiddle"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[1];
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformFar"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[2];
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformLod"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[3];
	TheShaderManager->ConstantsTable["TESR_ShadowCameraToLightTransformOrtho"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[4];
	TheShaderManager->ConstantsTable["TESR_OcclusionWorldViewProjTransform"] = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.OcclusionMap.OcclusionWorldViewProj;
	TheShaderManager->ConstantsTable["TESR_ShadowCubeMapLightPosition"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowCubeMapLightPosition;
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition0"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[0];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition1"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[1];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition2"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[2];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition3"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[3];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition4"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[4];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition5"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[5];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition6"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[6];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition7"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[7];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition8"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[8];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition9"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[9];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition10"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[10];
	TheShaderManager->ConstantsTable["TESR_ShadowLightPosition11"] = &TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[11];
	TheShaderManager->ConstantsTable["TESR_LightPosition0"] = &TheShaderManager->LightPosition[0];
	TheShaderManager->ConstantsTable["TESR_LightPosition1"] = &TheShaderManager->LightPosition[1];
	TheShaderManager->ConstantsTable["TESR_LightPosition2"] = &TheShaderManager->LightPosition[2];
	TheShaderManager->ConstantsTable["TESR_LightPosition3"] = &TheShaderManager->LightPosition[3];
	TheShaderManager->ConstantsTable["TESR_LightPosition4"] = &TheShaderManager->LightPosition[4];
	TheShaderManager->ConstantsTable["TESR_LightPosition5"] = &TheShaderManager->LightPosition[5];
	TheShaderManager->ConstantsTable["TESR_LightPosition6"] = &TheShaderManager->LightPosition[6];
	TheShaderManager->ConstantsTable["TESR_LightPosition7"] = &TheShaderManager->LightPosition[7];
	TheShaderManager->ConstantsTable["TESR_LightPosition8"] = &TheShaderManager->LightPosition[8];
	TheShaderManager->ConstantsTable["TESR_LightPosition9"] = &TheShaderManager->LightPosition[9];
	TheShaderManager->ConstantsTable["TESR_LightPosition10"] = &TheShaderManager->LightPosition[10];
	TheShaderManager->ConstantsTable["TESR_LightPosition11"] = &TheShaderManager->LightPosition[11];
	TheShaderManager->ConstantsTable["TESR_ViewSpaceLightDir"] = &TheShaderManager->ShaderConst.ViewSpaceLightDir;
	TheShaderManager->ConstantsTable["TESR_ScreenSpaceLightDir"] = &TheShaderManager->ShaderConst.ScreenSpaceLightDir;
	TheShaderManager->ConstantsTable["TESR_ReciprocalResolution"] = &TheShaderManager->ShaderConst.ReciprocalResolution;
	TheShaderManager->ConstantsTable["TESR_CameraForward"] = &TheRenderManager->CameraForward;
	TheShaderManager->ConstantsTable["TESR_DepthConstants"] = &TheRenderManager->DepthConstants;
	TheShaderManager->ConstantsTable["TESR_CameraData"] = &TheRenderManager->CameraData;
	TheShaderManager->ConstantsTable["TESR_CameraPosition"] = &TheRenderManager->CameraPosition;
	TheShaderManager->ConstantsTable["TESR_SunDirection"] = &TheShaderManager->ShaderConst.SunDir;
	TheShaderManager->ConstantsTable["TESR_SunPosition"] = &TheShaderManager->ShaderConst.SunPosition;
	TheShaderManager->ConstantsTable["TESR_SunTiming"] = &TheShaderManager->ShaderConst.SunTiming;
	TheShaderManager->ConstantsTable["TESR_SunAmount"] = &TheShaderManager->ShaderConst.SunAmount;
	TheShaderManager->ConstantsTable["TESR_GameTime"] = &TheShaderManager->ShaderConst.GameTime;
	TheShaderManager->ConstantsTable["TESR_FogData"] = &TheShaderManager->ShaderConst.fogData;
	TheShaderManager->ConstantsTable["TESR_FogDistance"] = &TheShaderManager->ShaderConst.fogDistance;
	TheShaderManager->ConstantsTable["TESR_FogColor"] = &TheShaderManager->ShaderConst.fogColor;
	TheShaderManager->ConstantsTable["TESR_SunColor"] = &TheShaderManager->ShaderConst.sunColor;
	TheShaderManager->ConstantsTable["TESR_SunAmbient"] = &TheShaderManager->ShaderConst.sunAmbient;
	TheShaderManager->ConstantsTable["TESR_SkyColor"] = &TheShaderManager->ShaderConst.skyColor;
	TheShaderManager->ConstantsTable["TESR_SkyLowColor"] = &TheShaderManager->ShaderConst.skyLowColor;
	TheShaderManager->ConstantsTable["TESR_HorizonColor"] = &TheShaderManager->ShaderConst.horizonColor;
	TheShaderManager->ConstantsTable["TESR_WaterCoefficients"] = &TheShaderManager->ShaderConst.Water.waterCoefficients;
	TheShaderManager->ConstantsTable["TESR_WaveParams"] = &TheShaderManager->ShaderConst.Water.waveParams;
	TheShaderManager->ConstantsTable["TESR_WaterVolume"] = &TheShaderManager->ShaderConst.Water.waterVolume;
	TheShaderManager->ConstantsTable["TESR_WaterSettings"] = &TheShaderManager->ShaderConst.Water.waterSettings;
	TheShaderManager->ConstantsTable["TESR_WaterDeepColor"] = &TheShaderManager->ShaderConst.Water.deepColor;
	TheShaderManager->ConstantsTable["TESR_WaterShallowColor"] = &TheShaderManager->ShaderConst.Water.shallowColor;
	TheShaderManager->ConstantsTable["TESR_WaterShorelineParams"] = &TheShaderManager->ShaderConst.Water.shorelineParams;
	TheShaderManager->ConstantsTable["TESR_ParallaxData"] = &TheShaderManager->ShaderConst.POM.ParallaxData;
	TheShaderManager->ConstantsTable["TESR_GrassScale"] = &TheShaderManager->ShaderConst.Grass.Scale;
	TheShaderManager->ConstantsTable["TESR_TerrainData"] = &TheShaderManager->ShaderConst.Terrain.Data;
	TheShaderManager->ConstantsTable["TESR_SkinData"] = &TheShaderManager->ShaderConst.Skin.SkinData;
	TheShaderManager->ConstantsTable["TESR_SkinColor"] = &TheShaderManager->ShaderConst.Skin.SkinColor;
	TheShaderManager->ConstantsTable["TESR_SkyData"] = &TheShaderManager->ShaderConst.Sky.SkyData;
	TheShaderManager->ConstantsTable["TESR_CloudData"] = &TheShaderManager->ShaderConst.Sky.CloudData;
	TheShaderManager->ConstantsTable["TESR_SunsetColor"] = &TheShaderManager->ShaderConst.Sky.SunsetColor;
	TheShaderManager->ConstantsTable["TESR_HDRBloomData"] = &TheShaderManager->ShaderConst.HDR.BloomData;
	TheShaderManager->ConstantsTable["TESR_HDRData"] = &TheShaderManager->ShaderConst.HDR.HDRData;
	TheShaderManager->ConstantsTable["TESR_LotteData"] = &TheShaderManager->ShaderConst.HDR.LotteData;
	TheShaderManager->ConstantsTable["TESR_ToneMapping"] = &TheShaderManager->ShaderConst.HDR.ToneMapping;
	TheShaderManager->ConstantsTable["TESR_DebugVar"] = &TheShaderManager->ShaderConst.DebugVar;

	// load actual effect files and initialize constant tables
	TheShaderManager->LoadEffects();

	timer.LogTime("ShaderManager::Initialize");
}

void ShaderManager::CreateFrameVertex(UInt32 Width, UInt32 Height, IDirect3DVertexBuffer9** FrameVertex) {
	
	void* VertexData = NULL;
	float OffsetX = (1.0f / (float)Width) * 0.5f;
	float OffsetY = (1.0f / (float)Height) * 0.5f;
	
	FrameVS FrameVertices[] = {
		{ -1.0f,  1.0f, 1.0f, 0.0f + OffsetX, 0.0f + OffsetY },
		{ -1.0f, -1.0f, 1.0f, 0.0f + OffsetX, 1.0f + OffsetY },
		{  1.0f,  1.0f, 1.0f, 1.0f + OffsetX, 0.0f + OffsetY },
		{  1.0f, -1.0f, 1.0f, 1.0f + OffsetX, 1.0f + OffsetY }
	};
	TheRenderManager->device->CreateVertexBuffer(4 * sizeof(FrameVS), D3DUSAGE_WRITEONLY, FrameFVF, D3DPOOL_DEFAULT, FrameVertex, NULL);
	(*FrameVertex)->Lock(0, 0, &VertexData, NULL);
	memcpy(VertexData, FrameVertices, sizeof(FrameVertices));
	(*FrameVertex)->Unlock();

}

/*
* Initializes the Effect Record for each effect activated in the settings.
*/
void ShaderManager::CreateEffects() {

	auto timer = TimeLogger();

	// create effect records for effects shaders based on name
	for (EffectsList::iterator v = TheShaderManager->EffectsNames.begin(); v != TheShaderManager->EffectsNames.end(); v++) {
		EffectRecord* Effect = CreateEffect(v->first.c_str());
		Effect->RegisterConstants();
		*v->second = Effect;
	}

	for (ShaderList::iterator v = TheShaderManager->ShaderNames.begin(); v != TheShaderManager->ShaderNames.end(); v++) {
		ShaderCollection* Collection = CreateCollection(v->first.c_str());
		*v->second = Collection;
	}

	/*TODO*/
	//CreateEffect(EffectRecord::EffectRecordType::Extra);
	//if (EffectsSettings->Extra) CreateEffect(EffectRecord::EffectRecordType::Extra);

	timer.LogTime("ShaderManager::CreateEffects");
}

void ShaderManager::LoadEffects() {

	auto timer = TimeLogger();

	// load effects from the effects list
	for (const auto [Name, effect] : TheShaderManager->EffectsNames) {
		(*effect)->LoadEffect();
	}

	/*TODO*/
	//CreateEffect(EffectRecord::EffectRecordType::Extra);
	//if (EffectsSettings->Extra) CreateEffect(EffectRecord::EffectRecordType::Extra);

	timer.LogTime("ShaderManager::LoadEffects");
}



void ShaderManager::InitializeConstants() {

	ShaderConst.pWeather = NULL;

	ShaderConst.ReciprocalResolution.x = 1.0f / (float)TheRenderManager->width;
	ShaderConst.ReciprocalResolution.y = 1.0f / (float)TheRenderManager->height;
	ShaderConst.ReciprocalResolution.z = (float)TheRenderManager->width / (float)TheRenderManager->height;
	ShaderConst.ReciprocalResolution.w = 0.0f; // Reserved to store the FoV
}


/*
Updates the values of the constants that can be accessed from shader code, with values representing the state of the game's elements.
*/
void ShaderManager::UpdateConstants() {
	
	auto timer = TimeLogger();

	bool IsThirdPersonView = !TheCameraManager->IsFirstPerson();
	Sky* WorldSky = Tes->sky;
	NiNode* SunRoot = WorldSky->sun->RootNode;
	TESClimate* currentClimate = WorldSky->firstClimate;
	TESWeather* currentWeather = WorldSky->firstWeather;
	TESWeather* previousWeather = WorldSky->secondWeather;
	TESObjectCELL* currentCell = Player->parentCell;
	TESWorldSpace* currentWorldSpace = Player->GetWorldSpace();
	TESRegion* currentRegion = Player->GetRegion();
	float weatherPercent = WorldSky->weatherPercent;
	float lastGameTime = ShaderConst.GameTime.y;
	const char* sectionName = NULL;
	avglumaRequired = false; // toggle for rendering AvgLuma
	orthoRequired = false; // toggle for rendering Ortho map

	if (Effects.Debug->Enabled) avglumaRequired = true;

	// context variables
	PipBoyIsOn = InterfaceManager->getIsMenuOpen();
	VATSIsOn = InterfaceManager->IsActive(Menu::kMenuType_VATS);
	OverlayIsOn = InterfaceManager->IsActive(Menu::kMenuType_Computers) ||
		InterfaceManager->IsActive(Menu::kMenuType_LockPick) ||
		InterfaceManager->IsActive(Menu::kMenuType_Surgery) ||
		InterfaceManager->IsActive(Menu::kMenuType_SlotMachine) ||
		InterfaceManager->IsActive(Menu::kMenuType_Blackjack) ||
		InterfaceManager->IsActive(Menu::kMenuType_Roulette) ||
		InterfaceManager->IsActive(Menu::kMenuType_Caravan);
	isDialog = InterfaceManager->IsActive(Menu::MenuType::kMenuType_Dialog);
	isPersuasion = InterfaceManager->IsActive(Menu::MenuType::kMenuType_Persuasion);

	if (!currentCell) return; // if no cell is present, we skip update to avoid trying to access values that don't exist
	isExterior = !currentCell->IsInterior();// || Player->parentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior; // < use exterior flag, broken for now
	isCellChanged = currentCell != PreviousCell;
	PreviousCell = currentCell;
	if (isCellChanged) TheSettingManager->SettingsChanged = true; // force update constants during cell transition

	isUnderwater = Tes->sky->GetIsUnderWater();
	isRainy = currentWeather?currentWeather->GetWeatherType() == TESWeather::WeatherType::kType_Rainy : false;
	isSnow = currentWeather?currentWeather->GetWeatherType() == TESWeather::WeatherType::kType_Snow : false;

	TimeGlobals* GameTimeGlobals = TimeGlobals::Get();
	float GameHour = fmod(GameTimeGlobals->GameHour->data, 24); // make sure the hours values are less than 24

	float SunriseStart = WorldSky->GetSunriseBegin();
	float SunriseEnd = WorldSky->GetSunriseEnd();
	float SunsetStart = WorldSky->GetSunsetBegin();
	float SunsetEnd = WorldSky->GetSunsetEnd();

	// calculating sun amount for shaders (currently not used by any shaders)
	float sunRise = step(SunriseStart, SunriseEnd, GameHour); // 0 at night to 1 after sunrise
	float sunSet = step(SunsetEnd, SunsetStart, GameHour);  // 1 before sunset to 0 at night
	isDayTime = sunRise * sunSet;

	ShaderConst.SunTiming.x = WorldSky->GetSunriseColorBegin();
	ShaderConst.SunTiming.y = SunriseEnd;
	ShaderConst.SunTiming.z = SunsetStart;
	ShaderConst.SunTiming.w = WorldSky->GetSunsetColorEnd();

	// fake sunset time tracking with more time given before sunrise/sunset
	float sunRiseLight = step(SunriseStart - 1.0f, SunriseEnd, GameHour); // 0 at night to 1 after sunrise
	float sunSetLight = step(SunsetEnd + 1.0f, SunsetStart, GameHour);  // 1 before sunset to 0 at night
	float newDayLight = sunRiseLight * sunSetLight;
	float transitionCurve = smoothStep(0.0f, 0.6f, newDayLight); // a curve for day/night transitions that occurs mostly during second half of sunset

	isDayTimeChanged = true;  // will fire settings update during sunset/sunrise transitions
	if (newDayLight == dayLight) isDayTimeChanged = false;
	dayLight = newDayLight;

	ShaderConst.GameTime.x = TimeGlobals::GetGameTime(); //time in milliseconds
	ShaderConst.GameTime.y = GameHour; //time in hours
	ShaderConst.GameTime.z = (float)TheFrameRateManager->Time;
	ShaderConst.GameTime.w = TheFrameRateManager->ElapsedTime; // frameTime in seconds

	// get water height based on player position
	ShaderConst.Water.waterSettings.x = Tes->GetWaterHeight(Player, WorldSceneGraph);
	ShaderConst.Water.waterSettings.z = isUnderwater;

	ShaderConst.SunPosition = SunRoot->m_localTransform.pos.toD3DXVEC4();
	D3DXVec4Normalize(&ShaderConst.SunPosition, &ShaderConst.SunPosition);
	ShaderConst.SunDir = Tes->directionalLight->direction.toD3DXVEC4() * -1.0f;

	// during the day, track the sun mesh position instead of the lighting direction in exteriors
	if (isExterior && isDayTime) ShaderConst.SunDir = ShaderConst.SunPosition;

	// expose the light vector in view space for screen space lighting
	D3DXVec4Transform(&ShaderConst.ScreenSpaceLightDir, &ShaderConst.SunDir, &TheRenderManager->ViewProjMatrix);
	D3DXVec4Normalize(&ShaderConst.ScreenSpaceLightDir, &ShaderConst.ScreenSpaceLightDir);

	D3DXVec4Transform(&ShaderConst.ViewSpaceLightDir, &ShaderConst.SunDir, &TheRenderManager->ViewMatrix);
	D3DXVec4Normalize(&ShaderConst.ViewSpaceLightDir, &ShaderConst.ViewSpaceLightDir);

	ShaderConst.sunGlare = currentWeather ? (currentWeather->GetSunGlare() / 255.0f) : 0.5f;

	isDayTime = smoothStep(0, 1, isDayTime); // smooth daytime progression
	ShaderConst.SunAmount.x = isDayTime; 
	ShaderConst.SunAmount.y = ShaderConst.sunGlare;
	if (TheSettingManager->SettingsChanged) {

		// update settings
		for (const auto [Name, effect] : EffectsNames) {
			(*effect)->UpdateSettings();
		}

		// sky settings are used in several shaders whether the shader is active or not
		ShaderConst.SunAmount.z = TheSettingManager->GetSettingI("Shaders.Sky.Main", "ReplaceSun");
		ShaderConst.SunAmount.w = TheSettingManager->GetSettingF("Shaders.Sky.Main", "GlareStrength");

		ShaderConst.Sky.SkyData.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "AthmosphereThickness");
		ShaderConst.Sky.SkyData.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunInfluence");
		ShaderConst.Sky.SkyData.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunStrength");
		ShaderConst.Sky.SkyData.w = TheSettingManager->GetSettingF("Shaders.Sky.Main", "StarStrength");

		ShaderConst.Sky.CloudData.x = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "UseNormals");
		ShaderConst.Sky.CloudData.y = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "SphericalNormals");
		ShaderConst.Sky.CloudData.z = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Transparency");
		ShaderConst.Sky.CloudData.w = TheSettingManager->GetSettingF("Shaders.Sky.Clouds", "Brightness");

		// only add sunset color boost in exteriors
		if (isExterior) {
			ShaderConst.Sky.SunsetColor.x = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetR");
			ShaderConst.Sky.SunsetColor.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetG");
			ShaderConst.Sky.SunsetColor.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunsetB");

			// TODO : fix sun culling for sun replacing?
			//if (TheSettingManager->GetMenuShaderEnabled("Sky")) {
			//	if (ShaderConst.SunAmount.z) WorldSky->sun->RootNode->m_flags |= ~NiAVObject::NiFlags::DISPLAY_OBJECT; // cull Sun node
			//	else WorldSky->sun->RootNode->m_flags &= NiAVObject::NiFlags::DISPLAY_OBJECT; // disable Sun node
			//}
		}
		else {
			ShaderConst.Sky.SunsetColor.x = 0;
			ShaderConst.Sky.SunsetColor.y = 0;
			ShaderConst.Sky.SunsetColor.z = 0;
		}
	}

	ShaderConst.sunColor.x = WorldSky->sunDirectional.r;
	ShaderConst.sunColor.y = WorldSky->sunDirectional.g;
	ShaderConst.sunColor.z = WorldSky->sunDirectional.b;
	ShaderConst.sunColor.w = ShaderConst.sunGlare;

	ShaderConst.windSpeed = WorldSky->windSpeed;

	ShaderConst.fogColor.x = WorldSky->fogColor.r;
	ShaderConst.fogColor.y = WorldSky->fogColor.g;
	ShaderConst.fogColor.z = WorldSky->fogColor.b;
	ShaderConst.fogColor.w = 1.0f;

	ShaderConst.horizonColor.x = WorldSky->Horizon.r;
	ShaderConst.horizonColor.y = WorldSky->Horizon.g;
	ShaderConst.horizonColor.z = WorldSky->Horizon.b;
	ShaderConst.horizonColor.w = 1.0f;

	ShaderConst.sunAmbient.x = WorldSky->sunAmbient.r;
	ShaderConst.sunAmbient.y = WorldSky->sunAmbient.g;
	ShaderConst.sunAmbient.z = WorldSky->sunAmbient.b;
	ShaderConst.sunAmbient.w = 1.0f;

	ShaderConst.skyLowColor.x = WorldSky->SkyLower.r;
	ShaderConst.skyLowColor.y = WorldSky->SkyLower.g;
	ShaderConst.skyLowColor.z = WorldSky->SkyLower.b;
	ShaderConst.skyLowColor.w = 1.0f;

	ShaderConst.skyColor.x = WorldSky->skyUpper.r;
	ShaderConst.skyColor.y = WorldSky->skyUpper.g;
	ShaderConst.skyColor.z = WorldSky->skyUpper.b;
	ShaderConst.skyColor.w = 1.0f;

	// for near plane, ensure that far > near
	ShaderConst.fogData.x = WorldSky->fogFarPlane > WorldSky->fogNearPlane ? WorldSky->fogNearPlane : WorldSky->fogFarPlane * 0.7;
	ShaderConst.fogData.y = WorldSky->fogFarPlane;
	ShaderConst.fogData.z = ShaderConst.sunGlare;
	ShaderConst.fogData.w = WorldSky->fogPower;

	ShaderConst.fogDistance.x = ShaderConst.fogData.x;
	ShaderConst.fogDistance.y = ShaderConst.fogData.y;
	ShaderConst.fogDistance.z = 1.0f;
	ShaderConst.fogDistance.w = ShaderConst.sunGlare;

	//if (weatherPercent == 1.0f) ShaderConst.pWeather = currentWeather;

	if (TheSettingManager->GetMenuShaderEnabled("Water") || Effects.Underwater->Enabled) {
		RGBA* rgba = NULL;
		SettingsWaterStruct* sws = NULL;
		TESWaterForm* currentWater = currentCell->GetWaterForm();
			
		sectionName = "Shaders.Water.Default";
		if (currentWater) {
			UInt32 WaterType = currentWater->GetWaterType();
			if (WaterType == TESWaterForm::WaterType::kWaterType_Blood)
				sectionName = "Shaders.Water.Blood";
			else if (WaterType == TESWaterForm::WaterType::kWaterType_Lava)
				sectionName = "Shaders.Water.Lava";

			// world space specific settings. TODO Reimplement with Toml
			//else if (!(sws = TheSettingManager->GetSettingsWater(currentCell->GetEditorName())) && currentWorldSpace)
			//	sws = TheSettingManager->GetSettingsWater(currentWorldSpace->GetEditorName());

			rgba = currentWater->GetDeepColor();
			ShaderConst.Water.deepColor.x = rgba->r / 255.0f;
			ShaderConst.Water.deepColor.y = rgba->g / 255.0f;
			ShaderConst.Water.deepColor.z = rgba->b / 255.0f;
			ShaderConst.Water.deepColor.w = rgba->a / 255.0f;
			rgba = currentWater->GetShallowColor();
			ShaderConst.Water.shallowColor.x = rgba->r / 255.0f;
			ShaderConst.Water.shallowColor.y = rgba->g / 255.0f;
			ShaderConst.Water.shallowColor.z = rgba->b / 255.0f;
			ShaderConst.Water.shallowColor.w = rgba->a / 255.0f;
		}

		if (TheSettingManager->SettingsChanged) {
			ShaderConst.Water.waterCoefficients.x = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_R");
			ShaderConst.Water.waterCoefficients.y = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_G");
			ShaderConst.Water.waterCoefficients.z = TheSettingManager->GetSettingF(sectionName, "inExtCoeff_B");
			ShaderConst.Water.waterCoefficients.w = TheSettingManager->GetSettingF(sectionName, "inScattCoeff");

			ShaderConst.Water.waveParams.x = TheSettingManager->GetSettingF(sectionName, "choppiness");
			ShaderConst.Water.waveParams.y = TheSettingManager->GetSettingF(sectionName, "waveWidth");
			ShaderConst.Water.waveParams.z = TheSettingManager->GetSettingF(sectionName, "waveSpeed");
			ShaderConst.Water.waveParams.w = TheSettingManager->GetSettingF(sectionName, "reflectivity");

			ShaderConst.Water.waterSettings.y = TheSettingManager->GetSettingF(sectionName, "depthDarkness");

			ShaderConst.Water.waterVolume.x = TheSettingManager->GetSettingF(sectionName, "causticsStrength") * ShaderConst.sunGlare;
			ShaderConst.Water.waterVolume.y = TheSettingManager->GetSettingF(sectionName, "shoreFactor");
			ShaderConst.Water.waterVolume.z = TheSettingManager->GetSettingF(sectionName, "turbidity");
			ShaderConst.Water.waterVolume.w = TheSettingManager->GetSettingF(sectionName, "causticsStrengthS");

			ShaderConst.Water.shorelineParams.x = TheSettingManager->GetSettingF(sectionName, "shoreMovement");
		}
	}

	if (TheSettingManager->SettingsChanged) {
		// Static constants that will only change when settings are edited

		if (TheSettingManager->GetMenuShaderEnabled("Grass")) {
			ShaderConst.Grass.Scale.x = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleX");
			ShaderConst.Grass.Scale.y = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleY");
			ShaderConst.Grass.Scale.z = TheSettingManager->GetSettingF("Shaders.Grass.Main", "ScaleZ");

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
				*Pointers::Settings::GrassWindMagnitudeMax = *Pointers::ShaderParams::GrassWindMagnitudeMax = TheSettingManager->GetSettingF("Shaders.Grass.Main", "WindCoefficient") * ShaderConst.windSpeed;
				*Pointers::Settings::GrassWindMagnitudeMin = *Pointers::ShaderParams::GrassWindMagnitudeMin = *Pointers::Settings::GrassWindMagnitudeMax * 0.5f;
			}
		}

		if (TheSettingManager->GetMenuShaderEnabled("POM")) {
			ShaderConst.POM.ParallaxData.x = TheSettingManager->GetSettingF("Shaders.POM.Main", "HeightMapScale");
			ShaderConst.POM.ParallaxData.y = TheSettingManager->GetSettingF("Shaders.POM.Main", "MinSamples");
			ShaderConst.POM.ParallaxData.z = TheSettingManager->GetSettingF("Shaders.POM.Main", "MaxSamples");
		}

		if (TheSettingManager->GetMenuShaderEnabled("Terrain")) {
			ShaderConst.Terrain.Data.x = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "DistantSpecular");
			ShaderConst.Terrain.Data.y = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "DistantNoise");
			ShaderConst.Terrain.Data.z = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "NearSpecular");
			ShaderConst.Terrain.Data.w = TheSettingManager->GetSettingF("Shaders.Terrain.Main", "MiddleSpecular");
		}

		if (TheSettingManager->GetMenuShaderEnabled("Skin")) {
			ShaderConst.Skin.SkinData.x = TheSettingManager->GetSettingF("Shaders.Skin.Main", "Attenuation");
			ShaderConst.Skin.SkinData.y = TheSettingManager->GetSettingF("Shaders.Skin.Main", "SpecularPower");
			ShaderConst.Skin.SkinData.z = TheSettingManager->GetSettingF("Shaders.Skin.Main", "MaterialThickness");
			ShaderConst.Skin.SkinData.w = TheSettingManager->GetSettingF("Shaders.Skin.Main", "RimScalar");
			ShaderConst.Skin.SkinColor.x = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffRed");
			ShaderConst.Skin.SkinColor.y = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffGreen");
			ShaderConst.Skin.SkinColor.z = TheSettingManager->GetSettingF("Shaders.Skin.Main", "CoeffBlue");
		}

		ShaderConst.DebugVar.x = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar1");
		ShaderConst.DebugVar.y = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar2");
		ShaderConst.DebugVar.z = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar3");
		ShaderConst.DebugVar.w = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar4");
	}

	if (TheSettingManager->SettingsChanged || isDayTimeChanged) {
		if (TheSettingManager->GetMenuShaderEnabled("Tonemapping") || TheSettingManager->GetMenuShaderEnabled("PreTonemapper")) {
			ShaderConst.HDR.PointLightMult = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "PointLightMultiplier", isExterior, transitionCurve);
			ShaderConst.HDR.ToneMapping.x = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "HighlightSaturation", isExterior, transitionCurve);
			ShaderConst.HDR.ToneMapping.y = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "WeatherContrast", isExterior, transitionCurve);
			ShaderConst.HDR.ToneMapping.z = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "ToneMappingColor", isExterior, transitionCurve);
			ShaderConst.HDR.ToneMapping.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "Linearization", isExterior, transitionCurve);
			ShaderConst.HDR.BloomData.x = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "BloomStrength", isExterior, transitionCurve);
			ShaderConst.HDR.BloomData.y = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "BloomExponent", isExterior, transitionCurve);
			ShaderConst.HDR.BloomData.z = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "WeatherModifier", isExterior, transitionCurve);
			ShaderConst.HDR.BloomData.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapWhitePoint", isExterior, transitionCurve);
			ShaderConst.HDR.HDRData.x = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemappingMode", isExterior, transitionCurve);
			ShaderConst.HDR.HDRData.y = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "Exposure", isExterior, transitionCurve);
			ShaderConst.HDR.HDRData.z = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "Saturation", isExterior, transitionCurve);
			ShaderConst.HDR.HDRData.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "Gamma", isExterior, transitionCurve);
			ShaderConst.Sky.SunsetColor.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "SkyMultiplier", isExterior, transitionCurve);

			ShaderConst.HDR.LotteData.x = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapContrast", isExterior, transitionCurve);
			ShaderConst.HDR.LotteData.y = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapBrightness", isExterior, transitionCurve);
			ShaderConst.HDR.LotteData.z = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapMidpoint", isExterior, transitionCurve);
			ShaderConst.HDR.LotteData.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapShoulder", isExterior, transitionCurve);

			if (ShaderConst.HDR.HDRData.x == 1.0 || TheSettingManager->GetMenuShaderEnabled("Pretonemapper")) {
				float hdrMax = max(1.0, ShaderConst.HDR.BloomData.w * 100.0);
				float contrast = max(0.01, ShaderConst.HDR.LotteData.x * 1.35);
				float shoulder = max(0.0, (min(1.0, ShaderConst.HDR.LotteData.w * 0.993))); // Shoulder should not! exceed 1.0
				float midIn = max(0.01, ShaderConst.HDR.LotteData.z * 0.18);
				float midOut = max(0.01, (ShaderConst.HDR.LotteData.y * 0.18) / shoulder);
				float colToneB = -((-pows(midIn, contrast) + (midOut * (pows(hdrMax, contrast * shoulder) * pows(midIn, contrast) -
					pows(hdrMax, contrast) * pows(midIn, contrast * shoulder) * midOut)) /
					(pows(hdrMax, contrast * shoulder) * midOut - pows(midIn, contrast * shoulder) * midOut)) /
					(pows(midIn, contrast * shoulder) * midOut));

				float colToneC = (pows(hdrMax, contrast * shoulder) * pows(midIn, contrast) - pows(hdrMax, contrast) * pows(midIn, contrast * shoulder) * midOut) /
					(pows(hdrMax, contrast * shoulder) * midOut - pows(midIn, contrast * shoulder) * midOut);

				ShaderConst.HDR.LotteData.x = contrast;
				ShaderConst.HDR.LotteData.w = shoulder;
				ShaderConst.HDR.LotteData.z = colToneB;
				ShaderConst.HDR.LotteData.y = colToneC;
			}
		}
		else {

			ShaderConst.Sky.SunsetColor.w = 1.0; // set sky multiplier to 1 if HDR disabled as it is used by the Sky shaders
			ShaderConst.HDR.PointLightMult = 1.0;
		}

	}

	// update Constants
	for (const auto [Name, effect] : EffectsNames) {
		if ((*effect)->Enabled) (*effect)->UpdateConstants();
	}

	TheSettingManager->SettingsChanged = false;
	timer.LogTime("ShaderManager::UpdateConstants");
}


int ShaderManager::GetVertexShaders(const char* Name, NiD3DVertexShader*** Shader) {

	//BSShader** Shaders = (BSShader**)0x011F9548;
	//NiD3DVertexShader** ShadowLightVertexShaders = (NiD3DVertexShader**)0x011FDE5C;
	int Size = 0;

	if (!strcmp(Name, "Water")) {
		*Shader = ((WaterShader*)BSVertexShaders[kShaderDefinition_WaterShader])->pVertexShaders;
		Size = sizeof(*Shader) / 4;
	}
	else if (!strcmp(Name, "WaterHeightMap")) {
		*Shader = WaterVertexShaders;
		Size = sizeof(WaterVertexShaders) / 4;
	}
	else if (!strcmp(Name, "Terrain") || !strcmp(Name, "ExtraShaders")) {
		*Shader = ShadowLightVertexShaders;
		Size = 103;
	}
	else if (!strcmp(Name, "POM")) {
		*Shader = ((ParallaxShader*)BSVertexShaders[kShaderDefinition_ParallaxShader])->pVertexShaders;
		Size = sizeof(*Shader) / 4;
	}

	return Size;
}

int ShaderManager::GetPixelShaders(const char* Name, NiD3DPixelShader*** Shader) {

	//BSShader** Shaders = (BSShader**)0x011F9548;
	//NiD3DPixelShader** ShadowLightPixelShaders = (NiD3DPixelShader**)0x011FDB08;
	int Size = 0;

	if (!strcmp(Name, "Water")) {
		*Shader = ((WaterShader*)BSPixelShaders[kShaderDefinition_WaterShader])->pPixelShaders;
		Size = sizeof(*Shader) / 4;
	}
	else if (!strcmp(Name, "WaterHeightMap")) {
		*Shader = WaterPixelShaders;
		Size = sizeof(WaterPixelShaders) / 4;
	}
	else if (!strcmp(Name, "Terrain") || !strcmp(Name, "ExtraShaders")) {
		*Shader = ShadowLightPixelShaders;
		Size = 160;
	}
	else if (!strcmp(Name, "POM")) {
		*Shader = ((ParallaxShader*)BSPixelShaders[kShaderDefinition_ParallaxShader])->pPixelShaders;
		Size = sizeof(*Shader) / 4;
	}
	return Size;

}


bool ShaderManager::CreateShader(const char* Name) {
	

	NiD3DVertexShader** VertexShaderList = NULL;
	NiD3DPixelShader** PixelShaderList = NULL;
	int WaterVertexShadersSize = sizeof(WaterVertexShaders) / 4;
	int WaterPixelShadersSize = sizeof(WaterPixelShaders) / 4;
	int Size = 0;
	bool success = true;

	if (!strcmp(Name, "Terrain") || !strcmp(Name, "ExtraShaders")) {
		VertexShaderList = ShadowLightVertexShaders;
		Size = 103;
		for (int i = 0; i < Size; i++)
			if (VertexShaderList[i] && strstr(TerrainShaders, ((NiD3DVertexShaderEx*)VertexShaderList[i])->ShaderName))
				success = LoadShader(VertexShaderList[i]) && success;
		
		PixelShaderList = ShadowLightPixelShaders;
		Size = 160;
		for (int i = 0; i < Size; i++)
			if (PixelShaderList[i] && strstr(TerrainShaders, ((NiD3DPixelShaderEx*)PixelShaderList[i])->ShaderName))
				success = LoadShader(PixelShaderList[i]) && success;
	}
	else if (!strcmp(Name, "Water")) {
		VertexShaderList = ((WaterShader*)BSVertexShaders[kShaderDefinition_WaterShader])->pVertexShaders;
		Size = sizeof(PixelShaderList) / 4;
		for (int i = 0; i < Size; i++)
			if (VertexShaderList[i]) success = LoadShader(VertexShaderList[i]) && success;
		
		PixelShaderList = ((WaterShader*)BSPixelShaders[kShaderDefinition_WaterShader])->pPixelShaders;
		Size = sizeof(PixelShaderList) / 4;
		for (int i = 0; i < Size; i++)
			if (PixelShaderList[i]) success = LoadShader(PixelShaderList[i]) && success;

		// Water Height map
		VertexShaderList = WaterVertexShaders;
		Size = sizeof(WaterVertexShaders) / 4;
		for (int i = 0; i < Size; i++)
			if (VertexShaderList[i]) success = LoadShader(VertexShaderList[i]) && success;

		PixelShaderList = WaterPixelShaders;
		Size = sizeof(WaterPixelShaders) / 4;
		for (int i = 0; i < Size; i++) if (PixelShaderList[i])
			success = LoadShader(PixelShaderList[i]) && success;
	}

	if (!success) {
		char Message[256] = "Error: Could not load shader ";
		strcat(Message, Name);
		InterfaceManager->ShowMessage(Message);
		Logger::Log(Message);
	}

	return success;
}


ShaderCollection* ShaderManager::GetShaderCollection(const char* Name) {

	if (!memcmp(Name, "WATER", 5)) return Shaders.Water;
	if (!memcmp(Name, "GRASS", 5)) return Shaders.Grass;
	if (!memcmp(Name, "ISHDR", 5) || !memcmp(Name, "HDR", 3)) return Shaders.Tonemapping; // tonemapping shaders have different names between New vegas and Oblivion
	if (!memcmp(Name, "PAR", 3)) return Shaders.POM;
	if (!memcmp(Name, "SKIN", 4)) return Shaders.Skin;
	if (!memcmp(Name, "SKY", 3)) return Shaders.Sky;
	if (strstr(TerrainShaders, Name)) return Shaders.Terrain;
	if (strstr(BloodShaders, Name)) return Shaders.Blood;

	return Shaders.ExtraShaders;
}


/*
* Load generic Vertex Shaders as well as the ones for interiors and exteriors if the exist. Returns false if generic one isn't found (as other ones are optional)
*/
bool ShaderManager::LoadShader(NiD3DVertexShader* Shader) {
	
	NiD3DVertexShaderEx* VertexShader = (NiD3DVertexShaderEx*)Shader;
	ShaderCollection* Collection = GetShaderCollection(VertexShader->ShaderName);

	// if AMD and no DXVK, disable glitchy Tonemapping shader replacements
	if (Collection == Shaders.Tonemapping && (TheRenderManager->RESZ && !TheRenderManager->DXVK)) return false;

	if (!Collection->Enabled) return false;

	// Load generic, interior and exterior shaders
	VertexShader->ShaderProg  = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, NULL);
	VertexShader->ShaderProgE = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, "Exteriors\\");
	VertexShader->ShaderProgI = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, "Interiors\\");

	if (VertexShader->ShaderProg != nullptr || VertexShader->ShaderProgE != nullptr || VertexShader->ShaderProgI != nullptr) {
		Collection->VertexShaderList.push_back(VertexShader);
		Logger::Log("Loaded %s Vertex Shader %s", Collection->Name, VertexShader->ShaderName);
	}

	return Collection->Enabled;
}


/*
* Load generic Pixel Shaders as well as the ones for interiors and exteriors if the exist. Returns false if generic one isn't found (as other ones are optional)
*/
bool ShaderManager::LoadShader(NiD3DPixelShader* Shader) {

	NiD3DPixelShaderEx* PixelShader = (NiD3DPixelShaderEx*)Shader;
	ShaderCollection* Collection = GetShaderCollection(PixelShader->ShaderName);

	// if AMD and no DXVK, disable glitchy Tonemapping shader replacements
	if (Collection == Shaders.Tonemapping && (TheRenderManager->RESZ && !TheRenderManager->DXVK)) return false;

	if (!Collection->Enabled) return false;

	PixelShader->ShaderProg  = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, NULL);
	PixelShader->ShaderProgE = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, "Exteriors\\");
	PixelShader->ShaderProgI = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, "Interiors\\");

	if (PixelShader->ShaderProg != nullptr || PixelShader->ShaderProgE != nullptr || PixelShader->ShaderProgI != nullptr) {
		Collection->PixelShaderList.push_back(PixelShader);
		Logger::Log("Loaded %s Pixel Shader %s", Collection->Name, PixelShader->ShaderName);
	}

	return Collection->Enabled;
}


void ShaderManager::DisposeShader(const char* Name) {

	NiD3DVertexShader** Vertex = NULL;
	NiD3DPixelShader** Pixel = NULL;
	int WaterVertexShadersSize = sizeof(WaterVertexShaders) / 4;
	int WaterPixelShadersSize = sizeof(WaterPixelShaders) / 4;
	int Upperbound = 0;

	if (!strcmp(Name, "Terrain")) {
		Upperbound = GetVertexShaders(Name, &Vertex);
		for (int i = 0; i < Upperbound; i++)
			if (Vertex[i] && strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) 
				((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();

		Upperbound = GetPixelShaders(Name, &Pixel);
		for (int i = 0; i < Upperbound; i++)
			if (Pixel[i] && strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) 
				((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
	}
	else if (!strcmp(Name, "ExtraShaders")) {
		Upperbound = GetVertexShaders(Name, &Vertex);
		for (int i = 0; i < Upperbound; i++)
			if (Vertex[i] && !strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) 
				((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();

		Upperbound = GetPixelShaders(Name, &Pixel);
		for (int i = 0; i < Upperbound; i++)
			if (Pixel[i] && !strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) 
				((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
	}
	else {
		Upperbound = GetVertexShaders(Name, &Vertex);
		for (int i = 0; i < Upperbound; i++)
			if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();

		Upperbound = GetPixelShaders(Name, &Pixel);
		for (int i = 0; i < Upperbound; i++)
			if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();

		if (!strcmp(Name, "Water")) {
			Upperbound = GetVertexShaders("WaterHeightMap", &Vertex);
			for (int i = 0; i < Upperbound; i++)
				if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
			
			Upperbound = GetPixelShaders("WaterHeightMap", &Pixel);
			for (int i = 0; i < Upperbound; i++)
				if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
			
			Upperbound = GetVertexShaders("WaterDisplacement", &Vertex);
			for (int i = 0; i < Upperbound; i++)
				if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
			
			Upperbound = GetPixelShaders("WaterDisplacement", &Pixel);
			for (int i = 0; i < Upperbound; i++)
				if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
		}
	}

}


void ShaderManager::GetNearbyLights(NiPointLight* ShadowLightsList[], NiPointLight* LightsList[]) {
	D3DXVECTOR4 PlayerPosition = Player->pos.toD3DXVEC4();
	//Logger::Log(" ==== Getting lights ====");
	auto timer = TimeLogger();

	// create a map of all nearby valid lights and sort them per distance to player
	std::map<int, NiPointLight*> SceneLights;
	NiTList<ShadowSceneLight>::Entry* Entry = SceneNode->lights.start;

	while (Entry) {
		NiPointLight* Light = Entry->data->sourceLight;
		D3DXVECTOR4 LightPosition = Light->m_worldTransform.pos.toD3DXVEC4();

		bool lightCulled = Light->m_flags & NiAVObject::NiFlags::APP_CULLED;
		bool lightOn = (Light->Diff.r + Light->Diff.g + Light->Diff.b) * Light->Dimmer > 5.0 / 255.0; // Check for low values in case of human error
		if (lightCulled || !lightOn) {
			Entry = Entry->next;
			continue;
		}

		D3DXVECTOR4 LightVector = LightPosition - PlayerPosition;
		D3DXVec4Normalize(&LightVector, &LightVector);
		float inFront = D3DXVec4Dot(&LightVector, &TheRenderManager->CameraForward);
		float Distance = Light->GetDistance(&Player->pos);
		float radius = Light->Spec.r * TheSettingManager->SettingsShadows.Interiors.LightRadiusMult;

		// select lights that will be tracked by removing culled lights and lights behind the player further away than their radius
		// TODO: handle using frustum check
		float drawDistance = TheShaderManager->isExterior ? TheSettingManager->SettingsShadows.Exteriors.ShadowMapRadius[TheShadowManager->ShadowMapTypeEnum::MapLod] : TheSettingManager->SettingsShadows.Interiors.DrawDistance;
		if ((inFront > 0 || Distance < radius) && (Distance + radius) < drawDistance) {
			SceneLights[(int)(Distance * 10000)] = Light; // mutliplying distance (used as key) before convertion to avoid duplicates in case of similar values
		}

		Entry = Entry->next;
	}

	// save only the n first lights (based on #define TrackedLightsMax)
	memset(&TheShaderManager->LightPosition, 0, TrackedLightsMax * sizeof(D3DXVECTOR4)); // clear previous lights from array

	// get the data for all tracked lights
	int ShadowIndex = 0;
	int LightIndex = 0;
	TheShadowManager->PointLightsNum = 0;

#if defined(OBLIVION)
	bool TorchOnBeltEnabled = TheSettingManager->SettingsMain.EquipmentMode.Enabled && TheSettingManager->SettingsMain.EquipmentMode.TorchKey != 255;
#endif

	D3DXVECTOR4 Empty = D3DXVECTOR4(0, 0, 0, 0);

	std::map<int, NiPointLight*>::iterator v = SceneLights.begin();
	for (int i = 0; i < TrackedLightsMax + ShadowCubeMapsMax; i++) {
		if (v != SceneLights.end()) {
			NiPointLight* Light = v->second;
			if (!Light || Light->EffectType != NiDynamicEffect::EffectTypes::POINT_LIGHT) {
				v++;
				continue;
			}

			// determin if light is a shadow caster
			bool CastShadow = TheSettingManager->SettingsShadows.Interiors.UseCastShadowFlag ? Light->CastShadows : true;

#if defined(OBLIVION)
			// Oblivion exception for carried torch lights 
			if (TorchOnBeltEnabled && Light->CanCarry == 2) {
				HighProcessEx* Process = (HighProcessEx*)Player->process;
				if (Process->OnBeltState == HighProcessEx::State::In) CastShadow = false;
			}
#endif

			D3DXVECTOR4 LightPos = Light->m_worldTransform.pos.toD3DXVEC4();
			LightPos.w = Light->Spec.r * TheSettingManager->SettingsShadows.Interiors.LightRadiusMult;

			if (CastShadow && ShadowIndex < ShadowCubeMapsMax) {
				// add found light to list of lights that cast shadows
				ShadowLightsList[ShadowIndex] = Light;
				TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[ShadowIndex] = LightPos;
				ShadowIndex++;
				TheShadowManager->PointLightsNum++; // Constant to track number of shadow casting lights are present
			}
			else if (LightIndex < TrackedLightsMax) {
				LightsList[LightIndex] = Light;
				LightPosition[LightIndex] = LightPos;
				LightIndex++;
			};

			v++;
		}
		else {
			// set null values if number of lights in the scene becomes lower than previous iteration
			if (LightIndex < TrackedLightsMax) {
				//Logger::Log("clearing light at index %i", LightIndex);
				LightsList[LightIndex] = NULL;
				LightPosition[LightIndex] = Empty;
				LightIndex++;
			}
			if (ShadowIndex < ShadowCubeMapsMax) {
				//Logger::Log("clearing shadow casting light at index %i", ShadowIndex);
				ShadowLightsList[ShadowIndex] = NULL;
				ShaderConst.ShadowMap.ShadowLightPosition[ShadowIndex] = Empty;
				ShadowIndex++;
			}
		}
	}

	timer.LogTime("ShadowManager::GetNearbyLights");

}



/*
* Loads an Effect Shader from the corresponding fx file based on the Effect Record effect Type.
*/
EffectRecord* ShaderManager::CreateEffect(const char* Name) {

	if (!memcmp(Name, "AvgLuma", 8)) return new AvgLumaEffect();
	if (!memcmp(Name, "AmbientOcclusion", 17)) return new AmbientOcclusionEffect();
	if (!memcmp(Name, "ShadowsExteriors", 17)) return new ShadowsExteriorEffect();
	if (!memcmp(Name, "BloodLens", 10)) return new BloodLensEffect();
	if (!memcmp(Name, "BloomLegacy", 12)) return new BloomLegacyEffect();
	if (!memcmp(Name, "Cinema", 7)) return new CinemaEffect();
	if (!memcmp(Name, "DepthOfField", 13)) return new DepthOfFieldEffect();
	if (!memcmp(Name, "Exposure", 9)) return new ExposureEffect();
	if (!memcmp(Name, "Debug", 6)) return new DebugEffect();
	if (!memcmp(Name, "GodRays", 8)) return new GodRaysEffect();
	if (!memcmp(Name, "ImageAdjust", 12)) return new ImageAdjustEffect();
	if (!memcmp(Name, "Lens", 5)) return new LensEffect();
	if (!memcmp(Name, "LowHF", 6)) return new LowHFEffect();
	if (!memcmp(Name, "Normals", 8)) return new NormalsEffect();
	if (!memcmp(Name, "MotionBlur", 11)) return new MotionBlurEffect();
	if (!memcmp(Name, "Precipitations", 15)) return new RainEffect();
	if (!memcmp(Name, "Sharpening", 11)) return new SharpeningEffect();
	if (!memcmp(Name, "Specular", 9)) return new SpecularEffect();
	if (!memcmp(Name, "SunShadows", 11)) return new SunShadowsEffect();
	if (!memcmp(Name, "PointShadows", 13)) return new PointShadowsEffect();
	if (!memcmp(Name, "PointShadows2", 14)) return new PointShadows2Effect();
	if (!memcmp(Name, "Pretonemapper", 14)) return new PretonemapperEffect();
	if (!memcmp(Name, "ShadowsInteriors", 17)) return new ShadowsInteriorsEffect();
	if (!memcmp(Name, "SnowAccumulation", 17)) return new SnowAccumulationEffect();
	if (!memcmp(Name, "Snow", 5)) return new SnowEffect();
	if (!memcmp(Name, "Underwater", 11)) return new UnderwaterEffect();
	if (!memcmp(Name, "VolumetricFog", 14)) return new VolumetricFogEffect();
	if (!memcmp(Name, "WaterLens", 10)) return new WaterLensEffect();
	if (!memcmp(Name, "WetWorld", 9)) return new WetWorldEffect();

	return new EffectRecord(Name);


	// TODO: simplify and streamline extra shaders creation
	//SettingsMainStruct* SettingsMain = &TheSettingManager->SettingsMain;
	//	case EffectRecord::EffectRecordType::Extra:
	//		WIN32_FIND_DATAA File;
	//		HANDLE H;
	//		char* cFileName = NULL;
	//		EffectRecord* ExtraEffect = NULL;

	//		if (SettingsMain->Develop.CompileEffects)
	//			strcat(Filename, "Extra\\*.hlsl");
	//		else
	//			strcat(Filename, "Extra\\*.fx");
	//		H = FindFirstFileA((LPCSTR)Filename, &File);
	//		if (H != INVALID_HANDLE_VALUE) {
	//			cFileName = (char*)File.cFileName;
	//			if (SettingsMain->Develop.CompileEffects) File.cFileName[strlen(cFileName) - 5] = '\0';
	//			strcpy(Filename, EffectsPath);
	//			strcat(Filename, "Extra\\");
	//			strcat(Filename, cFileName);
	//			ExtraEffect = EffectRecord::LoadEffect(Filename);
	//			if (ExtraEffect) ExtraEffects[std::string(cFileName).substr(0, strlen(cFileName) - 3)] = ExtraEffect;
	//			while (FindNextFileA(H, &File)) {
	//				cFileName = (char*)File.cFileName;
	//				if (SettingsMain->Develop.CompileEffects) File.cFileName[strlen(cFileName) - 5] = '\0';
	//				strcpy(Filename, EffectsPath);
	//				strcat(Filename, "Extra\\");
	//				strcat(Filename, cFileName);
	//				ExtraEffect = EffectRecord::LoadEffect(Filename);
	//				if (ExtraEffect) ExtraEffects[std::string(cFileName).substr(0, strlen(cFileName) - 3)] = ExtraEffect;
	//			}
	//			FindClose(H);
	//		}
	//		break;
	//}
}


ShaderCollection* ShaderManager::CreateCollection(const char* Name) {
	return new ShaderCollection(Name);
}

/*
* Deletes an Effect based on the Effect Record effect type. 
*/
void ShaderManager::DisposeEffect(EffectRecord** Effect) {
	*Effect = NULL;
	delete* Effect;

	//	case EffectRecord::EffectRecordType::Extra:
	//		ExtraEffectsList::iterator v = ExtraEffects.begin();
	//		while (v != ExtraEffects.end()) {
	//			delete v->second;
	//			v++;
	//		}
	//		ExtraEffects.clear();
	//		break;
	//}
}

/*
* Renders a given effect to an arbitrary render target
*/
void ShaderManager::RenderEffectToRT(IDirect3DSurface9* RenderTarget, EffectRecord* Effect, bool clearRenderTarget) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	Device->SetRenderTarget(0, RenderTarget);
	Device->StretchRect(RenderTarget, NULL, RenderTarget, NULL, D3DTEXF_NONE);
	Effect->Render(Device, RenderTarget, RenderTarget, 0, clearRenderTarget, NULL);
};


void ShaderManager::RenderEffectsPreTonemapping(IDirect3DSurface9* RenderTarget) {
	TheRenderManager->UpdateSceneCameraData();
	TheRenderManager->SetupSceneCamera();

	if (!TheSettingManager->SettingsMain.Main.RenderEffects) return; // Main toggle

	auto timer = TimeLogger();

	IDirect3DDevice9* Device = TheRenderManager->device;
	IDirect3DSurface9* SourceSurface = TheTextureManager->SourceSurface;
	IDirect3DSurface9* RenderedSurface = TheTextureManager->RenderedSurface;

	// prepare device for effects
	Device->SetStreamSource(0, FrameVertex, 0, sizeof(FrameVS));
	Device->SetFVF(FrameFVF);

	// render post process normals for use by shaders
	RenderEffectToRT(TheTextureManager->NormalsSurface, Effects.Normals, false);

	// render a shadow pass for point lights
	if ((isExterior && Effects.ShadowsExteriors->Enabled) || (!isExterior && Effects.ShadowsInteriors->Enabled)) {
		// separate lights in 2 batches
		RenderEffectToRT(TheTextureManager->ShadowPassSurface, Effects.PointShadows, true);
		if (TheShadowManager->PointLightsNum > 6) RenderEffectToRT(TheTextureManager->ShadowPassSurface, Effects.PointShadows2, false);
		if (isExterior) RenderEffectToRT(TheTextureManager->ShadowPassSurface, Effects.SunShadows, false);
	}

	if (!Player->parentCell) return;
	if (OverlayIsOn) return; // disable all effects during terminal/lockpicking sequences because they bleed through the overlay

	Device->SetRenderTarget(0, RenderTarget);

	// copy the source render target to both the rendered and source textures (rendered gets updated after every pass, source once per effect)
	Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
	Device->StretchRect(RenderTarget, NULL, SourceSurface, NULL, D3DTEXF_NONE);

	if (isExterior) Effects.ShadowsExteriors->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	else Effects.ShadowsInteriors->Render(Device, RenderTarget, RenderedSurface, 0, true, SourceSurface);

	Effects.AmbientOcclusion->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	if (isUnderwater) {
		// underwater only effects
		Effects.Underwater->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	} else {
		if (isExterior) {
			Effects.Specular->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
			if (Effects.WetWorld->Constants.Data.z > 0.0f) Effects.WetWorld->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
			if (Effects.SnowAccumulation->Constants.Data.w > 0.0f) Effects.SnowAccumulation->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
		}

		if (!PipBoyIsOn) Effects.VolumetricFog->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	}
	Effects.GodRays->Render(Device, RenderTarget, RenderedSurface, 0, true, SourceSurface);
	
	// For AMD devices without DXVK, replace vanilla tonemapping replacement with an Effect
	if ((TheRenderManager->RESZ && !TheRenderManager->DXVK) && TheSettingManager->GetMenuShaderEnabled("Tonemapping"))
		Effects.Pretonemapper->Render(Device, RenderTarget, RenderedSurface, 0, true, SourceSurface);

	timer.LogTime("ShaderManager::RenderEffectsPreTonemapping");
}


/*
* Renders the effect that have been set to enabled.
*/
void ShaderManager::RenderEffects(IDirect3DSurface9* RenderTarget) {
	if (!TheSettingManager->SettingsMain.Main.RenderEffects) return; // Main toggle
	if (!Player->parentCell) return;
	if (OverlayIsOn) return; // disable all effects during terminal/lockpicking sequences because they bleed through the overlay

	auto timer = TimeLogger();

	TheRenderManager->UpdateSceneCameraData();
	TheRenderManager->SetupSceneCamera();

	IDirect3DDevice9* Device = TheRenderManager->device;
	NiDX9RenderState* RenderState = TheRenderManager->renderState;
	IDirect3DSurface9* SourceSurface = TheTextureManager->SourceSurface;
	IDirect3DSurface9* RenderedSurface = TheTextureManager->RenderedSurface;

	Device->SetStreamSource(0, FrameVertex, 0, sizeof(FrameVS));
	Device->SetFVF(FrameFVF);

	// prepare device for effects
	Device->SetRenderTarget(0, RenderTarget);

	// copy the source render target to both the rendered and source textures (rendered gets updated after every pass, source once per effect)
	Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
	Device->StretchRect(RenderTarget, NULL, SourceSurface, NULL, D3DTEXF_NONE);

	if (!isUnderwater && isExterior) {
		if (Effects.Rain->Constants.Data.x > 0.0f) Effects.Rain->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
		if (Effects.Snow->Constants.Data.x > 0.0f) Effects.Snow->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	}

	Effects.BloomLegacy->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// calculate average luma for use by shaders
	if (avglumaRequired) {
		RenderEffectToRT(TheTextureManager->AvgLumaSurface, Effects.AvgLuma, NULL);
		Device->SetRenderTarget(0, RenderTarget); 	// restore device used for effects
	}

	Effects.Exposure->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// screenspace coloring/blurring effects get rendered last
	Effects.Coloring->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	if (Effects.DepthOfField->Constants.Enabled) Effects.DepthOfField->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	if (Effects.MotionBlur->Constants.Data.x || Effects.MotionBlur->Constants.Data.y) Effects.MotionBlur->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// lens effects
	if (Effects.BloodLens->Constants.Percent > 0.0f) Effects.BloodLens->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	if (Effects.WaterLens->Constants.Data.w > 0.0f) Effects.WaterLens->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	if (Effects.LowHF->Constants.Data.x) Effects.LowHF->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);
	if (!isUnderwater) Effects.Lens->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.Sharpening->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);

	// cinema effect gets rendered very last because of vignetting/letterboxing
	Effects.Cinema->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// final adjustments
	Effects.ImageAdjust->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// debug shader allows to display some of the buffers
	Effects.Debug->Render(Device, RenderTarget, RenderedSurface, 0, false, NULL);

	//if (EffectsSettings->Extra) {
	//	for (EffectsList::iterator iter = Effects.ExtraEffects.begin(); iter != Effects.ExtraEffects.end(); ++iter) {
	//		if (iter->second->Enabled) {
	//			iter->second->Render(Device, RenderTarget, RenderedSurface, false, true);
	//		}
	//	}
	//}

	timer.LogTime("ShaderManager::RenderEffects");
}

/*
* Writes the settings corresponding to the shader/effect name, to switch it between enabled/disabled.*
* Also creates or deletes the corresponding Effect Record.
*/
void ShaderManager::SwitchShaderStatus(const char* Name) {
	IsMenuSwitch = true;

	// effects
	try {
		EffectRecord* effect = *EffectsNames.at(Name);
		bool setting = effect->SwitchEffect();
		TheSettingManager->SetMenuShaderEnabled(Name, setting);
	}
	catch (const std::exception& e){
		// shaders
		Logger::Log("Toggling Shader %s", Name);
		bool enable = !TheSettingManager->GetMenuShaderEnabled(Name);
		DisposeShader(Name);
		if (enable) CreateShader(Name);
		TheSettingManager->SetMenuShaderEnabled(Name, enable);
	}

	//else if (!strcmp(Name, "ExtraEffectsSettings")) { //TODO change to new effect switch
	//	EffectsSettings->Extra = !EffectsSettings->Extra;
	//	DisposeEffect(EffectRecord::EffectRecordType::Extra);
	//	if (EffectsSettings->Extra) CreateEffect(EffectRecord::EffectRecordType::Extra);
	//}

	IsMenuSwitch = false;
}

void ShaderManager::SetCustomConstant(const char* Name, D3DXVECTOR4 Value) {
	CustomConstants::iterator v = CustomConst.find(std::string(Name));
	if (v != CustomConst.end()) v->second = Value;
}

void ShaderManager::SetExtraEffectEnabled(const char* Name, bool Value) {
	//EffectsList::iterator v = Effects.ExtraEffects.find(std::string(Name));
	//if (v != Effects.ExtraEffects.end()) v->second->Enabled = Value;
}

float ShaderManager::lerp(float a, float b, float t) {
	return std::lerp(a, b, t);
}

D3DXVECTOR4 ShaderManager::lerp(D3DXVECTOR4 a, D3DXVECTOR4 b, float t) {
	D3DXVECTOR4 result;
	result.x = std::lerp(a.x, b.x, t);
	result.y = std::lerp(a.y, b.y, t);
	result.z = std::lerp(a.z, b.z, t);
	result.w = std::lerp(a.w, b.w, t);

	return result;
}

float ShaderManager::step(float a, float b, float t) {
	return max(0, min(1, invLerp(a, b, t)));
}

float ShaderManager::invLerp(float a, float b, float t) {
	return(t - a) / (b - a);
}

float ShaderManager::clamp(float a, float b, float t) {
	return min(max(a, t), b);
}

float ShaderManager::smoothStep(float a, float b, float t) {
	t = clamp(0.0, 1.0, invLerp(a, b, t));
	return t * t * (3.0 - 2.0 * t);
}
