#define RESZ_CODE 0x7FA05000

/**
* Initializes the Shader Manager Singleton.
* The Shader Manager creates and holds onto the Effects activated in the Settings Manager, and sets the constants.
*/
void ShaderManager::Initialize() {

	auto timer = TimeLogger();

	Logger::Log("Starting the shaders manager...");
	TheShaderManager = new ShaderManager();
	TheShaderManager->FrameVertex = NULL;

	memset(TheShaderManager->WaterVertexShaders, NULL, sizeof(WaterVertexShaders));
	memset(TheShaderManager->WaterPixelShaders, NULL, sizeof(WaterPixelShaders));

	TheShaderManager->CreateFrameVertex(TheRenderManager->width, TheRenderManager->height, &TheShaderManager->FrameVertex);

	TheShaderManager->PreviousCell = nullptr;
	TheShaderManager->IsMenuSwitch = false;

	// initializing the list of effect names
	TheShaderManager->RegisterEffect<AvgLumaEffect>(&TheShaderManager->Effects.AvgLuma);
	TheShaderManager->RegisterEffect<AmbientOcclusionEffect>(&TheShaderManager->Effects.AmbientOcclusion);
	TheShaderManager->RegisterEffect<BloodLensEffect>(&TheShaderManager->Effects.BloodLens);
	TheShaderManager->RegisterEffect<BloomEffect>(&TheShaderManager->Effects.Bloom);
	TheShaderManager->RegisterEffect<BloomLegacyEffect>(&TheShaderManager->Effects.BloomLegacy);
	TheShaderManager->RegisterEffect<ColoringEffect>(&TheShaderManager->Effects.Coloring);
	TheShaderManager->RegisterEffect<CinemaEffect>(&TheShaderManager->Effects.Cinema);
	TheShaderManager->RegisterEffect<CombineDepthEffect>(&TheShaderManager->Effects.CombineDepth);
	TheShaderManager->RegisterEffect<DepthOfFieldEffect>(&TheShaderManager->Effects.DepthOfField);
	TheShaderManager->RegisterEffect<DebugEffect>(&TheShaderManager->Effects.Debug);
	TheShaderManager->RegisterEffect<ExposureEffect>(&TheShaderManager->Effects.Exposure);
	TheShaderManager->RegisterEffect<FlashlightEffect>(&TheShaderManager->Effects.Flashlight);
	TheShaderManager->RegisterEffect<GodRaysEffect>(&TheShaderManager->Effects.GodRays);
	TheShaderManager->RegisterEffect<ImageAdjustEffect>(&TheShaderManager->Effects.ImageAdjust);
	TheShaderManager->RegisterEffect<LensEffect>(&TheShaderManager->Effects.Lens);
	TheShaderManager->RegisterEffect<LowHFEffect>(&TheShaderManager->Effects.LowHF);
	TheShaderManager->RegisterEffect<MotionBlurEffect>(&TheShaderManager->Effects.MotionBlur);
	TheShaderManager->RegisterEffect<NormalsEffect>(&TheShaderManager->Effects.Normals);
	TheShaderManager->RegisterEffect<RainEffect>(&TheShaderManager->Effects.Rain);
	TheShaderManager->RegisterEffect<SharpeningEffect>(&TheShaderManager->Effects.Sharpening);
	TheShaderManager->RegisterEffect<ShadowsExteriorEffect>(&TheShaderManager->Effects.ShadowsExteriors);
	TheShaderManager->RegisterEffect<ShadowsInteriorsEffect>(&TheShaderManager->Effects.ShadowsInteriors);
	TheShaderManager->RegisterEffect<PointShadowsEffect>(&TheShaderManager->Effects.PointShadows);
	TheShaderManager->RegisterEffect<PointShadows2Effect>(&TheShaderManager->Effects.PointShadows2);
	TheShaderManager->RegisterEffect<SunShadowsEffect>(&TheShaderManager->Effects.SunShadows);
	TheShaderManager->RegisterEffect<SpecularEffect>(&TheShaderManager->Effects.Specular);
	TheShaderManager->RegisterEffect<SnowEffect>(&TheShaderManager->Effects.Snow);
	TheShaderManager->RegisterEffect<SnowAccumulationEffect>(&TheShaderManager->Effects.SnowAccumulation);
	TheShaderManager->RegisterEffect<UnderwaterEffect>(&TheShaderManager->Effects.Underwater);
	TheShaderManager->RegisterEffect<VolumetricFogEffect>(&TheShaderManager->Effects.VolumetricFog);
	TheShaderManager->RegisterEffect<WaterLensEffect>(&TheShaderManager->Effects.WaterLens);
	TheShaderManager->RegisterEffect<WetWorldEffect>(&TheShaderManager->Effects.WetWorld);

	TheShaderManager->RegisterShaderCollection<TonemappingShaders>(&TheShaderManager->Shaders.Tonemapping);
	TheShaderManager->RegisterShaderCollection<POMShaders>(&TheShaderManager->Shaders.POM);
	TheShaderManager->RegisterShaderCollection<PBRShaders>(&TheShaderManager->Shaders.PBR);
	TheShaderManager->RegisterShaderCollection<WaterShaders>(&TheShaderManager->Shaders.Water);
	TheShaderManager->RegisterShaderCollection<SkyShaders>(&TheShaderManager->Shaders.Sky);
	TheShaderManager->RegisterShaderCollection<SkinShaders>(&TheShaderManager->Shaders.Skin);
	TheShaderManager->RegisterShaderCollection<GrassShaders>(&TheShaderManager->Shaders.Grass);
	TheShaderManager->RegisterShaderCollection<TerrainShaders>(&TheShaderManager->Shaders.Terrain);
	TheShaderManager->RegisterShaderCollection<ExtraShaders>(&TheShaderManager->Shaders.ExtraShaders);
	
	//setup map of constant names
	TheShaderManager->RegisterConstant("TESR_WorldTransform", (D3DXVECTOR4*)&TheRenderManager->worldMatrix);
	TheShaderManager->RegisterConstant("TESR_ViewTransform", (D3DXVECTOR4*)&TheRenderManager->viewMatrix);
	TheShaderManager->RegisterConstant("TESR_ProjectionTransform", (D3DXVECTOR4*)&TheRenderManager->projMatrix);
	TheShaderManager->RegisterConstant("TESR_InvProjectionTransform",  (D3DXVECTOR4*)&TheRenderManager->InvProjMatrix);
	TheShaderManager->RegisterConstant("TESR_WorldViewProjectionTransform",  (D3DXVECTOR4*)&TheRenderManager->WorldViewProjMatrix);
	TheShaderManager->RegisterConstant("TESR_InvViewProjectionTransform", (D3DXVECTOR4*)&TheRenderManager->InvViewProjMatrix);
	TheShaderManager->RegisterConstant("TESR_ViewProjectionTransform", (D3DXVECTOR4*)&TheRenderManager->ViewProjMatrix);
	TheShaderManager->RegisterConstant("TESR_OcclusionWorldViewProjTransform", (D3DXVECTOR4*)&TheShaderManager->ShaderConst.OcclusionMap.OcclusionWorldViewProj);
	TheShaderManager->RegisterConstant("TESR_LightPosition", (D3DXVECTOR4*) &TheShaderManager->LightPosition);
	TheShaderManager->RegisterConstant("TESR_LightColor", (D3DXVECTOR4*) &TheShaderManager->LightColor);
	TheShaderManager->RegisterConstant("TESR_SpotLightPosition", (D3DXVECTOR4*) &TheShaderManager->SpotLightPosition);
	TheShaderManager->RegisterConstant("TESR_SpotLightColor", (D3DXVECTOR4*) &TheShaderManager->SpotLightColor);
	TheShaderManager->RegisterConstant("TESR_SpotLightDirection", (D3DXVECTOR4*) &TheShaderManager->SpotLightDirection);
	TheShaderManager->RegisterConstant("TESR_SpotLightToWorldTransform", (D3DXVECTOR4*) &TheShaderManager->SpotLightWorldToLightMatrix[0]);
	TheShaderManager->RegisterConstant("TESR_ViewSpaceLightDir", &TheShaderManager->ShaderConst.ViewSpaceLightDir);
	TheShaderManager->RegisterConstant("TESR_ScreenSpaceLightDir", &TheShaderManager->ShaderConst.ScreenSpaceLightDir);
	TheShaderManager->RegisterConstant("TESR_ReciprocalResolution", &TheShaderManager->ShaderConst.ReciprocalResolution);
	TheShaderManager->RegisterConstant("TESR_CameraForward", &TheRenderManager->CameraForward);
	TheShaderManager->RegisterConstant("TESR_DepthConstants", &TheRenderManager->DepthConstants);
	TheShaderManager->RegisterConstant("TESR_CameraData", &TheRenderManager->CameraData);
	TheShaderManager->RegisterConstant("TESR_CameraPosition", &TheRenderManager->CameraPosition);
	TheShaderManager->RegisterConstant("TESR_SunDirection", &TheShaderManager->ShaderConst.SunDir);
	TheShaderManager->RegisterConstant("TESR_SunPosition", &TheShaderManager->ShaderConst.SunPosition);
	TheShaderManager->RegisterConstant("TESR_SunTiming", &TheShaderManager->ShaderConst.SunTiming);
	TheShaderManager->RegisterConstant("TESR_SunAmount", &TheShaderManager->ShaderConst.SunAmount);
	TheShaderManager->RegisterConstant("TESR_GameTime", &TheShaderManager->ShaderConst.GameTime);
	TheShaderManager->RegisterConstant("TESR_FogData", &TheShaderManager->ShaderConst.fogData);
	TheShaderManager->RegisterConstant("TESR_FogDistance", &TheShaderManager->ShaderConst.fogDistance);
	TheShaderManager->RegisterConstant("TESR_FogColor", &TheShaderManager->ShaderConst.fogColor);
	TheShaderManager->RegisterConstant("TESR_SunColor", &TheShaderManager->ShaderConst.sunColor);
	TheShaderManager->RegisterConstant("TESR_SunDiskColor", &TheShaderManager->ShaderConst.sunDiskColor);
	TheShaderManager->RegisterConstant("TESR_SunAmbient", &TheShaderManager->ShaderConst.sunAmbient);
	TheShaderManager->RegisterConstant("TESR_SkyColor", &TheShaderManager->ShaderConst.skyColor);
	TheShaderManager->RegisterConstant("TESR_SkyLowColor", &TheShaderManager->ShaderConst.skyLowColor);
	TheShaderManager->RegisterConstant("TESR_HorizonColor", &TheShaderManager->ShaderConst.horizonColor);

	TheShaderManager->InitializeConstants();

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
* Initializes and register an effect and its constants
*/
template <typename T> void ShaderManager::RegisterEffect(T** Pointer)
{
	T* effect = new T();
	*Pointer = effect;

	EffectsNames[effect->Name] = (EffectRecord**)Pointer;
	effect->UpdateSettings();
	effect->RegisterConstants();
	effect->RegisterTextures();
	effect->LoadEffect();
}


template <typename T> void ShaderManager::RegisterShaderCollection(T** Pointer)
{
	T* collection = new T();
	*Pointer = collection;
	
	ShaderNames[collection->Name] = (ShaderCollection**)Pointer;
	collection->RegisterConstants();
}


void ShaderManager::RegisterConstant(const char* Name, D3DXVECTOR4* FloatValue)
{
	ConstantsTable[Name] = FloatValue;
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

	if (!TheSettingManager->SettingsMain.Main.RenderEffects) return; // Main toggle

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
	GameState.PipBoyIsOn = InterfaceManager->getIsMenuOpen();
	GameState.VATSIsOn = InterfaceManager->IsActive(Menu::kMenuType_VATS);
	GameState.OverlayIsOn = InterfaceManager->IsActive(Menu::kMenuType_Computers) ||
		InterfaceManager->IsActive(Menu::kMenuType_LockPick) ||
		InterfaceManager->IsActive(Menu::kMenuType_Surgery) ||
		InterfaceManager->IsActive(Menu::kMenuType_SlotMachine) ||
		InterfaceManager->IsActive(Menu::kMenuType_Blackjack) ||
		InterfaceManager->IsActive(Menu::kMenuType_Roulette) ||
		InterfaceManager->IsActive(Menu::kMenuType_Caravan);
	GameState.isDialog = InterfaceManager->IsActive(Menu::MenuType::kMenuType_Dialog);
	GameState.isPersuasion = InterfaceManager->IsActive(Menu::MenuType::kMenuType_Persuasion);
	
	if (!currentCell) return; // if no cell is present, we skip update to avoid trying to access values that don't exist
	
	GameState.isExterior = !currentCell->IsInterior();// || Player->parentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior; // < use exterior flag, broken for now
	GameState.isCellChanged = currentCell != PreviousCell;
	PreviousCell = currentCell;
	if (GameState.isCellChanged) TheSettingManager->SettingsChanged = true; // force update constants during cell transition

	GameState.isUnderwater = Tes->sky->GetIsUnderWater();
	GameState.isRainy = currentWeather?currentWeather->GetWeatherType() == TESWeather::WeatherType::kType_Rainy : false;
	GameState.isSnow = currentWeather?currentWeather->GetWeatherType() == TESWeather::WeatherType::kType_Snow : false;

	TimeGlobals* GameTimeGlobals = TimeGlobals::Get();
	float GameHour = fmod(GameTimeGlobals->GameHour->data, 24); // make sure the hours values are less than 24

	float SunriseStart = WorldSky->GetSunriseBegin();
	float SunriseEnd = WorldSky->GetSunriseEnd();
	float SunsetStart = WorldSky->GetSunsetBegin();
	float SunsetEnd = WorldSky->GetSunsetEnd();

	// calculating sun amount for shaders (currently not used by any shaders)
	float sunRise = step(SunriseStart, SunriseEnd, GameHour); // 0 at night to 1 after sunrise
	float sunSet = step(SunsetEnd, SunsetStart, GameHour);  // 1 before sunset to 0 at night
	GameState.isDayTime = sunRise * sunSet;

	ShaderConst.SunTiming.x = WorldSky->GetSunriseColorBegin();
	ShaderConst.SunTiming.y = SunriseEnd;
	ShaderConst.SunTiming.z = SunsetStart;
	ShaderConst.SunTiming.w = WorldSky->GetSunsetColorEnd();

	// fake sunset time tracking with more time given before sunrise/sunset
	float sunRiseLight = step(SunriseStart - 1.0f, SunriseEnd - 1.0f, GameHour); // 0 at night to 1 after sunrise
	float sunSetLight = step(SunsetEnd + 1.0f, SunsetStart + 1.0f, GameHour);  // 1 before sunset to 0 at night
	float newDayLight = sunRiseLight * sunSetLight;
	GameState.transitionCurve = smoothStep(0.0f, 1.0f, newDayLight); // a curve for day/night transitions that occurs mostly during second half of sunset

	GameState.isDayTimeChanged = (newDayLight != GameState.dayLight);  // allow effects to fire settings update during sunset/sunrise transitions
	GameState.dayLight = newDayLight;

	ShaderConst.GameTime.x = TimeGlobals::GetGameTime(); //time in milliseconds
	ShaderConst.GameTime.y = GameHour; //time in hours
	ShaderConst.GameTime.z = (float)TheFrameRateManager->Time;
	ShaderConst.GameTime.w = TheFrameRateManager->ElapsedTime; // frameTime in seconds

	ShaderConst.SunPosition = SunRoot->m_localTransform.pos.toD3DXVEC4();
	D3DXVec4Normalize(&ShaderConst.SunPosition, &ShaderConst.SunPosition);
	ShaderConst.SunDir = Tes->directionalLight->direction.toD3DXVEC4() * -1.0f;

	// during the day, track the sun mesh position instead of the lighting direction in exteriors
	if (GameState.isExterior && GameState.dayLight > 0.5)
		ShaderConst.SunDir = ShaderConst.SunPosition;
	else
		ShaderConst.SunPosition.z = -ShaderConst.SunPosition.z;


	// expose the light vector in view space for screen space lighting
	D3DXVec4Transform(&ShaderConst.ScreenSpaceLightDir, &ShaderConst.SunDir, &TheRenderManager->ViewProjMatrix);
	D3DXVec4Normalize(&ShaderConst.ScreenSpaceLightDir, &ShaderConst.ScreenSpaceLightDir);

	D3DXVec4Transform(&ShaderConst.ViewSpaceLightDir, &ShaderConst.SunDir, &TheRenderManager->ViewMatrix);
	D3DXVec4Normalize(&ShaderConst.ViewSpaceLightDir, &ShaderConst.ViewSpaceLightDir);

	ShaderConst.sunGlare = currentWeather ? (currentWeather->GetSunGlare() / 255.0f) : 0.5f;

	ShaderConst.SunAmount.y = GameState.isDayTime; // accurate 0 - 1 value based on weather transition times
	GameState.isDayTime = smoothStep(0, 1, GameState.dayLight); // smooth daytime progression -- more accurate to light changes
	ShaderConst.SunAmount.x = GameState.isDayTime;

	ShaderConst.sunColor.x = WorldSky->sunDirectional.r;
	ShaderConst.sunColor.y = WorldSky->sunDirectional.g;
	ShaderConst.sunColor.z = WorldSky->sunDirectional.b;
	ShaderConst.sunColor.w = ShaderConst.sunGlare;

	if (Shaders.Sky->useSunDiskColor) {
		// experimental color used for more sky tinting capabilities
		ShaderConst.sunDiskColor.x = WorldSky->SunColor.r;
		ShaderConst.sunDiskColor.y = WorldSky->SunColor.g;
		ShaderConst.sunDiskColor.z = WorldSky->SunColor.b;
		ShaderConst.sunDiskColor.w = 1.0;
	}
	else {
		ShaderConst.sunDiskColor = ShaderConst.sunColor; // override with the color of the lighting
	}

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

	// replicate vanilla behavior of enforcing max fog distance in interiors
	ShaderConst.fogData.y = WorldSky->fogFarPlane;
	if (!GameState.isExterior && (WorldSky->fogFarPlane < 0 || WorldSky->fogFarPlane > 163840)) {
		ShaderConst.fogData.y = 163840;
	}

	// for near plane, ensure that far > near
	ShaderConst.fogData.x = WorldSky->fogNearPlane;
	if (WorldSky->fogNearPlane < 0 || ShaderConst.fogData.y < WorldSky->fogNearPlane)
		ShaderConst.fogData.x = ShaderConst.fogData.y * 0.17;

	ShaderConst.fogData.z = ShaderConst.sunGlare;
	ShaderConst.fogData.w = WorldSky->fogPower;

	ShaderConst.fogDistance.x = ShaderConst.fogData.x;
	ShaderConst.fogDistance.y = ShaderConst.fogData.y;
	ShaderConst.fogDistance.z = 1.0f;
	ShaderConst.fogDistance.w = ShaderConst.sunGlare;

	timer.LogTime("ShaderManager::UpdateConstants for generic constants");

	if (TheSettingManager->SettingsChanged) {
		TheGameMenuManager->UpdateSettings();

		// update settings
		for (const auto [Name, effect] : EffectsNames) {
			(*effect)->UpdateSettings();
		}
		for (const auto [Name, shader] : ShaderNames) {
			(*shader)->UpdateSettings();
		}

		// sky settings are used in several shaders whether the shader is active or not
		ShaderConst.SunAmount.z = TheSettingManager->GetSettingI("Shaders.Sky.Main", "ReplaceSun");
		ShaderConst.SunAmount.w = TheSettingManager->GetSettingF("Shaders.Sky.Main", "GlareStrength");
		timer.LogTime("ShaderManager::UpdateSettings for shaders & effects");
	}

	// update Constants
	for (const auto [Name, shader] : ShaderNames) {
		if ((*shader)->Enabled) (*shader)->UpdateConstants();
	}
	timer.LogTime("ShaderManager::UpdateConstants for shaders");

	for (const auto [Name, effect] : EffectsNames) {
		if ((*effect)->Enabled) {
			(*effect)->UpdateConstants();
			(*effect)->constantUpdateTime = timer.LogTime((*effect)->Name);
		}
		else {
			(*effect)->constantUpdateTime = 0;
		}
	}

	// Underwater effect uses constants from the water shader
	if (Effects.Underwater->Enabled && !Shaders.Water->Enabled) Shaders.Water->UpdateConstants();
	if (!Effects.ShadowsExteriors->Enabled && Effects.ShadowsInteriors->Enabled) Effects.ShadowsExteriors->UpdateConstants(); // Interior and exterior shadows share settings

	TheSettingManager->SettingsChanged = false;
	timer.LogTime("ShaderManager::UpdateConstants");
}


float ShaderManager::GetTransitionValue(float Day, float Night, float Interior) {
	if (GameState.isExterior) {
		return std::lerp(Night, Day, GameState.transitionCurve);
	}
	else {
		return Interior;
	}
}


ShaderCollection* ShaderManager::GetShaderCollection(const char* Name) {

	if (!memcmp(Name, "WATER", 5)) return Shaders.Water;
	if (!memcmp(Name, "GRASS", 5)) return Shaders.Grass;
	if (!memcmp(Name, "ISHDR", 5) || !memcmp(Name, "HDR", 3)) return Shaders.Tonemapping; // tonemapping shaders have different names between New vegas and Oblivion
	if (!memcmp(Name, "PAR", 3)) return Shaders.POM;
	if (!memcmp(Name, "SKIN", 4)) return Shaders.Skin;
	if (!memcmp(Name, "SKY", 3)) return Shaders.Sky;
	if (strstr(TerrainShadersNames, Name)) return Shaders.Terrain;
	if (strstr(PBRShadersNames, Name)) return Shaders.PBR;
	if (strstr(BloodShaders, Name)) return Shaders.Blood;

	return Shaders.ExtraShaders;
}


/*
* Load generic Vertex Shaders as well as the ones for interiors and exteriors if the exist. 
* Returns false if generic one isn't found (as other ones are optional)
*/
bool ShaderManager::LoadShader(NiD3DVertexShader* Shader) {
	
	NiD3DVertexShaderEx* VertexShader = (NiD3DVertexShaderEx*)Shader;
	ShaderCollection* Collection = GetShaderCollection(VertexShader->Name);
	
	bool enabled = Collection->Enabled;

	ShaderTemplate* Template = Collection->GetTemplate(VertexShader->Name);

	// Load generic, interior and exterior shaders
	VertexShader->ShaderProg[ShaderRecordType::Default]  = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->Name, NULL, Template);
	VertexShader->ShaderProg[ShaderRecordType::Exterior] = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->Name, "Exteriors\\", Template);
	VertexShader->ShaderProg[ShaderRecordType::Interior] = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->Name, "Interiors\\", Template);
	VertexShader->Enabled = enabled;

	if (VertexShader->ShaderProg[ShaderRecordType::Default] != nullptr || VertexShader->ShaderProg[ShaderRecordType::Exterior] != nullptr || VertexShader->ShaderProg[ShaderRecordType::Interior] != nullptr) {
		Collection->VertexShaderList.push_back(VertexShader);
		Logger::Log("Loaded %s Vertex Shader %s", Collection->Name, VertexShader->Name);
	}

	return enabled;
}


/*
* Load generic Pixel Shaders as well as the ones for interiors and exteriors if the exist. 
* Returns false if generic one isn't found (as other ones are optional)
*/
bool ShaderManager::LoadShader(NiD3DPixelShader* Shader) {

	NiD3DPixelShaderEx* PixelShader = (NiD3DPixelShaderEx*)Shader;
	ShaderCollection* Collection = GetShaderCollection(PixelShader->Name);

	bool enabled = Collection->Enabled;

	ShaderTemplate* Template = Collection->GetTemplate(PixelShader->Name);

	PixelShader->ShaderProg[ShaderRecordType::Default]  = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->Name, NULL, Template);
	PixelShader->ShaderProg[ShaderRecordType::Exterior] = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->Name, "Exteriors\\", Template);
	PixelShader->ShaderProg[ShaderRecordType::Interior] = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->Name, "Interiors\\", Template);
	PixelShader->Enabled = enabled;

	if (PixelShader->ShaderProg[ShaderRecordType::Default] != nullptr || PixelShader->ShaderProg[ShaderRecordType::Exterior] != nullptr || PixelShader->ShaderProg[ShaderRecordType::Interior] != nullptr) {
		Collection->PixelShaderList.push_back(PixelShader);
		Logger::Log("Loaded %s Pixel Shader %s", Collection->Name, PixelShader->Name);
	}

	return enabled;
}


void ShaderManager::GetNearbyLights(ShadowSceneLight* ShadowLightsList[], NiPointLight* LightsList[], NiSpotLight* SpotLightList[]) {
	D3DXVECTOR4 PlayerPosition = Player->pos.toD3DXVEC4();
	//Logger::Log(" ==== Getting lights ====");
	auto timer = TimeLogger();

	// create a map of all nearby valid lights and sort them per distance to player
	std::map<int, ShadowSceneLight*> SceneLights;
	NiTList<ShadowSceneLight>::Entry* Entry = SceneNode->lights.start;

	ShadowsExteriorEffect::InteriorsStruct* Settings = &Effects.ShadowsExteriors->Settings.Interiors;
	ShadowsExteriorEffect::ShadowStruct* ShadowsConstants = &Effects.ShadowsExteriors->Constants;

	// Creating list of lights in order of distance to the player
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
		bool inFront = D3DXVec4Dot(&LightVector, &TheRenderManager->CameraForward) > 0;
		float Distance = Light->GetDistance(&Player->pos);
		float radius = Light->Spec.r * Settings->LightRadiusMult;

		// select lights that will be tracked by removing culled lights and lights behind the player further away than their radius
		// TODO: handle using frustum check
		float drawDistance = 8000;//TheShaderManager->GameState.isExterior ? TheSettingManager->SettingsShadows.Exteriors.ShadowMapRadius[TheShadowManager->ShadowMapTypeEnum::MapLod] : TheSettingManager->SettingsShadows.Interiors.DrawDistance;
		if ((inFront || Distance < radius) && (Distance + radius) < drawDistance) {
			SceneLights[(int)(Distance * 10000)] = Entry->data; // multiplying distance (used as key) before conversion to avoid overwriting in case of similar values
		}

		Entry = Entry->next;
	}

	// save only the n first lights (based on #define TrackedLightsMax)
	memset(&TheShaderManager->LightPosition, 0, TrackedLightsMax * sizeof(D3DXVECTOR4)); // clear previous lights from array
	//memset(&ShadowsConstants->ShadowLightPosition, 0, ShadowCubeMapsMax * sizeof(D3DXVECTOR4)); // clear previous lights from array
	memset(&TheShaderManager->LightColor, 0, (TrackedLightsMax + ShadowCubeMapsMax) * sizeof(D3DXVECTOR4)); // clear previous lights from array

	// get the data for all tracked lights
	int ShadowIndex = 0;
	int LightIndex = 0;
	TheShadowManager->PointLightsNum = 0;

#if defined(OBLIVION)
	bool TorchOnBeltEnabled = TheSettingManager->SettingsMain.EquipmentMode.Enabled && TheSettingManager->SettingsMain.EquipmentMode.TorchKey != 255;
#endif

	D3DXVECTOR4 Empty = D3DXVECTOR4(0, 0, 0, 0);

	// TEMP : get data for spotlights. Right now, is done manually since spotlights aren't implemented in the engine
	TheShaderManager->Effects.Flashlight->UpdateConstants();
	if (TheShaderManager->Effects.Flashlight->Enabled && TheShaderManager->Effects.Flashlight->spotLightActive) {
		SpotLightList[0] = TheShaderManager->Effects.Flashlight->SpotLight;
	}
	else {
		SpotLightList[0] = nullptr;
	}

	//Setting constants
	if (SpotLightList[0] != nullptr) {
		TheShaderManager->SpotLightPosition[0] = SpotLightList[0]->m_worldTransform.pos.toD3DXVEC4();
		TheShaderManager->SpotLightPosition[0].w = SpotLightList[0]->Spec.r; // radius
		TheShaderManager->SpotLightDirection[0] = D3DXVECTOR4(
			SpotLightList[0]->m_worldTransform.rot.data[0][0],
			SpotLightList[0]->m_worldTransform.rot.data[1][0], 
			SpotLightList[0]->m_worldTransform.rot.data[2][0], 
			SpotLightList[0]->OuterSpotAngle); // outside angle of the light cone
		TheShaderManager->SpotLightColor[0] = D3DXVECTOR4(SpotLightList[0]->Diff.r, SpotLightList[0]->Diff.g, SpotLightList[0]->Diff.b, SpotLightList[0]->Dimmer);
	}
	else {
		TheShaderManager->SpotLightPosition[0] = Empty;
		TheShaderManager->SpotLightDirection[0] = Empty;
		TheShaderManager->SpotLightColor[0] = Empty;
	}

	std::map<int, ShadowSceneLight*>::iterator v = SceneLights.begin();
	for (int i = 0; i < TrackedLightsMax + ShadowCubeMapsMax; i++) {
		// set null values if we reached the end of lights in the scene and current index is lower than max amount
		if (v == SceneLights.end()) {
			if (ShadowIndex < ShadowCubeMapsMax) {
				//Logger::Log("clearing shadow casting light at index %i", ShadowIndex);
				ShadowLightsList[ShadowIndex] = NULL;
				ShadowsConstants->ShadowLightPosition[ShadowIndex] = Empty;
				LightColor[ShadowIndex] = Empty;
				ShadowIndex++;
			}
			if (LightIndex < TrackedLightsMax) {
				//Logger::Log("clearing light at index %i", LightIndex);
				LightsList[LightIndex] = NULL;
				LightPosition[LightIndex] = Empty;
				LightColor[ShadowCubeMapsMax + LightIndex] = Empty;
				LightIndex++;
			}

			continue;
		}

		NiPointLight* Light = v->second->sourceLight;
		if (!Light) {
			v++;
			continue;
		}

		if (Light->EffectType == NiDynamicEffect::EffectTypes::POINT_LIGHT) {
			// determin if light is a shadow caster
			//bool CastShadow = Settings->UseCastShadowFlag ? Light->CastShadows : true; // Flag is broken by JIP
			bool CastShadow = true;

#if defined(OBLIVION)
			// Oblivion exception for carried torch lights 
			if (TorchOnBeltEnabled && Light->CanCarry == 2) {
				HighProcessEx* Process = (HighProcessEx*)Player->process;
				if (Process->OnBeltState == HighProcessEx::State::In) CastShadow = false;
		}
#endif
			float radius = Light->Spec.r * Settings->LightRadiusMult;
			D3DXVECTOR4 LightPos = Light->m_worldTransform.pos.toD3DXVEC4();
			LightPos.w = radius;

			if (CastShadow && ShadowIndex < ShadowCubeMapsMax && radius > 10) {
				// add found light to list of lights that cast shadows
				ShadowLightsList[ShadowIndex] = v->second;
				ShadowsConstants->ShadowLightPosition[ShadowIndex] = LightPos;
				LightColor[ShadowIndex] = D3DXVECTOR4(Light->Diff.r, Light->Diff.g, Light->Diff.b, Light->Dimmer);

				ShadowIndex++;
				TheShadowManager->PointLightsNum++; // Constant to track number of shadow casting lights are present
			}
			else if (LightIndex < TrackedLightsMax) {
				LightsList[LightIndex] = Light;
				LightPosition[LightIndex] = LightPos;
				LightColor[ShadowCubeMapsMax + LightIndex] = D3DXVECTOR4(Light->Diff.r, Light->Diff.g, Light->Diff.b, Light->Dimmer);
				LightIndex++;
			};
		}
		else if (Light->EffectType == NiDynamicEffect::EffectTypes::SPOT_LIGHT) {
			// Here will go the collecting of the spotlights and setting of constants
		}
		v++;
	}

	timer.LogTime("ShaderManager::GetNearbyLights");
}


bool ShaderManager::ShouldRenderShadowMaps() {
	if (GameState.isExterior)
		return orthoRequired || (
			Effects.ShadowsExteriors->Settings.Exteriors.Enabled &&
			Effects.ShadowsExteriors->Enabled);
	else
		return Effects.ShadowsInteriors->Enabled;
}

/*
* Renders a given effect to an arbitrary render target
*/
void ShaderManager::RenderEffectToRT(IDirect3DSurface9* RenderTarget, EffectRecord* Effect, bool clearRenderTarget) {
	IDirect3DDevice9* Device = TheRenderManager->device;
	Device->SetRenderTarget(0, RenderTarget);
	Effect->Render(Device, RenderTarget, RenderTarget, 0, clearRenderTarget, RenderTarget);
};


void ShaderManager::RenderEffectsPreTonemapping(IDirect3DSurface9* RenderTarget) {
	if (!TheSettingManager->SettingsMain.Main.RenderEffects) return; // Main toggle
	if (!Player->parentCell) return;
	if (GameState.OverlayIsOn) return; // disable all effects during terminal/lockpicking sequences because they bleed through the overlay

	auto timer = TimeLogger();

	IDirect3DDevice9* Device = TheRenderManager->device;
	IDirect3DSurface9* SourceSurface = TheTextureManager->SourceSurface;
	IDirect3DSurface9* RenderedSurface = TheTextureManager->RenderedSurface;

	// prepare device for effects
	Device->SetStreamSource(0, FrameVertex, 0, sizeof(FrameVS));
	Device->SetFVF(FrameFVF);

	// render post process normals for use by shaders
	RenderEffectToRT(Effects.CombineDepth->Textures.CombinedDepthSurface, Effects.CombineDepth, false);
	RenderEffectToRT(Effects.Normals->Textures.NormalsSurface, Effects.Normals, false);

	// render a shadow pass for point lights
	if ((GameState.isExterior && Effects.ShadowsExteriors->Enabled) || (!GameState.isExterior && Effects.ShadowsInteriors->Enabled)) {
		// separate lights in 2 batches
		RenderEffectToRT(Effects.ShadowsExteriors->Textures.ShadowPassSurface, Effects.PointShadows, true);
		if (Effects.ShadowsExteriors->Settings.Interiors.LightPoints > 6) RenderEffectToRT(Effects.ShadowsExteriors->Textures.ShadowPassSurface, Effects.PointShadows2, false);
		if (GameState.isExterior) RenderEffectToRT(Effects.ShadowsExteriors->Textures.ShadowPassSurface, Effects.SunShadows, false);
	}

	Device->SetRenderTarget(0, RenderTarget);

	// copy the source render target to both the rendered and source textures (rendered gets updated after every pass, source once per effect)
	Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
	Device->StretchRect(RenderTarget, NULL, SourceSurface, NULL, D3DTEXF_NONE);

	if (GameState.isExterior) 
		Effects.ShadowsExteriors->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	else 
		Effects.ShadowsInteriors->Render(Device, RenderTarget, RenderedSurface, 0, true, SourceSurface);

	Effects.SnowAccumulation->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.AmbientOcclusion->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.WetWorld->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.Flashlight->Render(Device, RenderTarget, RenderedSurface, Effects.Flashlight->selectedPass, true, SourceSurface);
	Effects.Specular->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.Underwater->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.VolumetricFog->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.GodRays->Render(Device, RenderTarget, RenderedSurface, 0, true, SourceSurface);

	// calculate average luma for use by shaders
	if (avglumaRequired) {
		RenderEffectToRT(Effects.AvgLuma->Textures.AvgLumaSurface, Effects.AvgLuma, NULL);
		Device->SetRenderTarget(0, RenderTarget); 	// restore device used for effects
	}

	Effects.Exposure->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.Bloom->RenderBloomBuffer(RenderTarget);

	Effects.Lens->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	timer.LogTime("ShaderManager::RenderEffectsPreTonemapping");
}


/*
* Renders the effect that have been set to enabled.
*/
void ShaderManager::RenderEffects(IDirect3DSurface9* RenderTarget) {
	if (!TheSettingManager->SettingsMain.Main.RenderEffects) return; // Main toggle
	if (!Player->parentCell) return;
	if (GameState.OverlayIsOn) return; // disable all effects during terminal/lockpicking sequences because they bleed through the overlay

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

	Effects.Rain->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.Snow->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	//Effects.Linearization->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.BloomLegacy->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// screenspace coloring/blurring effects get rendered last
	Effects.Coloring->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.DepthOfField->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.MotionBlur->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// lens effects
	Effects.BloodLens->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.WaterLens->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.LowHF->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);
	Effects.Sharpening->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// cinema effect gets rendered very last because of vignetting/letterboxing
	Effects.Cinema->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// final adjustments
	Effects.ImageAdjust->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	// debug shader allows to display some of the buffers
	Effects.Debug->Render(Device, RenderTarget, RenderedSurface, 0, false, SourceSurface);

	//if (EffectsSettings->Extra) {
	//	for (EffectsList::iterator iter = Effects.ExtraEffects.begin(); iter != Effects.ExtraEffects.end(); ++iter) {
	//		if (iter->second->Enabled) {
	//			iter->second->Render(Device, RenderTarget, RenderedSurface, false, true);
	//		}
	//	}
	//}

	timer.LogTime("ShaderManager::RenderEffects");
}

EffectRecord* ShaderManager::GetEffectByName(const char* Name) {
	// effects
	EffectsList::iterator t = EffectsNames.find(Name);
	if (t == EffectsNames.end()) return nullptr;
	return *(t->second);
}


ShaderCollection* ShaderManager::GetShaderCollectionByName(const char* Name) {
	// shaders
	ShaderList::iterator t = ShaderNames.find(Name);
	if (t == ShaderNames.end()) return nullptr;
	return *(t->second);
}

/*
* Writes the settings corresponding to the shader/effect name, to switch it between enabled/disabled.*
* Also creates or deletes the corresponding Effect Record.
*/
void ShaderManager::SwitchShaderStatus(const char* Name) {
	IsMenuSwitch = true;

	// effects
	EffectRecord* effect = GetEffectByName(Name);
	if (effect) {
		bool setting = effect->SwitchEffect();
		TheSettingManager->SetMenuShaderEnabled(Name, setting);

		IsMenuSwitch = false;
		return;
	}

	// shaders
	ShaderCollection* shader = GetShaderCollectionByName(Name);
	if (shader) {
		bool setting = shader->SwitchShader();
		TheSettingManager->SetMenuShaderEnabled(Name, setting);

		IsMenuSwitch = false;
		return;
	}

	//else if (!strcmp(Name, "ExtraEffectsSettings")) { //TODO change to new effect switch
	//	EffectsSettings->Extra = !EffectsSettings->Extra;
	//	DisposeEffect(EffectRecord::EffectRecordType::Extra);
	//	if (EffectsSettings->Extra) CreateEffect(EffectRecord::EffectRecordType::Extra);
	//}
}

void ShaderManager::SetCustomConstant(const char* Name, D3DXVECTOR4 Value) {
	CustomConstants::iterator v = CustomConst.find(std::string(Name));
	if (v != CustomConst.end()) v->second = Value;
}

void ShaderManager::SetExtraEffectEnabled(const char* Name, bool Value) {
	//EffectsList::iterator v = Effects.ExtraEffects.find(std::string(Name));
	//if (v != Effects.ExtraEffects.end()) v->second->Enabled = Value;
}