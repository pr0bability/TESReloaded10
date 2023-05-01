/*
* Constructor of Animator class. Starts an animator for a given value.
*/
Animator::Animator() {
	time = TimeGlobals::Get();
	startValue = 0;
	endValue = 0;
	startTime = 0;
	endTime = 0;
	running = false;
	switched = false;
};


Animator::~Animator() {
};


void Animator::Initialize(float value) {
	startValue = value;
	endValue = value;
}

/*
* Starts the animator by setting a target value and a duration to reach it.
*/
void Animator::Start(float duration, float finalValue) {
	float currenttime = time->GameHour->data;

	startTime = currenttime;
	endTime = currenttime + duration;
	running = true;

	startValue = endValue;
	endValue = finalValue;
}


/*
* Gets the value for the animated value at the current time.
*/
float Animator::GetValue() {
	float currenttime = time->GameHour->data;
	if (!running) return startValue;
	
	if (currenttime > endTime) {
		running = false;
		startValue = endValue;
		return endValue;
	}

	running = true;
	currenttime = ShaderManager::invLerp(startTime, endTime, currenttime);
	return ShaderManager::lerp(startValue, endValue, currenttime);
};


ShaderProgram::ShaderProgram() {

	FloatShaderValues = NULL;
	TextureShaderValues = NULL;
	FloatShaderValuesCount = 0;
	TextureShaderValuesCount = 0;

}

ShaderProgram::~ShaderProgram() {

	if (FloatShaderValues) free(FloatShaderValues);
	if (TextureShaderValues) free(TextureShaderValues);

}

/* 
Declares the constants that can be accessed from the shader code from the Constant Table, and gives them a readable name.
*/
void ShaderProgram::SetConstantTableValue(LPCSTR Name, UInt32 Index) {
	
	if (!strcmp(Name, "TESR_ToneMapping"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.HDR.ToneMapping;
	else if (!strcmp(Name, "TESR_ParallaxData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.POM.ParallaxData;
	else if (!strcmp(Name, "TESR_GrassScale"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Grass.Scale;
	else if (!strcmp(Name, "TESR_TerrainData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Terrain.Data;
	else if (!strcmp(Name, "TESR_SkinData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Skin.SkinData;
	else if (!strcmp(Name, "TESR_SkinColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Skin.SkinColor;
	else if (!strcmp(Name, "TESR_ShadowData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Shadow.Data;
	else if (!strcmp(Name, "TESR_ShadowScreenSpaceData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Shadow.ScreenSpaceData;
	else if (!strcmp(Name, "TESR_ShadowRadius"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.ShadowMap.ShadowMapRadius;
	else if (!strcmp(Name, "TESR_OrthoData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Shadow.OrthoData;
	else if (!strcmp(Name, "TESR_RainData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Rain.RainData;
	else if (!strcmp(Name, "TESR_RainAspect"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Rain.RainAspect;
	else if (!strcmp(Name, "TESR_SnowData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Snow.SnowData;
	else if (!strcmp(Name, "TESR_WorldTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheRenderManager->worldMatrix;
	else if (!strcmp(Name, "TESR_ViewTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheRenderManager->viewMatrix;
	else if (!strcmp(Name, "TESR_ProjectionTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheRenderManager->projMatrix;
	else if (!strcmp(Name, "TESR_InvProjectionTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheRenderManager->InvProjMatrix;
	else if (!strcmp(Name, "TESR_WorldViewProjectionTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheRenderManager->WorldViewProjMatrix;
	else if (!strcmp(Name, "TESR_InvViewProjectionTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheRenderManager->InvViewProjMatrix;
	else if (!strcmp(Name, "TESR_ViewProjectionTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheRenderManager->ViewProjMatrix;
	else if (!strcmp(Name, "TESR_ViewSpaceLightDir"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ViewSpaceLightDir;
	else if (!strcmp(Name, "TESR_ScreenSpaceLightDir"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ScreenSpaceLightDir;
	else if (!strcmp(Name, "TESR_ShadowWorldTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowWorld;
	else if (!strcmp(Name, "TESR_ShadowViewProjTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowViewProj;
	else if (!strcmp(Name, "TESR_ShadowCameraToLightTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight;
	else if (!strcmp(Name, "TESR_ShadowCameraToLightTransformNear"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[0];
	else if (!strcmp(Name, "TESR_ShadowCameraToLightTransformMiddle"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[1];
	else if (!strcmp(Name, "TESR_ShadowCameraToLightTransformFar"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[2];
	else if (!strcmp(Name, "TESR_ShadowCameraToLightTransformLod"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[3];
	else if (!strcmp(Name, "TESR_ShadowCameraToLightTransformOrtho"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowCameraToLight[4];
	else if (!strcmp(Name, "TESR_ShadowCubeMapLightPosition"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.ShadowMap.ShadowCubeMapLightPosition;
	else if (!strcmp(Name, "TESR_ShadowLightPosition"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition;
	else if (!strcmp(Name, "TESR_ShadowLightPosition0"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[0];
	else if (!strcmp(Name, "TESR_ShadowLightPosition1"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[1];
	else if (!strcmp(Name, "TESR_ShadowLightPosition2"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[2];
	else if (!strcmp(Name, "TESR_ShadowLightPosition3"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[3];
	else if (!strcmp(Name, "TESR_ShadowLightPosition4"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[4];
	else if (!strcmp(Name, "TESR_ShadowLightPosition5"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[5];
	else if (!strcmp(Name, "TESR_ShadowLightPosition6"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[6];
	else if (!strcmp(Name, "TESR_ShadowLightPosition7"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[7];
	else if (!strcmp(Name, "TESR_ShadowLightPosition8"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[8];
	else if (!strcmp(Name, "TESR_ShadowLightPosition9"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[9];
	else if (!strcmp(Name, "TESR_ShadowLightPosition10"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[10];
	else if (!strcmp(Name, "TESR_ShadowLightPosition11"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowMap.ShadowLightPosition[11];
	else if (!strcmp(Name, "TESR_LightPosition0"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightPosition[0];
	else if (!strcmp(Name, "TESR_LightPosition1"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightPosition[1];
	else if (!strcmp(Name, "TESR_LightPosition2"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightPosition[2];
	else if (!strcmp(Name, "TESR_LightPosition3"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightPosition[3];
	else if (!strcmp(Name, "TESR_LightPosition4"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightPosition[4];
	else if (!strcmp(Name, "TESR_LightPosition5"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightPosition[5];
	else if (!strcmp(Name, "TESR_LightPosition6"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightPosition[6];
	else if (!strcmp(Name, "TESR_LightPosition7"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightPosition[7];
	else if (!strcmp(Name, "TESR_LightAttenuation0"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[0];
	else if (!strcmp(Name, "TESR_LightAttenuation1"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[1];
	else if (!strcmp(Name, "TESR_LightAttenuation2"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[2];
	else if (!strcmp(Name, "TESR_LightAttenuation3"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[3];
	else if (!strcmp(Name, "TESR_LightAttenuation4"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[4];
	else if (!strcmp(Name, "TESR_LightAttenuation5"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[5];
	else if (!strcmp(Name, "TESR_LightAttenuation6"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[6];
	else if (!strcmp(Name, "TESR_LightAttenuation7"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->LightAttenuation[7];
	else if (!strcmp(Name, "TESR_ShadowCubeMapBlend"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.ShadowMap.ShadowCubeMapBlend;
	else if (!strcmp(Name, "TESR_OcclusionWorldViewProjTransform"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.OcclusionMap.OcclusionWorldViewProj;
	else if (!strcmp(Name, "TESR_ReciprocalResolution"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.ReciprocalResolution;
	else if (!strcmp(Name, "TESR_CameraForward"))
		FloatShaderValues[Index].Value = &TheRenderManager->CameraForward;
	else if (!strcmp(Name, "TESR_DepthConstants"))
		FloatShaderValues[Index].Value = &TheRenderManager->DepthConstants;
	else if (!strcmp(Name, "TESR_CameraData"))
		FloatShaderValues[Index].Value = &TheRenderManager->CameraData;
	else if (!strcmp(Name, "TESR_CameraPosition"))
		FloatShaderValues[Index].Value = &TheRenderManager->CameraPosition;
	else if (!strcmp(Name, "TESR_SunDirection"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.SunDir;
	else if (!strcmp(Name, "TESR_SunTiming"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.SunTiming;
	else if (!strcmp(Name, "TESR_SunAmount"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.SunAmount;
	else if (!strcmp(Name, "TESR_ShadowFade"))
		FloatShaderValues[Index].Value = (D3DXVECTOR4*)&TheShaderManager->ShaderConst.ShadowFade;
	else if (!strcmp(Name, "TESR_GameTime"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.GameTime;
	else if (!strcmp(Name, "TESR_WaterCoefficients"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Water.waterCoefficients;
	else if (!strcmp(Name, "TESR_WaveParams"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Water.waveParams;
	else if (!strcmp(Name, "TESR_WaterVolume"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Water.waterVolume;
	else if (!strcmp(Name, "TESR_WaterSettings"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Water.waterSettings;
	else if (!strcmp(Name, "TESR_WaterDeepColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Water.deepColor;
	else if (!strcmp(Name, "TESR_WaterShallowColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Water.shallowColor;
	else if (!strcmp(Name, "TESR_WaterShorelineParams"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Water.shorelineParams;
	else if (!strcmp(Name, "TESR_FogColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.fogColor;
	else if (!strcmp(Name, "TESR_HorizonColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.horizonColor;
	else if (!strcmp(Name, "TESR_SunColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.sunColor;
	else if (!strcmp(Name, "TESR_SkyColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.skyColor;
	else if (!strcmp(Name, "TESR_SunAmbient"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.sunColor;
	else if (!strcmp(Name, "TESR_FogData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.fogData;
	else if (!strcmp(Name, "TESR_FogDistance"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.fogDistance;
	else if (!strcmp(Name, "TESR_AmbientOcclusionAOData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.AmbientOcclusion.AOData;
	else if (!strcmp(Name, "TESR_AmbientOcclusionData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.AmbientOcclusion.Data;
	else if (!strcmp(Name, "TESR_BloodLensParams"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.BloodLens.Params;
	else if (!strcmp(Name, "TESR_BloodLensColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.BloodLens.BloodColor;
	else if (!strcmp(Name, "TESR_BloomData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Bloom.BloomData;
	else if (!strcmp(Name, "TESR_BloomValues"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Bloom.BloomValues;
	else if (!strcmp(Name, "TESR_CinemaData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Cinema.Data;
	else if (!strcmp(Name, "TESR_CinemaSettings"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Cinema.Settings;
	else if (!strcmp(Name, "TESR_ColoringColorCurve"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Coloring.ColorCurve;
	else if (!strcmp(Name, "TESR_ColoringEffectGamma"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Coloring.EffectGamma;
	else if (!strcmp(Name, "TESR_ColoringData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Coloring.Data;
	else if (!strcmp(Name, "TESR_ColoringValues"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Coloring.Values;
	else if (!strcmp(Name, "TESR_DepthOfFieldBlur"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.DepthOfField.Blur;
	else if (!strcmp(Name, "TESR_DepthOfFieldData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.DepthOfField.Data;
	else if (!strcmp(Name, "TESR_ExposureData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Exposure.Data;
	else if (!strcmp(Name, "TESR_GodRaysRay"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.GodRays.Ray;
	else if (!strcmp(Name, "TESR_GodRaysRayColor"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.GodRays.RayColor;
	else if (!strcmp(Name, "TESR_GodRaysData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.GodRays.Data;
	else if (!strcmp(Name, "TESR_LowHFData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.LowHF.Data;
	else if (!strcmp(Name, "TESR_MotionBlurParams"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.MotionBlur.BlurParams;
	else if (!strcmp(Name, "TESR_MotionBlurData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.MotionBlur.Data;
	else if (!strcmp(Name, "TESR_SharpeningData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Sharpening.Data;
	else if (!strcmp(Name, "TESR_SpecularData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Specular.Data;
	else if (!strcmp(Name, "TESR_SpecularEffects"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.Specular.EffectStrength;
	else if (!strcmp(Name, "TESR_SnowAccumulationParams"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.SnowAccumulation.Params;
	else if (!strcmp(Name, "TESR_VolumetricFogData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.VolumetricFog.Data;
	else if (!strcmp(Name, "TESR_WaterLensData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.WaterLens.Time;
	else if (!strcmp(Name, "TESR_WetWorldCoeffs"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.WetWorld.Coeffs;
	else if (!strcmp(Name, "TESR_WetWorldData"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.WetWorld.Data;
	else if (!strcmp(Name, "TESR_DebugVar"))
		FloatShaderValues[Index].Value = &TheShaderManager->ShaderConst.DebugVar;
	else {
		Logger::Log("Custom constant found: %s", Name);
		D3DXVECTOR4 v; v.x = v.y = v.z = v.w = 0.0f;
		TheShaderManager->CustomConst[Name] = v;
		FloatShaderValues[Index].Value = &TheShaderManager->CustomConst[Name];
	}
}

ShaderRecord::ShaderRecord() {
	
	HasRenderedBuffer = false;
	HasDepthBuffer = false;

}
ShaderRecord::~ShaderRecord() {}

/**
* @param fileBin the name of the compiled shader file
* @param fileHlsl the name of the hlsl source file for this shader
* @param CompileStatus an integer for the status of the compilation. If set to 2, will compare file dates to return status. 
* @returns wether the shader should be compiled, from a given binary shader and corresponding hlsl.
*/
bool ShaderProgram::ShouldCompileShader(const char* fileBin, const char* fileHlsl, ShaderCompileType CompileStatus){
	if(CompileStatus == ShaderCompileType::AlwaysOn) return  true;
	if(CompileStatus == ShaderCompileType::AlwaysOff) return  false;
	if(CompileStatus == ShaderCompileType::RecompileInMenu) return TheShaderManager->IsMenuSwitch ? true : false;

	if(CompileStatus == ShaderCompileType::RecompileChanged) {
		WIN32_FILE_ATTRIBUTE_DATA attributesBin = {0};
		WIN32_FILE_ATTRIBUTE_DATA attributesSource = {0};
		BOOL hr = GetFileAttributesExA(fileBin, GetFileExInfoStandard, &attributesBin); // from winbase.h
		if(hr == FALSE) {
// 			Logger::Log("File %s not present, compile", fileHlsl);
			return true; //File not present compile
		}
		else{
			BOOL hr = GetFileAttributesExA(fileHlsl, GetFileExInfoStandard, &attributesSource); // from winbase.h
			if(hr == FALSE) {
//				Logger::Log("[ERROR] Can't open source %s", fileHlsl);
				return true; //BOH
			}

			ULARGE_INTEGER timeBin, timeSource;
			timeBin.LowPart = attributesBin.ftLastWriteTime.dwLowDateTime;
			timeBin.HighPart = attributesBin.ftLastWriteTime.dwHighDateTime;
			timeSource.LowPart = attributesSource.ftLastWriteTime.dwLowDateTime;
			timeSource.HighPart = attributesSource.ftLastWriteTime.dwHighDateTime;

			if(timeBin.QuadPart < timeSource.QuadPart){
				Logger::Log("Binary older then source, compile %s", fileHlsl);
				return true;
			}
		}
	}
	return false;
}

/* 
Loads the shader by name from a given subfolder (optionally). Shader will be compiled if needed.
@returns the ShaderRecord for this shader.
*/
ShaderRecord* ShaderRecord::LoadShader(const char* Name, const char* SubPath) {
	auto timer = TimeLogger();

	ShaderRecord* ShaderProg = NULL;
	ID3DXBuffer* ShaderSource = NULL;
	ID3DXBuffer* Shader = NULL;
	ID3DXBuffer* Errors = NULL;
	ID3DXConstantTable* ConstantTable = NULL;
	void* Function = NULL;
	UInt32 SourceSize = 0;
	char ShaderProfile[7];
	char FileName[MAX_PATH];
	char FileNameBinary[MAX_PATH];

	strcpy(FileName, ShadersPath);
	if (!memcmp(Name, "WATER", 5)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Water")) return NULL;
	}
	else if (!memcmp(Name, "GRASS", 5)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Grass")) return NULL;
	}
	else if (!memcmp(Name, "HDR", 3) || !memcmp(Name, "ISHDR", 5)) {
		// load tonemapping shaders, with different names between New vegas and Oblivion
		if (!TheSettingManager->GetMenuShaderEnabled("HDR")) return NULL;
	}
	else if (!memcmp(Name, "PAR", 3)) {
		if (!TheSettingManager->GetMenuShaderEnabled("POM")) return NULL;
	}
	else if (!memcmp(Name, "SKIN", 4)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Skin")) return NULL;
	}
	else if (strstr(TerrainShaders, Name)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Terrain")) return NULL;
	}
	else if (strstr(BloodShaders, Name)) {
		if (!TheSettingManager->GetMenuShaderEnabled("Blood")) return NULL;
	}
	else if (!memcmp(Name, "NIGHTEYE", 8)) {
		if (!TheSettingManager->GetMenuShaderEnabled("NightEye")) return NULL;
	}
	else if (!memcmp(Name, "Shadow", 6)) {
		strcat(FileName, "Shadows\\");
	}
	else if (!memcmp(Name, "Occlusion", 9)) {
		strcat(FileName, "Occlusion\\");
	}
	else if (!memcmp(Name, "Bink", 4)) {
		strcat(FileName, "Bink\\");
	}
	else if(!TheSettingManager->SettingsMain.Shaders.Extra) return NULL;
    
	if (SubPath) strcat(FileName, SubPath);
	strcat(FileName, Name);
	strcpy(FileNameBinary, FileName);
	strcat(FileName, ".hlsl");

    HRESULT prepass = D3DXPreprocessShaderFromFileA(FileName, NULL, NULL, &ShaderSource , &Errors);
	bool Compile = ShouldCompileShader(FileNameBinary, FileName, (ShaderCompileType) TheSettingManager->SettingsMain.Develop.CompileShaders);
	if (prepass == D3D_OK) {
		if (strstr(Name, ".vso"))
			strcpy(ShaderProfile, "vs_3_0");
		else if (strstr(Name, ".pso"))
			strcpy(ShaderProfile, "ps_3_0");
		if (Compile) {
			D3DXCompileShaderFromFileA(FileName, NULL, NULL, "main", ShaderProfile, NULL, &Shader, &Errors, &ConstantTable);
			if (Errors) Logger::Log((char*)Errors->GetBufferPointer());
			if (Shader) {
				Function = Shader->GetBufferPointer();
				std::ofstream FileBinary(FileNameBinary, std::ios::out | std::ios::binary);
				FileBinary.write((const char*)Function, Shader->GetBufferSize());
				FileBinary.flush();
				FileBinary.close();
				Logger::Log("Shader compiled: %s", FileName);
			}
		}
		else {
			std::ifstream FileBinary(FileNameBinary, std::ios::in | std::ios::binary | std::ios::ate);
			if (FileBinary.is_open()) {
				std::streamoff Size = FileBinary.tellg();
				D3DXCreateBuffer(Size, &Shader);
				FileBinary.seekg(0, std::ios::beg);
				Function = Shader->GetBufferPointer();
				FileBinary.read((char*)Function, Size);
				FileBinary.close();
				D3DXGetShaderConstantTable((const DWORD*)Function, &ConstantTable);
			}
			else {
				Logger::Log("ERROR: Shader %s not found. Try to enable the CompileShader option to recompile the shaders.", FileNameBinary);
			}
		}

		timer.LogTime("ShaderRecord::LoadShader");

		if (Shader) {
			if (ShaderProfile[0] == 'v') {
				ShaderProg = new ShaderRecordVertex();
				TheRenderManager->device->CreateVertexShader((const DWORD*)Function, &((ShaderRecordVertex*)ShaderProg)->ShaderHandle);
			}
			else {
				ShaderProg = new ShaderRecordPixel();
				TheRenderManager->device->CreatePixelShader((const DWORD*)Function, &((ShaderRecordPixel*)ShaderProg)->ShaderHandle);
			}
			ShaderProg->CreateCT(ShaderSource, ConstantTable);
			Logger::Log("Shader loaded: %s", FileNameBinary);
		}
	}
	else {
		if (Errors) Logger::Log((char*)Errors->GetBufferPointer());
	}

	if (ShaderSource) ShaderSource->Release();
	if (Shader) Shader->Release();
	if (Errors) Errors->Release();
	return ShaderProg;
}

/**
* Creates the constants table for the Shader.
* @param ShaderSource
* @param ConstantTable
*/
void ShaderRecord::CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable) {


	D3DXCONSTANTTABLE_DESC ConstantTableDesc;
	D3DXCONSTANT_DESC ConstantDesc;
	D3DXHANDLE Handle;
	UINT ConstantCount = 1;
	UInt32 FloatIndex = 0;
	UInt32 TextureIndex = 0;

	ConstantTable->GetDesc(&ConstantTableDesc);
    for (UINT c = 0; c < ConstantTableDesc.Constants; c++) {
		Handle = ConstantTable->GetConstant(NULL, c);
		ConstantTable->GetConstantDesc(Handle, &ConstantDesc, &ConstantCount);
		if (ConstantDesc.RegisterSet == D3DXRS_FLOAT4 && !memcmp(ConstantDesc.Name, "TESR_", 5)) FloatShaderValuesCount += 1;
		if (ConstantDesc.RegisterSet == D3DXRS_SAMPLER && !memcmp(ConstantDesc.Name, "TESR_", 5)) TextureShaderValuesCount += 1;
    }

	auto timer = TimeLogger();
	if (FloatShaderValuesCount) FloatShaderValues = (ShaderValue*)malloc(FloatShaderValuesCount * sizeof(ShaderValue));
	if (TextureShaderValuesCount) TextureShaderValues = (ShaderValue*)malloc(TextureShaderValuesCount * sizeof(ShaderValue));
	timer.LogTime("ShaderRecord::createCT Malloc");

	//Logger::Log("CreateCT: Shader has %i constants", ConstantTableDesc.Constants);

	for (UINT c = 0; c < ConstantTableDesc.Constants; c++) {
		Handle = ConstantTable->GetConstant(NULL, c);
		ConstantTable->GetConstantDesc(Handle, &ConstantDesc, &ConstantCount);
		if (!memcmp(ConstantDesc.Name, "TESR_", 5)) {
			switch (ConstantDesc.RegisterSet) {
				case D3DXRS_FLOAT4:
					SetConstantTableValue(ConstantDesc.Name, FloatIndex);
					FloatShaderValues[FloatIndex].RegisterIndex = ConstantDesc.RegisterIndex;
					FloatShaderValues[FloatIndex].RegisterCount = ConstantDesc.RegisterCount;
					FloatIndex++;
 					break;
				case D3DXRS_SAMPLER:
					TextureShaderValues[TextureIndex].Texture = TheTextureManager->LoadTexture(ShaderSource, ConstantDesc.Type, ConstantDesc.Name, ConstantDesc.RegisterIndex, &HasRenderedBuffer, &HasDepthBuffer);
					TextureShaderValues[TextureIndex].RegisterIndex = ConstantDesc.RegisterIndex;
					TextureShaderValues[TextureIndex].RegisterCount = 1;
					TextureIndex++;
					break;
			}
		}
	}

	timer.LogTime("ShaderRecord::createCT Done");
}

/* 
* Sets the Constant Table for the shader
*/
void ShaderRecord::SetCT() {
	ShaderValue* Value;

	if (HasRenderedBuffer) TheRenderManager->device->StretchRect(TheRenderManager->currentRTGroup->RenderTargets[0]->data->Surface, NULL, TheTextureManager->RenderedSurface, NULL, D3DTEXF_NONE);
	if (HasDepthBuffer) TheRenderManager->ResolveDepthBuffer();
	for (UInt32 c = 0; c < TextureShaderValuesCount; c++) {
		Value = &TextureShaderValues[c];
		if (Value->Texture->Texture) TheRenderManager->renderState->SetTexture(Value->RegisterIndex, Value->Texture->Texture);
		for (int i = 1; i < SamplerStatesMax; i++) {
			TheRenderManager->SetSamplerState(Value->RegisterIndex, (D3DSAMPLERSTATETYPE)i, Value->Texture->SamplerStates[i]);
		}
	}
	for (UInt32 c = 0; c < FloatShaderValuesCount; c++) {
		Value = &FloatShaderValues[c];
		SetShaderConstantF(Value->RegisterIndex, Value->Value, Value->RegisterCount);
	}
}


ShaderRecordVertex::ShaderRecordVertex() {
	
	ShaderHandle = NULL;

}

ShaderRecordVertex::~ShaderRecordVertex() {

	ShaderHandle->Release();

}

ShaderRecordPixel::ShaderRecordPixel() {
	
	ShaderHandle = NULL;

}

ShaderRecordPixel::~ShaderRecordPixel() {
	
	ShaderHandle->Release();

}


void ShaderRecordVertex::SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount) {
	
	TheRenderManager->device->SetVertexShaderConstantF(RegisterIndex, (const float*)Value, RegisterCount);

}

void ShaderRecordPixel::SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount) {
	
	TheRenderManager->device->SetPixelShaderConstantF(RegisterIndex, (const float*)Value, RegisterCount);

}

/*
* Class that wraps an effect shader, in order to load it/render it/set constants.
*/
EffectRecord::EffectRecord() {

	Enabled = false;
	Effect = NULL;

}
/*Shader Values arrays are freed in the superclass Destructor*/
EffectRecord::~EffectRecord() {
	if (Effect) Effect->Release();
	delete Path;
	delete SourcePath;
}

/*
 * Unload effects, allowing it to be reloaded from  a blank state.
 */
void EffectRecord::DisposeEffect(){
	if (Effect) Effect->Release();
	Effect = nullptr;
	if (FloatShaderValues) free(FloatShaderValues);
	FloatShaderValues = nullptr;
	if (TextureShaderValues) free(TextureShaderValues);
	TextureShaderValues = nullptr;
	Enabled = false;
}

/*
 * Compile and Load the Effect shader
 */
bool EffectRecord::LoadEffect(bool alwaysCompile){
	auto timer = TimeLogger();

	ID3DXBuffer* ShaderSource = NULL;
	ID3DXBuffer* Errors = NULL;
	ID3DXEffect* Effect = NULL;
	bool success = false;
    HRESULT prepass = D3DXPreprocessShaderFromFileA(SourcePath->data(), NULL, NULL, &ShaderSource , &Errors);
	ID3DXEffectCompiler* Compiler = NULL;
	ID3DXBuffer* EffectBuffer = NULL;
	HRESULT load = NULL;
	if(alwaysCompile || ShouldCompileShader(Path->data(), SourcePath->data(), (ShaderCompileType)TheSettingManager->SettingsMain.Develop.CompileEffects) ){
		HRESULT comp  = D3DXCreateEffectCompilerFromFileA(SourcePath->data(), NULL, NULL, NULL, &Compiler, &Errors);
		if(FAILED(comp)) goto cleanup;
		if (Errors){
			Logger::Log((char*)Errors->GetBufferPointer());
			Errors->Release();
			Errors = nullptr;
		}

		HRESULT compiled = Compiler->CompileEffect(NULL, &EffectBuffer, &Errors);
		if(FAILED(compiled)) goto cleanup;
		if (Errors){
			Logger::Log((char*)Errors->GetBufferPointer());
			Errors->Release();
			Errors = nullptr;
		}

		if (EffectBuffer) {
			std::ofstream FileBinary(Path->data(), std::ios::out | std::ios::binary);
			FileBinary.write((char*)EffectBuffer->GetBufferPointer(), EffectBuffer->GetBufferSize());
			FileBinary.flush();
			FileBinary.close();
			Logger::Log("Effect compiled: %s", SourcePath->data());
		}
	}
	load = D3DXCreateEffectFromFileA(TheRenderManager->device, Path->data(), NULL, NULL, NULL, NULL, &Effect, &Errors);
	if(FAILED(load)) goto cleanup;

	if (Errors) Logger::Log((char*)Errors->GetBufferPointer()); // LAst can be cleaned in the cleanup section
	if (Effect) {
		this->Effect = Effect;
		CreateCT(ShaderSource, NULL); //Recreate CT;
		Logger::Log("Effect loaded: %s", Path->data());
	}

	success = true;
cleanup:
	if (EffectBuffer) EffectBuffer->Release();
	if (Compiler) Compiler->Release();

	if (ShaderSource) ShaderSource->Release();
	if (Errors){
		Logger::Log((char*)Errors->GetBufferPointer());
		Errors->Release();
	}

	timer.LogTime("EffectRecord::LoadSEffect");

	return success;
}

/**
* Loads an effect shader by name (The post process effects stored in the Effects folder)
* @param Name the name for the effect
* @returns an EffectRecord describing the effect shader.
*/
EffectRecord* EffectRecord::LoadEffect(const char* Name) {
	
	char FileName[MAX_PATH];

	strcpy(FileName, Name);
	strcat(FileName, ".hlsl");
	EffectRecord* EffectProg = new EffectRecord();
	EffectProg->Path = new std::string(Name); //TODO pass them to constructor for clean code 
	EffectProg->SourcePath = new std::string(FileName);
	EffectProg->LoadEffect();
	return EffectProg;
}

bool EffectRecord::IsLoaded(){
	return Effect != nullptr; 
}

/**
Creates the Constant Table for the Effect Record. 
*/
void EffectRecord::CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable) {
	auto timer = TimeLogger();

	D3DXEFFECT_DESC ConstantTableDesc;
	D3DXPARAMETER_DESC ConstantDesc;
	D3DXHANDLE Handle;
	UINT ConstantCount = 1;
	UInt32 FloatIndex = 0;
	UInt32 TextureIndex = 0;

	Effect->GetDesc(&ConstantTableDesc);
	for (UINT c = 0; c < ConstantTableDesc.Parameters; c++) {
		Handle = Effect->GetParameter(NULL, c);
		Effect->GetParameterDesc(Handle, &ConstantDesc);
		if ((ConstantDesc.Class == D3DXPC_VECTOR || ConstantDesc.Class == D3DXPC_MATRIX_ROWS) && !memcmp(ConstantDesc.Name, "TESR_", 5)) FloatShaderValuesCount += 1;
		if (ConstantDesc.Class == D3DXPC_OBJECT && ConstantDesc.Type >= D3DXPT_SAMPLER && ConstantDesc.Type <= D3DXPT_SAMPLERCUBE && !memcmp(ConstantDesc.Name, "TESR_", 5)) TextureShaderValuesCount += 1;
	}
	if (FloatShaderValuesCount) FloatShaderValues = (ShaderValue*)malloc(FloatShaderValuesCount * sizeof(ShaderValue));
	if (TextureShaderValuesCount) TextureShaderValues = (ShaderValue*)malloc(TextureShaderValuesCount * sizeof(ShaderValue));

	Logger::Log("CreateCT: Effect has %i constants", ConstantTableDesc.Parameters);

	for (UINT c = 0; c < ConstantTableDesc.Parameters; c++) {
		Handle = Effect->GetParameter(NULL, c);
		Effect->GetParameterDesc(Handle, &ConstantDesc);
		if (!memcmp(ConstantDesc.Name, "TESR_", 5)) {
			switch (ConstantDesc.Class) {
				case D3DXPC_VECTOR:
				case D3DXPC_MATRIX_ROWS:
					SetConstantTableValue(ConstantDesc.Name, FloatIndex);
					FloatShaderValues[FloatIndex].RegisterIndex = (UInt32)Handle;
					FloatShaderValues[FloatIndex].RegisterCount = ConstantDesc.Rows;
					FloatIndex++;
					break;
				case D3DXPC_OBJECT:
					if (ConstantDesc.Class == D3DXPC_OBJECT && ConstantDesc.Type >= D3DXPT_SAMPLER && ConstantDesc.Type <= D3DXPT_SAMPLERCUBE) {
						TextureShaderValues[TextureIndex].Texture = TheTextureManager->LoadTexture(ShaderSource, ConstantDesc.Type, ConstantDesc.Name, TextureIndex, NULL, NULL);
						TextureShaderValues[TextureIndex].RegisterIndex = TextureIndex;
						TextureShaderValues[TextureIndex].RegisterCount = 1;
						TextureIndex++;
					}
					break;
			}
		}
	}

	timer.LogTime("EffectRecord::ConstantTableDesc");
}

/*
*Sets the Effect Shader constants table and texture registers.
*/
void EffectRecord::SetCT() {
	ShaderValue* Value;
	if (!Enabled || Effect == nullptr) return;
	for (UInt32 c = 0; c < TextureShaderValuesCount; c++) {
		Value = &TextureShaderValues[c];
		if (Value->Texture->Texture) TheRenderManager->device->SetTexture(Value->RegisterIndex, Value->Texture->Texture);
		for (int i = 1; i < SamplerStatesMax; i++) {
			TheRenderManager->SetSamplerState(Value->RegisterIndex, (D3DSAMPLERSTATETYPE)i, Value->Texture->SamplerStates[i]);
		}
	}
	for (UInt32 c = 0; c < FloatShaderValuesCount; c++) {
		Value = &FloatShaderValues[c];
		if (Value->RegisterCount == 1)
			Effect->SetVector((D3DXHANDLE)Value->RegisterIndex, Value->Value);
		else
			Effect->SetMatrix((D3DXHANDLE)Value->RegisterIndex, (D3DXMATRIX*)Value->Value);
	}
}

/*
 * Enable or Disable Effect, with reloading it if it's changed on disk
 */
bool EffectRecord::SwitchEffect(){
	bool change = true;
	if (!IsLoaded() || (!Enabled && ShouldCompileShader(Path->data(), SourcePath->data(), ShaderCompileType::RecompileChanged)) ) {
		Logger::Log("Effect %s is not loaded", Path->data());
		DisposeEffect();
		change = LoadEffect(true);
	}
	if (change) {
		Enabled = !Enabled;
	}
	else {
		Logger::Log("Couldn't switch status of effect %s", Path->data());
	}
	return Enabled;
}

/**
* Renders the given effect shader.
*/
void EffectRecord::Render(IDirect3DDevice9* Device, IDirect3DSurface9* RenderTarget, IDirect3DSurface9* RenderedSurface, bool ClearRenderTarget, bool useSourceBuffer) {

	if (useSourceBuffer) Device->StretchRect(RenderTarget, NULL, TheTextureManager->SourceSurface, NULL, D3DTEXF_NONE);

	SetCT(); // update the constant table
	UINT Passes;
	if (!Enabled || Effect == nullptr) return;
	Effect->Begin(&Passes, NULL);
	for (UINT p = 0; p < Passes; p++) {
		if (ClearRenderTarget) Device->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L);
		Effect->BeginPass(p);
		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		Effect->EndPass();
		Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
	}
	Effect->End();
}

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
	TheShaderManager->EffectsNames["AvgLuma"] = &TheShaderManager->Effects.AvgLuma;
	TheShaderManager->EffectsNames["AmbientOcclusion"] = &TheShaderManager->Effects.AmbientOcclusion;
	TheShaderManager->EffectsNames["BloodLens"] = &TheShaderManager->Effects.BloodLens;
	TheShaderManager->EffectsNames["Bloom"] = &TheShaderManager->Effects.Bloom;
	TheShaderManager->EffectsNames["Coloring"] = &TheShaderManager->Effects.Coloring;
	TheShaderManager->EffectsNames["Cinema"] = &TheShaderManager->Effects.Cinema;
	TheShaderManager->EffectsNames["DepthOfField"] = &TheShaderManager->Effects.DepthOfField;
	TheShaderManager->EffectsNames["Exposure"] = &TheShaderManager->Effects.Exposure;
	TheShaderManager->EffectsNames["GodRays"] = &TheShaderManager->Effects.GodRays;
	TheShaderManager->EffectsNames["LowHF"] = &TheShaderManager->Effects.LowHF;
	TheShaderManager->EffectsNames["MotionBlur"] = &TheShaderManager->Effects.MotionBlur;
	TheShaderManager->EffectsNames["Normals"] = &TheShaderManager->Effects.Normals;
	TheShaderManager->EffectsNames["Precipitations"] = &TheShaderManager->Effects.Rain;
	TheShaderManager->EffectsNames["Sharpening"] = &TheShaderManager->Effects.Sharpening;
	TheShaderManager->EffectsNames["ShadowsExteriors"] = &TheShaderManager->Effects.ShadowsExteriors;
	TheShaderManager->EffectsNames["ShadowsInteriors"] = &TheShaderManager->Effects.ShadowsInteriors;
	TheShaderManager->EffectsNames["Specular"] = &TheShaderManager->Effects.Specular;
	TheShaderManager->EffectsNames["Snow"] = &TheShaderManager->Effects.Snow;
	TheShaderManager->EffectsNames["SnowAccumulation"] = &TheShaderManager->Effects.SnowAccumulation;
	TheShaderManager->EffectsNames["Underwater"] = &TheShaderManager->Effects.Underwater;
	TheShaderManager->EffectsNames["VolumetricFog"] = &TheShaderManager->Effects.VolumetricFog;
	TheShaderManager->EffectsNames["WaterLens"] = &TheShaderManager->Effects.WaterLens;
	TheShaderManager->EffectsNames["WetWorld"] = &TheShaderManager->Effects.WetWorld;

	// Initialize all effects to NULL
	EffectsList::iterator v = TheShaderManager->EffectsNames.begin();
	while (v != TheShaderManager->EffectsNames.end()) {
		*v->second = NULL;
		v++;
	}

	memset(TheShaderManager->WaterVertexShaders, NULL, sizeof(WaterVertexShaders));
	memset(TheShaderManager->WaterPixelShaders, NULL, sizeof(WaterPixelShaders));
	TheShaderManager->InitializeConstants();
	TheShaderManager->ShaderConst.ReciprocalResolution.x = 1.0f / (float)TheRenderManager->width;
	TheShaderManager->ShaderConst.ReciprocalResolution.y = 1.0f / (float)TheRenderManager->height;
	TheShaderManager->ShaderConst.ReciprocalResolution.z = (float)TheRenderManager->width / (float)TheRenderManager->height;
	TheShaderManager->ShaderConst.ReciprocalResolution.w = 0.0f; // Reserved to store the FoV
	TheShaderManager->CreateFrameVertex(TheRenderManager->width, TheRenderManager->height, &TheShaderManager->FrameVertex);

    TheShaderManager->PreviousCell = nullptr;
    TheShaderManager->IsMenuSwitch = false;

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
	EffectsList::iterator v = TheShaderManager->EffectsNames.begin();
	while (v != TheShaderManager->EffectsNames.end()) {
		const char* Name = v->first.c_str();
		bool enabled = TheSettingManager->GetMenuShaderEnabled(Name);

		EffectRecord* effect = CreateEffect(Name, enabled);
		*v->second = effect; // assign pointer from Effects struct to the newly created effect record

		enabled = effect->Enabled;
		TheSettingManager->SetMenuShaderEnabled(Name, enabled); // disable settings of effects that couldn't load
		v++;
	}

	/*TODO*/
	//CreateEffect(EffectRecord::EffectRecordType::Extra);
	//if (EffectsSettings->Extra) CreateEffect(EffectRecord::EffectRecordType::Extra);

	timer.LogTime("ShaderManager::CreateEffects");
}

void ShaderManager::InitializeConstants() {

	ShaderConst.pWeather = NULL;
	ShaderConst.WaterLens.Percent = 0.0f;
	ShaderConst.BloodLens.Percent = 0.0f;
	ShaderConst.SnowAccumulation.Params.w = 0.0f;
	ShaderConst.WetWorld.Data.x = 0.0f;
	ShaderConst.WetWorld.Data.y = 0.0f;
	ShaderConst.WetWorld.Data.z = 0.0f;
	ShaderConst.Rain.RainData.x = 0.0f;
	ShaderConst.Rain.RainData.y = 0.0f;
	ShaderConst.Rain.RainData.z = 0.0f;
	ShaderConst.Rain.RainData.w = 0.0f;

	ShaderConst.Animators.PuddlesAnimator.Initialize(0);
	ShaderConst.Animators.RainAnimator.Initialize(0);
	ShaderConst.Animators.SnowAnimator.Initialize(0);
	ShaderConst.Animators.SnowAccumulationAnimator.Initialize(0);
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

	TheRenderManager->UpdateSceneCameraData();
	TheRenderManager->SetupSceneCamera();

	// context variables
	bool isExterior = Player->GetWorldSpace() || Player->parentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior;
	bool isUnderwater = WorldSky->GetIsUnderWater();
	bool isDialog = InterfaceManager->IsActive(Menu::MenuType::kMenuType_Dialog);
	bool isPersuasion = InterfaceManager->IsActive(Menu::MenuType::kMenuType_Persuasion);

	bool isRainy = false;
	bool isSnow = false;
	if (currentWeather) {
		isRainy = currentWeather->GetWeatherType() == TESWeather::WeatherType::kType_Rainy;
		isSnow = currentWeather->GetWeatherType() == TESWeather::WeatherType::kType_Snow;
	}

	TimeGlobals* GameTimeGlobals = TimeGlobals::Get();
	float GameHour = GameTimeGlobals->GameHour->data;
	float DaysPassed = GameTimeGlobals->GameDaysPassed ? GameTimeGlobals->GameDaysPassed->data : 1.0f;

	float SunriseStart = WorldSky->GetSunriseBegin();
	float SunriseEnd = WorldSky->GetSunriseEnd();
	float SunsetStart = WorldSky->GetSunsetBegin();
	float SunsetEnd = WorldSky->GetSunsetEnd();

	ShaderConst.GameTime.x = TimeGlobals::GetGameTime(); //time in milliseconds
	ShaderConst.GameTime.y = GameHour; //time in hours
	ShaderConst.GameTime.z = (float)TheFrameRateManager->Time;

	// get water height based on player position
	ShaderConst.Water.waterSettings.x = Tes->GetWaterHeight(Player, WorldSceneGraph);
	ShaderConst.Water.waterSettings.z = isUnderwater;

	float updateDelay = 0.01;

	if (currentCell) {
		ShaderConst.SunTiming.x = WorldSky->GetSunriseColorBegin();
		ShaderConst.SunTiming.y = SunriseEnd;
		ShaderConst.SunTiming.z = SunsetStart;
		ShaderConst.SunTiming.w = WorldSky->GetSunsetColorEnd();

		if (lastGameTime != ShaderConst.GameTime.y) {
			// update Sun position
			float deltaz = ShaderConst.SunDir.z;

			if (GameHour > SunsetEnd || GameHour < SunriseStart) {
				// use lighting direction at night time
				ShaderConst.SunDir.x = Tes->directionalLight->direction.x * -1;
				ShaderConst.SunDir.y = Tes->directionalLight->direction.y * -1;
				ShaderConst.SunDir.z = Tes->directionalLight->direction.z * -1;
			}
			else {
				ShaderConst.SunDir.x = SunRoot->m_localTransform.pos.x;
				ShaderConst.SunDir.y = SunRoot->m_localTransform.pos.y;
				ShaderConst.SunDir.z = SunRoot->m_localTransform.pos.z;
				D3DXVec4Normalize(&ShaderConst.SunDir, &ShaderConst.SunDir);
			}

		}

		// expose the light vector in view space for screen space lighting
		D3DXVec4Transform(&ShaderConst.ScreenSpaceLightDir, &ShaderConst.SunDir, &TheRenderManager->ViewProjMatrix);
		D3DXVec4Normalize(&ShaderConst.ScreenSpaceLightDir, &ShaderConst.ScreenSpaceLightDir);

		D3DXVec4Transform(&ShaderConst.ViewSpaceLightDir, &ShaderConst.SunDir, &TheRenderManager->ViewMatrix);
		D3DXVec4Normalize(&ShaderConst.ViewSpaceLightDir, &ShaderConst.ViewSpaceLightDir);

		ShaderConst.ShadowFade.x = 0;

		if (isExterior) {
			// fade shadows at sunrise/sunset
			float shadowFadeTime = 1.0f;
			if ((GameHour >= SunsetEnd - shadowFadeTime) && GameHour < SunsetEnd) { //sunset
				ShaderConst.ShadowFade.x = smoothStep(SunsetEnd - shadowFadeTime, SunsetEnd, GameHour);
			}
			else if (GameHour >= SunsetEnd && GameHour < SunsetEnd + shadowFadeTime) { //moonrise
				ShaderConst.ShadowFade.x = 1.0f - smoothStep(SunsetEnd, SunsetEnd + shadowFadeTime, GameHour);
			}
			else if (GameHour >= SunriseStart - shadowFadeTime && GameHour < SunriseStart) { //moonset
				ShaderConst.ShadowFade.x = smoothStep(SunriseStart - shadowFadeTime, SunriseStart, GameHour);
			}
			else if (GameHour >= SunriseStart && GameHour < SunriseStart + shadowFadeTime) { //sunrise
				ShaderConst.ShadowFade.x = 1.0f - smoothStep(SunriseStart, SunriseStart + shadowFadeTime, GameHour);
			}

			// at night time, fade based on moonphase
			if (GameHour > SunsetEnd || GameHour < SunriseStart) {
				// moonphase goes from 0 to 8
				float MoonPhase = (fmod(DaysPassed, 8 * currentClimate->phaseLength & 0x3F)) / (currentClimate->phaseLength & 0x3F);

				float PI = 3.1416; // use cos curve to fade moon light shadows strength
				MoonPhase = lerp(-PI, PI, MoonPhase / 8) - PI / 4; // map moonphase to 1/2PI/2PI + 1/2

				// map MoonVisibility to MinNightDarkness/1 range
				float nightMinDarkness = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "NightMinDarkness");
				float MoonVisibility = lerp(0.0, nightMinDarkness, cos(MoonPhase) * 0.5 + 0.5);
				ShaderConst.ShadowFade.x = lerp(MoonVisibility, 1, ShaderConst.ShadowFade.x);
			}

			// pass the enabled/disabled property of the shadow maps to the shadowfade constant
			ShaderConst.ShadowFade.y = Effects.ShadowsExteriors->Enabled;
		}
		else {
			// pass the enabled/disabled property of the shadow maps to the shadowfade constant
			ShaderConst.ShadowFade.y = Effects.ShadowsInteriors->Enabled;
		}

		//Logger::Log("exterior");

		// calculating fog color/fog amount based on sun amount
		ShaderConst.SunDir.w = 1.0f;
		if (ShaderConst.GameTime.y >= ShaderConst.SunTiming.y && ShaderConst.GameTime.y <= ShaderConst.SunTiming.z) {
			// Day time
			ShaderConst.SunAmount.x = 0.0f;
			ShaderConst.SunAmount.y = 1.0f;
			ShaderConst.SunAmount.z = 0.0f;
			ShaderConst.SunAmount.w = 0.0f;
		}
		else if ((ShaderConst.GameTime.y >= ShaderConst.SunTiming.w && ShaderConst.GameTime.y <= 23.59) || (ShaderConst.GameTime.y >= 0 && ShaderConst.GameTime.y <= ShaderConst.SunTiming.x)) {
			// Night time
			ShaderConst.SunAmount.x = 0.0f;
			ShaderConst.SunAmount.y = 0.0f;
			ShaderConst.SunAmount.z = 0.0f;
			ShaderConst.SunAmount.w = 1.0f;
		}
		else if (ShaderConst.GameTime.y >= ShaderConst.SunTiming.x && ShaderConst.GameTime.y <= ShaderConst.SunTiming.y) {
			// Sunrise
			float delta = 2.0f * invLerp(ShaderConst.SunTiming.x, ShaderConst.SunTiming.y, ShaderConst.GameTime.y); // from 0 (transition start) to 2 (transition end)

			if (delta <= 1.0f) {
				// first half: night is more prevalent than sun
				ShaderConst.SunAmount.x = delta;
				ShaderConst.SunAmount.y = 0.0f;
				ShaderConst.SunAmount.z = 0.0f;
				ShaderConst.SunAmount.w = 1.0f - delta;
			}
			else {
				// sun is more prevalent than night
				ShaderConst.SunAmount.x = 2.0f - delta;
				ShaderConst.SunAmount.y = delta - 1.0f;
				ShaderConst.SunAmount.z = 0.0f;
				ShaderConst.SunAmount.w = 0.0f;
			}
		}
		else if (ShaderConst.GameTime.y >= ShaderConst.SunTiming.z && ShaderConst.GameTime.y <= ShaderConst.SunTiming.w) {
			// Sunset
			float delta = 2.0f * invLerp(ShaderConst.SunTiming.w, ShaderConst.SunTiming.z, ShaderConst.GameTime.y); // from 0 (transition start) to 2 (transition end)

			if (delta <= 1.0f) {
				// first half, sun is more prevalent than night
				ShaderConst.SunAmount.x = 0.0f;
				ShaderConst.SunAmount.y = 1.0f - delta;
				ShaderConst.SunAmount.z = delta;
				ShaderConst.SunAmount.w = 0.0f;
			}
			else {
				// night is more prevalent than sun
				ShaderConst.SunAmount.x = 0.0f;
				ShaderConst.SunAmount.y = 0.0f;
				ShaderConst.SunAmount.z = 2.0f - delta;
				ShaderConst.SunAmount.w = delta - 1.0f;
			}
		}

		if (currentWeather) {
			ShaderConst.sunGlare = currentWeather->GetSunGlare() / 255.0f;
		}
		else {
			ShaderConst.sunGlare = 0.5f;
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

		ShaderConst.sunColor.x = WorldSky->sunDirectional.r;
		ShaderConst.sunColor.y = WorldSky->sunDirectional.g;
		ShaderConst.sunColor.z = WorldSky->sunDirectional.b;
		ShaderConst.sunColor.w = ShaderConst.sunGlare;

		ShaderConst.sunAmbient.x = WorldSky->sunAmbient.r;
		ShaderConst.sunAmbient.y = WorldSky->sunAmbient.g;
		ShaderConst.sunAmbient.z = WorldSky->sunAmbient.b;
		ShaderConst.sunAmbient.w = 1.0f;

		ShaderConst.skyColor.x = WorldSky->skyUpper.r;
		ShaderConst.skyColor.y = WorldSky->skyUpper.g;
		ShaderConst.skyColor.z = WorldSky->skyUpper.b;
		ShaderConst.skyColor.w = 1.0f;

		ShaderConst.fogData.x = WorldSky->fogNearPlane;
		ShaderConst.fogData.y = WorldSky->fogFarPlane;
		ShaderConst.fogData.z = ShaderConst.sunGlare;
		ShaderConst.fogData.w = WorldSky->fogPower;


		ShaderConst.fogDistance.x = ShaderConst.fogData.x;
		ShaderConst.fogDistance.y = ShaderConst.fogData.y;
		ShaderConst.fogDistance.z = 1.0f;
		ShaderConst.fogDistance.w = ShaderConst.sunGlare;

		//if (weatherPercent == 1.0f) ShaderConst.pWeather = currentWeather;

		//Logger::Log("Night? %f", ShaderConst.SunAmount.w);
		//Logger::Log("weather percent %f", weatherPercent);
		//Logger::Log("fog power %f", ShaderConst.fogData.w);
		//Logger::Log("fog near %f", ShaderConst.fogData.x);
		//Logger::Log("fog far %f", ShaderConst.fogData.y);
		//Logger::Log("sun r %f", ShaderConst.sunColor.x);
		//Logger::Log("sun g %f", ShaderConst.sunColor.y);
		//Logger::Log("sun b %f", ShaderConst.sunColor.z);
		//Logger::Log("sunglare %f", ShaderConst.fogData.z);
		//Logger::Log("sunglare %f", WorldSky->sun->glareScale);3

		if (TheSettingManager->GetMenuShaderEnabled("Water") || Effects.Underwater->Enabled) {
			RGBA* rgba = NULL;
			SettingsWaterStruct* sws = NULL;
			TESWaterForm* currentWater = currentCell->GetWaterForm();
			
			if (currentWater) {
				UInt32 WaterType = currentWater->GetWaterType();
				if (WaterType == TESWaterForm::WaterType::kWaterType_Blood)
					sws = TheSettingManager->GetSettingsWater("Blood");
				else if (WaterType == TESWaterForm::WaterType::kWaterType_Lava)
					sws = TheSettingManager->GetSettingsWater("Lava");
				else if (!(sws = TheSettingManager->GetSettingsWater(currentCell->GetEditorName())) && currentWorldSpace)
					sws = TheSettingManager->GetSettingsWater(currentWorldSpace->GetEditorName());
			}
			if (!sws) sws = TheSettingManager->GetSettingsWater("Default");

			if (currentWater) {
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

			ShaderConst.Water.waterCoefficients.x = sws->inExtCoeff_R;
			ShaderConst.Water.waterCoefficients.y = sws->inExtCoeff_G;
			ShaderConst.Water.waterCoefficients.z = sws->inExtCoeff_B;
			ShaderConst.Water.waterCoefficients.w = sws->inScattCoeff;

			ShaderConst.Water.waveParams.x = sws->choppiness;
			ShaderConst.Water.waveParams.y = sws->waveWidth;
			ShaderConst.Water.waveParams.z = sws->waveSpeed;
			ShaderConst.Water.waveParams.w = sws->reflectivity;

			ShaderConst.Water.waterSettings.y = sws->depthDarkness;

			ShaderConst.Water.waterVolume.x = sws->causticsStrength * ShaderConst.sunGlare;
			ShaderConst.Water.waterVolume.y = sws->shoreFactor;
			ShaderConst.Water.waterVolume.z = sws->turbidity;
			ShaderConst.Water.waterVolume.w = sws->causticsStrengthS;
			
			ShaderConst.Water.shorelineParams.x = sws->shoreMovement;
		}		

		if (isUnderwater) {
			ShaderConst.BloodLens.Percent = 0.0f;
			ShaderConst.WaterLens.Percent = -1.0f;
			ShaderConst.Animators.WaterLensAnimator.switched = true;
			ShaderConst.Animators.WaterLensAnimator.Start(0.0, 0);
		}

		if (Effects.WaterLens->Enabled) {

			if (!isUnderwater && ShaderConst.Animators.WaterLensAnimator.switched == true) {
				ShaderConst.Animators.WaterLensAnimator.switched = false;
				// start the waterlens effect and animate it fading
				ShaderConst.WaterLens.Time.x = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "TimeMultA");
				ShaderConst.WaterLens.Time.y = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "TimeMultB");
				ShaderConst.WaterLens.Time.z = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "Viscosity");
				ShaderConst.Animators.WaterLensAnimator.Initialize(1);
				ShaderConst.Animators.WaterLensAnimator.Start(0.01, 0);
			}
			ShaderConst.WaterLens.Percent = ShaderConst.Animators.WaterLensAnimator.GetValue();
			ShaderConst.WaterLens.Time.w = TheSettingManager->GetSettingF("Shaders.WaterLens.Main", "Amount") * ShaderConst.WaterLens.Percent;
		}
			
		if (isExterior) {
			// Rain fall & puddles
			if (isRainy && ShaderConst.Animators.RainAnimator.switched == false) {
				// it just started raining
				ShaderConst.WetWorld.Data.y = 1.0f;
				ShaderConst.Animators.PuddlesAnimator.Start(0.3, 1);
				ShaderConst.Animators.RainAnimator.switched = true;
				ShaderConst.Animators.RainAnimator.Start(0.05, 1);
			}
			else if (!isRainy && ShaderConst.Animators.RainAnimator.switched) {
				// it just stopped raining
				ShaderConst.WetWorld.Data.y = 0.0f;
				ShaderConst.Animators.PuddlesAnimator.Start(1.2, 0);
				ShaderConst.Animators.RainAnimator.switched = false;
				ShaderConst.Animators.RainAnimator.Start(0.07, 0);
			}
			ShaderConst.WetWorld.Data.x = ShaderConst.Animators.RainAnimator.GetValue();
			ShaderConst.WetWorld.Data.z = ShaderConst.Animators.PuddlesAnimator.GetValue();

			ShaderConst.WetWorld.Coeffs.x = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_R");
			ShaderConst.WetWorld.Coeffs.y = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_G");
			ShaderConst.WetWorld.Coeffs.z = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleCoeff_B");
			ShaderConst.WetWorld.Coeffs.w = TheSettingManager->GetSettingF("Shaders.WetWorld.Main", "PuddleSpecularMultiplier");

			ShaderConst.Rain.RainData.x = ShaderConst.Animators.RainAnimator.GetValue();
			ShaderConst.Rain.RainData.y = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "VerticalScale");
			ShaderConst.Rain.RainData.z = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Speed");
			ShaderConst.Rain.RainData.w = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Opacity");

			ShaderConst.Rain.RainAspect.x = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Refraction");
			ShaderConst.Rain.RainAspect.y = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Coloring");
			ShaderConst.Rain.RainAspect.z = TheSettingManager->GetSettingF("Shaders.Precipitations.Main", "Bloom");

			if (Effects.Snow->Enabled) {
				// Snow fall
				if (isSnow && ShaderConst.Animators.SnowAnimator.switched == false) {
					// it just started snowing
					ShaderConst.Animators.PuddlesAnimator.Start(0.3, 0); // fade out any puddles if they exist
					ShaderConst.Animators.SnowAnimator.switched = true;
					ShaderConst.Animators.SnowAnimator.Initialize(0);
					ShaderConst.Animators.SnowAnimator.Start(0.5, 1);
				}
				else if (!isSnow && ShaderConst.Animators.SnowAnimator.switched) {
					// it just stopped snowing
					ShaderConst.Animators.SnowAnimator.switched = false;
					ShaderConst.Animators.SnowAnimator.Start(0.2, 0);
				}
				ShaderConst.Snow.SnowData.x = ShaderConst.Animators.SnowAnimator.GetValue();

				ShaderConst.Snow.SnowData.y = TheSettingManager->GetSettingF("Shaders.Snow.Main", "DepthStep");
				ShaderConst.Snow.SnowData.z = TheSettingManager->GetSettingF("Shaders.Snow.Main", "Speed");
				ShaderConst.Snow.SnowData.w = TheSettingManager->GetSettingF("Shaders.Snow.Main", "Flakes");
			}

			if (Effects.SnowAccumulation->Enabled) {
				// Snow Accumulation
				if (isSnow && !ShaderConst.Animators.SnowAccumulationAnimator.switched) {
					// it just started snowing
					ShaderConst.Animators.SnowAccumulationAnimator.switched = true;
					ShaderConst.Animators.SnowAccumulationAnimator.Initialize(0);
					ShaderConst.Animators.SnowAccumulationAnimator.Start(0.8, 1);
				}
				else if (!isSnow && ShaderConst.Animators.SnowAccumulationAnimator.switched) {
					// it just stopped snowing
					ShaderConst.Animators.SnowAccumulationAnimator.switched = false;
					ShaderConst.Animators.SnowAccumulationAnimator.Start(12, 0);
				}
				ShaderConst.SnowAccumulation.Params.x = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "BlurNormDropThreshhold");
				ShaderConst.SnowAccumulation.Params.y = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "BlurRadiusMultiplier");
				ShaderConst.SnowAccumulation.Params.z = TheSettingManager->GetSettingF("Shaders.SnowAccumulation.Main", "SunPower");
				ShaderConst.SnowAccumulation.Params.w = ShaderConst.Animators.SnowAccumulationAnimator.GetValue();
			}
		}
		
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
			*Pointers::Settings::GrassStartFadeDistance = TheSettingManager->GetSettingF("Shaders.Grass.Main", "MinDistance");
			*Pointers::Settings::GrassEndDistance = TheSettingManager->GetSettingF("Shaders.Grass.Main", "MaxDistance");
			if (TheSettingManager->GetSettingI("Shaders.Grass.Main", "WindEnabled")) {
				*Pointers::Settings::GrassWindMagnitudeMax = *Pointers::ShaderParams::GrassWindMagnitudeMax = TheSettingManager->GetSettingF("Shaders.Grass.Main", "WindCoefficient") * ShaderConst.windSpeed;
				*Pointers::Settings::GrassWindMagnitudeMin = *Pointers::ShaderParams::GrassWindMagnitudeMin = *Pointers::Settings::GrassWindMagnitudeMax * 0.5f;
			}
		}

		if (TheSettingManager->GetMenuShaderEnabled("HDR")) {
			ShaderConst.HDR.ToneMapping.x = TheSettingManager->GetSettingF("Shaders.HDR.Main", "ToneMapping");
			ShaderConst.HDR.ToneMapping.y = TheSettingManager->GetSettingF("Shaders.HDR.Main", "ToneMappingBlur");
			ShaderConst.HDR.ToneMapping.z = TheSettingManager->GetSettingF("Shaders.HDR.Main", "ToneMappingColor");
			ShaderConst.HDR.ToneMapping.w = TheSettingManager->GetSettingF("Shaders.HDR.Main", "Linearization");
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

		if (Effects.GodRays->Enabled) {
			ShaderConst.GodRays.Ray.x = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "RayIntensity");
			ShaderConst.GodRays.Ray.y = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "RayLength");
			ShaderConst.GodRays.Ray.z = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "RayDensity");
			ShaderConst.GodRays.Ray.w = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "RayVisibility");

			if (TheSettingManager->GetSettingI("Shaders.Godrays.Main", "SunGlareEnabled")) {
				ShaderConst.GodRays.Ray.z *= ShaderConst.sunGlare;
				ShaderConst.GodRays.Ray.w *= ShaderConst.sunGlare;
			}


			ShaderConst.GodRays.RayColor.x = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "RayR");
			ShaderConst.GodRays.RayColor.y = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "RayG");
			ShaderConst.GodRays.RayColor.z = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "RayB");
			ShaderConst.GodRays.RayColor.w = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "Saturate");
			ShaderConst.GodRays.Data.x = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "LightShaftPasses");
			ShaderConst.GodRays.Data.y = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "Luminance");
			ShaderConst.GodRays.Data.z = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "GlobalMultiplier");
			ShaderConst.GodRays.Data.w = TheSettingManager->GetSettingF("Shaders.Godrays.Main", "TimeEnabled");
		}

		if (Effects.AmbientOcclusion->Enabled) {
			sectionName = "Shaders.AmbientOcclusion.Exteriors";
			if (!isExterior) sectionName = "Shaders.AmbientOcclusion.Interiors";

			ShaderConst.AmbientOcclusion.Enabled = TheSettingManager->GetSettingI(sectionName, "Enabled");
			if (ShaderConst.AmbientOcclusion.Enabled) {
				ShaderConst.AmbientOcclusion.AOData.x = TheSettingManager->GetSettingF(sectionName, "Samples");
				ShaderConst.AmbientOcclusion.AOData.y = TheSettingManager->GetSettingF(sectionName, "StrengthMultiplier");
				ShaderConst.AmbientOcclusion.AOData.z = TheSettingManager->GetSettingF(sectionName, "ClampStrength");
				ShaderConst.AmbientOcclusion.AOData.w = TheSettingManager->GetSettingF(sectionName, "Range");
				ShaderConst.AmbientOcclusion.Data.x = TheSettingManager->GetSettingF(sectionName, "AngleBias");
				ShaderConst.AmbientOcclusion.Data.y = TheSettingManager->GetSettingF(sectionName, "LumThreshold");
				ShaderConst.AmbientOcclusion.Data.z = TheSettingManager->GetSettingF(sectionName, "BlurDropThreshold");
				ShaderConst.AmbientOcclusion.Data.w = TheSettingManager->GetSettingF(sectionName, "BlurRadiusMultiplier");
			}
		}

		if (Effects.Bloom->Enabled) {
			sectionName = "Shaders.Bloom.Exteriors";
			if (!isExterior) sectionName = "Shaders.Bloom.Interiors";

			ShaderConst.Bloom.BloomData.x = TheSettingManager->GetSettingF(sectionName, "Luminance");
			ShaderConst.Bloom.BloomData.y = TheSettingManager->GetSettingF(sectionName, "MiddleGray");
			ShaderConst.Bloom.BloomData.z = TheSettingManager->GetSettingF(sectionName, "WhiteCutOff");
			ShaderConst.Bloom.BloomValues.x = TheSettingManager->GetSettingF(sectionName, "BloomIntensity");
			ShaderConst.Bloom.BloomValues.y = TheSettingManager->GetSettingF(sectionName, "OriginalIntensity");
			ShaderConst.Bloom.BloomValues.z = TheSettingManager->GetSettingF(sectionName, "BloomSaturation");
			ShaderConst.Bloom.BloomValues.w = TheSettingManager->GetSettingF(sectionName, "OriginalSaturation");
		}

		if (Effects.Coloring->Enabled) {
			SettingsColoringStruct* scs = TheSettingManager->GetSettingsColoring(currentCell->GetEditorName());

			if (!scs && isExterior) scs = TheSettingManager->GetSettingsColoring(currentWorldSpace->GetEditorName());
			if (!scs) scs = TheSettingManager->GetSettingsColoring("Default");
			ShaderConst.Coloring.Data.x = scs->Strength;
			ShaderConst.Coloring.Data.y = scs->BaseGamma;
			ShaderConst.Coloring.Data.z = scs->Fade;
			ShaderConst.Coloring.Data.w = scs->Contrast;
			ShaderConst.Coloring.Values.x = scs->Saturation;
			ShaderConst.Coloring.Values.y = scs->Bleach;
			ShaderConst.Coloring.Values.z = scs->BleachLuma;
			ShaderConst.Coloring.Values.w = scs->Linearization;
			ShaderConst.Coloring.ColorCurve.x = scs->ColorCurve;
			ShaderConst.Coloring.ColorCurve.y = scs->ColorCurveR;
			ShaderConst.Coloring.ColorCurve.z = scs->ColorCurveG;
			ShaderConst.Coloring.ColorCurve.w = scs->ColorCurveB;
			ShaderConst.Coloring.EffectGamma.x = scs->EffectGamma;
			ShaderConst.Coloring.EffectGamma.y = scs->EffectGammaR;
			ShaderConst.Coloring.EffectGamma.z = scs->EffectGammaG;
			ShaderConst.Coloring.EffectGamma.w = scs->EffectGammaB;
		}

		if (Effects.BloodLens->Enabled) {
			if (ShaderConst.BloodLens.Percent > 0.0f) {
				ShaderConst.BloodLens.Time.z = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "Time");
				if (ShaderConst.BloodLens.Percent == 1.0f) {
					ShaderConst.BloodLens.Time.w = 0.0f;
					srand(time(NULL));
					ShaderConst.BloodLens.Params.x = (double)rand() / (RAND_MAX + 1) * (0.75f - 0.25f) + 0.25f; //from 0.25 to 0.75
					ShaderConst.BloodLens.Params.y = (double)rand() / (RAND_MAX + 1) * (0.5f + 0.1f) - 0.1f; //from -0.1 to 0.5
					ShaderConst.BloodLens.Params.z = (double)rand() / (RAND_MAX + 1) * (2.0f + 2.0f) - 2.0f; //from -2 to 2
				}
				ShaderConst.BloodLens.Time.w += 1.0f;
				ShaderConst.BloodLens.Percent = 1.0f - ShaderConst.BloodLens.Time.w / ShaderConst.BloodLens.Time.z;
				if (ShaderConst.BloodLens.Percent < 0.0f)
					ShaderConst.BloodLens.Percent = 0.0f;
				ShaderConst.BloodLens.Params.w = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "Intensity") * ShaderConst.BloodLens.Percent;
				ShaderConst.BloodLens.BloodColor.x = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorR");
				ShaderConst.BloodLens.BloodColor.y = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorG");
				ShaderConst.BloodLens.BloodColor.z = TheSettingManager->GetSettingF("Shaders.BloodLens.Main", "ColorB");
			}
		}

		if (Effects.LowHF->Enabled) {
			float PlayerHealthPercent = (float)Player->GetActorValue(Actor::ActorVal::kActorVal_Health) / (float)Player->GetBaseActorValue(Actor::ActorVal::kActorVal_Health);
			float PlayerFatiguePercent = (float)Player->GetActorValue(Actor::ActorVal::kActorVal_Stamina) / (float)Player->GetBaseActorValue(Actor::ActorVal::kActorVal_Stamina);

			ShaderConst.LowHF.Data.x = 0.0f;
			ShaderConst.LowHF.Data.y = 0.0f;
			ShaderConst.LowHF.Data.z = 0.0f;
			ShaderConst.LowHF.Data.w = 0.0f;

			float healthLimit = TheSettingManager->GetSettingF("Shaders.LowHF.Main", "HealthLimit");
			if (Player->IsAlive()) {
				ShaderConst.LowHF.HealthCoeff = 1.0f - PlayerHealthPercent / healthLimit;
				ShaderConst.LowHF.FatigueCoeff = 1.0f - PlayerFatiguePercent / TheSettingManager->GetSettingF("Shaders.LowHF.Main", "FatigueLimit");
				if (PlayerHealthPercent < healthLimit) {
					ShaderConst.LowHF.Data.x = ShaderConst.LowHF.HealthCoeff * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "LumaMultiplier");
					ShaderConst.LowHF.Data.y = ShaderConst.LowHF.HealthCoeff * 0.01f * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "BlurMultiplier");
					ShaderConst.LowHF.Data.z = ShaderConst.LowHF.HealthCoeff * 20.0f * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "VignetteMultiplier");
					ShaderConst.LowHF.Data.w = (1.0f - ShaderConst.LowHF.HealthCoeff) * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "DarknessMultiplier");
				}
				if (!ShaderConst.LowHF.Data.x && PlayerFatiguePercent < TheSettingManager->GetSettingF("Shaders.LowHF.Main", "FatigueLimit"))
					ShaderConst.LowHF.Data.x = ShaderConst.LowHF.FatigueCoeff * TheSettingManager->GetSettingF("Shaders.LowHF.Main", "LumaMultiplier");
			}
		}

		if (Effects.DepthOfField->Enabled) {
			sectionName = "Shaders.DepthOfField.FirstPersonView";
			if (TheCameraManager->IsVanity())
				sectionName = "Shaders.DepthOfField.VanityView";
			else if (IsThirdPersonView)
				sectionName = "Shaders.DepthOfField.ThirdPersonView";


			bool dofActive = TheSettingManager->GetSettingI(sectionName, "Enabled");
			switch (TheSettingManager->GetSettingI(sectionName, "Mode")){
				case 1:
					if (isDialog || isPersuasion) dofActive = false;
					break;
				case 2:
					if (!isDialog) dofActive = false;
					break;
				case 3:
					if (!isPersuasion) dofActive = false;
					break;
				case 4:
					if (!isDialog && !isPersuasion) dofActive = false;
				default:
					break;
			}

			if (ShaderConst.DepthOfField.Enabled = dofActive) {
				ShaderConst.DepthOfField.Blur.x = TheSettingManager->GetSettingF(sectionName, "DistantBlur");
				ShaderConst.DepthOfField.Blur.y = TheSettingManager->GetSettingF(sectionName, "DistantBlurStartRange");
				ShaderConst.DepthOfField.Blur.z = TheSettingManager->GetSettingF(sectionName, "DistantBlurEndRange");
				ShaderConst.DepthOfField.Blur.w = TheSettingManager->GetSettingF(sectionName, "BaseBlurRadius");
				ShaderConst.DepthOfField.Data.x = TheSettingManager->GetSettingF(sectionName, "BlurFallOff");
				ShaderConst.DepthOfField.Data.y = TheSettingManager->GetSettingF(sectionName, "Radius");
				ShaderConst.DepthOfField.Data.z = TheSettingManager->GetSettingF(sectionName, "DiameterRange");
				ShaderConst.DepthOfField.Data.w = TheSettingManager->GetSettingF(sectionName, "NearBlurCutOff");
			}
		}

		if (Effects.Cinema->Enabled) {
			UInt8 Mode = TheSettingManager->GetSettingI("Shaders.Cinema.Main", "Mode");

			ShaderConst.Cinema.Data.x = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "AspectRatio");
			ShaderConst.Cinema.Data.y = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "VignetteRadius");
			switch (Mode) {
				case 1:
					if (isDialog || isPersuasion) Mode = -1; // disabled during dialog an persuation menus
					break;
				case 2:
					if (!isDialog) Mode = -1;
					break;
				case 3:
					if (!isPersuasion) Mode = -1;
					break;
				case 4:
					if (!isDialog && !isPersuasion) Mode = -1;
					break;
				default:
					break;
			}
			if (Mode == -1) {
				ShaderConst.Cinema.Data.x = ShaderConst.ReciprocalResolution.z; // set cinema aspect ratio to native ar
				ShaderConst.Cinema.Data.y = 0.0f;
			}
			ShaderConst.Cinema.Data.z = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "VignetteDarkness");
			ShaderConst.Cinema.Data.w = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "OverlayStrength");
			ShaderConst.Cinema.Settings.x = isUnderwater?0:TheSettingManager->GetSettingF("Shaders.Cinema.Main", "DirtLensAmount"); // disable dirt lens underwater
			ShaderConst.Cinema.Settings.y = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "FilmGrainAmount");
			ShaderConst.Cinema.Settings.z = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "ChromaticAberration");
		}

		// camera/position change data
		sectionName = "Shaders.MotionBlur.FirstPersonView";
		if (IsThirdPersonView) sectionName = "Shaders.MotionBlur.ThirdPersonView";

		float AngleZ = D3DXToDegree(Player->rot.z);
		float AngleX = D3DXToDegree(Player->rot.x);
		float fMotionBlurAmtX = ShaderConst.MotionBlur.oldAngleZ - AngleZ;
		float fMotionBlurAmtY = ShaderConst.MotionBlur.oldAngleX - AngleX;
		float fBlurDistScratchpad = fMotionBlurAmtX + 360.0f;
		float fBlurDistScratchpad2 = (AngleZ - ShaderConst.MotionBlur.oldAngleZ + 360.0f) * -1.0f;

		if (abs(fMotionBlurAmtX) > abs(fBlurDistScratchpad))
			fMotionBlurAmtX = fBlurDistScratchpad;
		else if (abs(fMotionBlurAmtX) > abs(fBlurDistScratchpad2))
			fMotionBlurAmtX = fBlurDistScratchpad2;

		if (pow(fMotionBlurAmtX, 2) + pow(fMotionBlurAmtY, 2) < TheSettingManager->GetSettingF(sectionName, "BlurCutOff")) {
			fMotionBlurAmtX = 0.0f;
			fMotionBlurAmtY = 0.0f;
		}

		ShaderConst.MotionBlur.Data.x = (ShaderConst.MotionBlur.oldoldAmountX + ShaderConst.MotionBlur.oldAmountX + fMotionBlurAmtX) / 3.0f;
		ShaderConst.MotionBlur.Data.y = (ShaderConst.MotionBlur.oldoldAmountY + ShaderConst.MotionBlur.oldAmountY + fMotionBlurAmtY) / 3.0f;
		ShaderConst.MotionBlur.oldAngleZ = AngleZ;
		ShaderConst.MotionBlur.oldAngleX = AngleX;
		ShaderConst.MotionBlur.oldoldAmountX = ShaderConst.MotionBlur.oldAmountX;
		ShaderConst.MotionBlur.oldoldAmountY = ShaderConst.MotionBlur.oldAmountY;
		ShaderConst.MotionBlur.oldAmountX = fMotionBlurAmtX;
		ShaderConst.MotionBlur.oldAmountY = fMotionBlurAmtY;
		ShaderConst.MotionBlur.BlurParams.x = TheSettingManager->GetSettingF(sectionName, "GaussianWeight");
		ShaderConst.MotionBlur.BlurParams.y = TheSettingManager->GetSettingF(sectionName, "BlurScale");
		ShaderConst.MotionBlur.BlurParams.z = TheSettingManager->GetSettingF(sectionName, "BlurOffsetMax");


		if (Effects.Sharpening->Enabled) {
			ShaderConst.Sharpening.Data.x = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Strength");
			ShaderConst.Sharpening.Data.y = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Clamp");
			ShaderConst.Sharpening.Data.z = TheSettingManager->GetSettingF("Shaders.Sharpening.Main", "Offset");
		}

		if (Effects.Exposure->Enabled) {
			ShaderConst.Exposure.Data.x = TheSettingManager->GetSettingF("Shaders.Exposure.Main", "MinBrightness");
			ShaderConst.Exposure.Data.y = TheSettingManager->GetSettingF("Shaders.Exposure.Main", "MaxBrightness");
			ShaderConst.Exposure.Data.z = TheSettingManager->GetSettingF("Shaders.Exposure.Main", "DarkAdaptSpeed");
			ShaderConst.Exposure.Data.w = TheSettingManager->GetSettingF("Shaders.Exposure.Main", "LightAdaptSpeed");
		}

		ShaderConst.Shadow.ScreenSpaceData.x = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ScreenSpace", "Enabled");
		ShaderConst.Shadow.ScreenSpaceData.y = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "BlurRadius");
		ShaderConst.Shadow.ScreenSpaceData.z = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "RenderDistance");

		if (Effects.Specular->Enabled) {
			float rainyPercent = ShaderConst.Animators.RainAnimator.GetValue();
			SettingsSpecularStruct::ExteriorStruct* ext = &TheSettingManager->SettingsSpecular.Exterior;
			SettingsSpecularStruct::RainStruct* rain = &TheSettingManager->SettingsSpecular.Rain;

			// handle transition by interpolating previous and current weather settings
			ShaderConst.Specular.Data.x = lerp(ext->SpecLumaTreshold, rain->SpecLumaTreshold, rainyPercent);
			ShaderConst.Specular.Data.y = lerp(ext->BlurMultiplier, rain->BlurMultiplier, rainyPercent);
			ShaderConst.Specular.Data.z = lerp(ext->Glossiness, rain->Glossiness, rainyPercent);
			ShaderConst.Specular.Data.w = lerp(ext->DistanceFade, rain->DistanceFade, rainyPercent);
			ShaderConst.Specular.EffectStrength.x = lerp(ext->SpecularStrength, rain->SpecularStrength, rainyPercent);
			ShaderConst.Specular.EffectStrength.y = lerp(ext->SkyTintStrength, rain->SkyTintStrength, rainyPercent);
			ShaderConst.Specular.EffectStrength.z = lerp(ext->FresnelStrength, rain->FresnelStrength, rainyPercent);
			ShaderConst.Specular.EffectStrength.w = lerp(ext->SkyTintSaturation, rain->SkyTintSaturation, rainyPercent);
		}

		if (Effects.VolumetricFog->Enabled) {
			ShaderConst.VolumetricFog.Data.x = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Exponent");
			ShaderConst.VolumetricFog.Data.y = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "ColorCoeff");
			ShaderConst.VolumetricFog.Data.z = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "Amount");
			ShaderConst.VolumetricFog.Data.w = TheSettingManager->GetSettingF("Shaders.VolumetricFog.Main", "MaxDistance");
			//if (weatherPercent == 1.0f && ShaderConst.fogData.y > TheSettingManager->SettingsVolumetricFog.MaxDistance) ShaderConst.VolumetricFog.Data.w = 0.0f;
		}
	}

	ShaderConst.DebugVar.x = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar1");
	ShaderConst.DebugVar.y = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar2");
	ShaderConst.DebugVar.z = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar3");
	ShaderConst.DebugVar.w = TheSettingManager->GetSettingF("Main.Develop.Main", "DebugVar4");

	timer.LogTime("ShaderManager::UpdateConstants");
}

void ShaderManager::CreateShader(const char* Name) {
	
	NiD3DVertexShader** Vertex = NULL;
	NiD3DPixelShader** Pixel = NULL;
	int WaterVertexShadersSize = sizeof(WaterVertexShaders) / 4;
	int WaterPixelShadersSize = sizeof(WaterPixelShaders) / 4;
	int Upperbound = 0;

	if (!strcmp(Name, "Terrain")) {
		Upperbound = GetShader(Name, &Vertex, NULL, 0);
		for (int i = 0; i < Upperbound; i++) if (Vertex[i] && strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) LoadShader(Vertex[i]);
		Upperbound = GetShader(Name, &Pixel, NULL, 0);
		for (int i = 0; i < Upperbound; i++) if (Pixel[i] && strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) LoadShader(Pixel[i]);
	}
	else if (!strcmp(Name, "ExtraShaders")) {
		Upperbound = GetShader(Name, &Vertex, NULL, 0);
		for (int i = 0; i < Upperbound; i++) if (Vertex[i] && !strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) LoadShader(Vertex[i]);
		Upperbound = GetShader(Name, &Pixel, NULL, 0);
		for (int i = 0; i < Upperbound; i++) if (Pixel[i] && !strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) LoadShader(Pixel[i]);
	}
	else {
		Upperbound = GetShader(Name, &Vertex, WaterVertexShaders, WaterVertexShadersSize);
		for (int i = 0; i < Upperbound; i++) if (Vertex[i]) LoadShader(Vertex[i]);
		Upperbound = GetShader(Name, &Pixel, WaterPixelShaders, WaterPixelShadersSize);
		for (int i = 0; i < Upperbound; i++) if (Pixel[i]) LoadShader(Pixel[i]);
		if (!strcmp(Name, "Water")) {
			Upperbound = GetShader("WaterHeightMap", &Vertex, WaterVertexShaders, WaterVertexShadersSize);
			for (int i = 0; i < Upperbound; i++) if (Vertex[i]) LoadShader(Vertex[i]);
			Upperbound = GetShader("WaterHeightMap", &Pixel, WaterPixelShaders, WaterPixelShadersSize);
			for (int i = 0; i < Upperbound; i++) if (Pixel[i]) LoadShader(Pixel[i]);
			Upperbound = GetShader("WaterDisplacement", &Vertex, WaterVertexShaders, WaterVertexShadersSize);
			for (int i = 0; i < Upperbound; i++) if (Vertex[i]) LoadShader(Vertex[i]);
			Upperbound = GetShader("WaterDisplacement", &Pixel, WaterPixelShaders, WaterPixelShadersSize);
			for (int i = 0; i < Upperbound; i++) if (Pixel[i]) LoadShader(Pixel[i]);
		}
	}

}

void ShaderManager::LoadShader(NiD3DVertexShader* Shader) {
	
	NiD3DVertexShaderEx* VertexShader = (NiD3DVertexShaderEx*)Shader;

	VertexShader->ShaderProg  = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, NULL);
	VertexShader->ShaderProgE = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, "Exteriors\\");
	VertexShader->ShaderProgI = (ShaderRecordVertex*)ShaderRecord::LoadShader(VertexShader->ShaderName, "Interiors\\");

}

void ShaderManager::LoadShader(NiD3DPixelShader* Shader) {

	NiD3DPixelShaderEx* PixelShader = (NiD3DPixelShaderEx*)Shader;

	PixelShader->ShaderProg  = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, NULL);
	PixelShader->ShaderProgE = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, "Exteriors\\");
	PixelShader->ShaderProgI = (ShaderRecordPixel*)ShaderRecord::LoadShader(PixelShader->ShaderName, "Interiors\\");

}

void ShaderManager::DisposeShader(const char* Name) {

	NiD3DVertexShader** Vertex = NULL;
	NiD3DPixelShader** Pixel = NULL;
	int WaterVertexShadersSize = sizeof(WaterVertexShaders) / 4;
	int WaterPixelShadersSize = sizeof(WaterPixelShaders) / 4;
	int Upperbound = 0;

	if (!strcmp(Name, "Terrain")) {
		Upperbound = GetShader(Name, &Vertex, NULL, 0);
		for (int i = 0; i < GetShader(Name, &Vertex, NULL, 0); i++) if (Vertex[i] && strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
		Upperbound = GetShader(Name, &Pixel, NULL, 0);
		for (int i = 0; i < GetShader(Name, &Pixel, NULL, 0); i++) if (Pixel[i] && strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
	}
	else if (!strcmp(Name, "ExtraShaders")) {
		Upperbound = GetShader(Name, &Vertex, NULL, 0);
		for (int i = 0; i < GetShader(Name, &Vertex, NULL, 0); i++) if (Vertex[i] && !strstr(TerrainShaders, ((NiD3DVertexShaderEx*)Vertex[i])->ShaderName)) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
		Upperbound = GetShader(Name, &Pixel, NULL, 0);
		for (int i = 0; i < GetShader(Name, &Pixel, NULL, 0); i++) if (Pixel[i] && !strstr(TerrainShaders, ((NiD3DPixelShaderEx*)Pixel[i])->ShaderName)) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
	}
	else {
		Upperbound = GetShader(Name, &Vertex, WaterVertexShaders, WaterVertexShadersSize);
		for (int i = 0; i < GetShader(Name, &Vertex, WaterVertexShaders, WaterVertexShadersSize); i++) if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
		Upperbound = GetShader(Name, &Pixel, WaterPixelShaders, WaterPixelShadersSize);
		for (int i = 0; i < GetShader(Name, &Pixel, WaterPixelShaders, WaterPixelShadersSize); i++) if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
		if (!strcmp(Name, "Water")) {
			Upperbound = GetShader("WaterHeightMap", &Vertex, WaterVertexShaders, WaterVertexShadersSize);
			for (int i = 0; i < GetShader("WaterHeightMap", &Vertex, WaterVertexShaders, WaterVertexShadersSize); i++) if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
			Upperbound = GetShader("WaterHeightMap", &Pixel, WaterPixelShaders, WaterPixelShadersSize);
			for (int i = 0; i < GetShader("WaterHeightMap", &Pixel, WaterPixelShaders, WaterPixelShadersSize); i++) if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
			Upperbound = GetShader("WaterDisplacement", &Vertex, WaterVertexShaders, WaterVertexShadersSize);
			for (int i = 0; i < GetShader("WaterDisplacement", &Vertex, WaterVertexShaders, WaterVertexShadersSize); i++) if (Vertex[i]) ((NiD3DVertexShaderEx*)Vertex[i])->DisposeShader();
			Upperbound = GetShader("WaterDisplacement", &Pixel, WaterPixelShaders, WaterPixelShadersSize);
			for (int i = 0; i < GetShader("WaterDisplacement", &Pixel, WaterPixelShaders, WaterPixelShadersSize); i++) if (Pixel[i]) ((NiD3DPixelShaderEx*)Pixel[i])->DisposeShader();
		}
	}

}

/*
* Loads an Effect Shader from the corresponding fx file based on the Effect Record effect Type.
*/
EffectRecord* ShaderManager::CreateEffect(const char* Name, bool setEnabled) {
	
	char Filename[MAX_PATH];
	strcpy(Filename, EffectsPath);
	strcat(Filename, Name);
	strcat(Filename, ".fx");

	EffectRecord* effect = EffectRecord::LoadEffect(Filename);
	if (!effect->IsLoaded()) setEnabled = false;

	effect->Enabled = setEnabled;
	return effect;

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
	Effect->Render(Device, RenderTarget, RenderTarget, clearRenderTarget, true);
};

/*
* Renders the effect that have been set to enabled.
*/
void ShaderManager::RenderEffects(IDirect3DSurface9* RenderTarget) {
	
	auto timer = TimeLogger();

	IDirect3DDevice9* Device = TheRenderManager->device;
	IDirect3DSurface9* SourceSurface = TheTextureManager->SourceSurface;
	IDirect3DSurface9* RenderedSurface = TheTextureManager->RenderedSurface;
	TESWorldSpace* currentWorldSpace = Player->GetWorldSpace();
	TESObjectCELL* currentCell = Player->parentCell;
	Sky* WorldSky = Tes->sky;
	bool isExterior = Player->GetWorldSpace() || Player->parentCell->flags0 & TESObjectCELL::kFlags0_BehaveLikeExterior;
	bool isUnderwater = Tes->sky->GetIsUnderWater(); /*TODO do this work in interior????*/
	bool isDaytime = (ShaderConst.GameTime.y >= ShaderConst.SunTiming.x && ShaderConst.GameTime.y <= ShaderConst.SunTiming.w); // gametime is between sunrise start and sunset end
	bool isCellTransition = currentCell != PreviousCell;

	//bool pipboyIsOn = InterfaceManager->IsActive(Menu::kMenuType_BigFour);
	bool pipboyIsOn = InterfaceManager->getIsMenuOpen();
	bool VATSIsOn = InterfaceManager->IsActive(Menu::kMenuType_VATS);
	bool terminalIsOn = InterfaceManager->IsActive(Menu::kMenuType_Computers) ||
		InterfaceManager->IsActive(Menu::kMenuType_LockPick) || 
		InterfaceManager->IsActive(Menu::kMenuType_Surgery);

	TheShaderManager->UpdateConstants();

	Device->SetStreamSource(0, FrameVertex, 0, sizeof(FrameVS));
	Device->SetFVF(FrameFVF);

	// render post process normals for use by shaders
	RenderEffectToRT(TheTextureManager->NormalsSurface, Effects.Normals, true);

	// prepare device for effects
	Device->SetRenderTarget(0, RenderTarget);
	Device->StretchRect(RenderTarget, NULL, RenderedSurface, NULL, D3DTEXF_NONE);
	Device->StretchRect(RenderTarget, NULL, SourceSurface, NULL, D3DTEXF_NONE);

	if (!terminalIsOn) {
		// disable these effects during terminal/lockpicking sequences because they bleed through the overlay

		// Snow must be rendered first so other effects appear on top
		if (Effects.SnowAccumulation->Enabled && ShaderConst.SnowAccumulation.Params.w > 0.0f && isExterior && !isUnderwater) Effects.SnowAccumulation->Render(Device, RenderTarget, RenderedSurface, false, true);
		if (ShaderConst.AmbientOcclusion.Enabled) Effects.AmbientOcclusion->Render(Device, RenderTarget, RenderedSurface, false, true);

		// Disable shadows during VATS
		if (!VATSIsOn) {
			if (isExterior && Effects.ShadowsExteriors->Enabled) Effects.ShadowsExteriors->Render(Device, RenderTarget, RenderedSurface, false, true);
			else if (!isExterior && Effects.ShadowsInteriors->Enabled) Effects.ShadowsInteriors->Render(Device, RenderTarget, RenderedSurface, false, true);
		}

		if (isUnderwater) {
			// underwater only effects
			if (Effects.Underwater->Enabled && isUnderwater) Effects.Underwater->Render(Device, RenderTarget, RenderedSurface, false, false);
		}
		else {
			if (isExterior) {
				if (Effects.Specular->Enabled) Effects.Specular->Render(Device, RenderTarget, RenderedSurface, false, true);
				if (Effects.WetWorld->Enabled && ShaderConst.WetWorld.Data.z > 0.0f && !VATSIsOn) Effects.WetWorld->Render(Device, RenderTarget, RenderedSurface, false, true);
			}

			if (Effects.VolumetricFog->Enabled && !pipboyIsOn) Effects.VolumetricFog->Render(Device, RenderTarget, RenderedSurface, false, false);

			if (isExterior) {
				if (Effects.Rain->Enabled && ShaderConst.Rain.RainData.x > 0.0f) Effects.Rain->Render(Device, RenderTarget, RenderedSurface, false, true);
				if (Effects.Snow->Enabled && ShaderConst.Snow.SnowData.x > 0.0f) Effects.Snow->Render(Device, RenderTarget, RenderedSurface, false, false);
				if (Effects.GodRays->Enabled && isDaytime) Effects.GodRays->Render(Device, RenderTarget, RenderedSurface, false, true);
			}
		}
	}

	// calculate average luma for use by shaders
	RenderEffectToRT(TheTextureManager->AvgLumaSurface, Effects.AvgLuma, false);
	Device->SetRenderTarget(0, RenderTarget); 	// restore device used for effects

	// screenspace coloring/blurring effects get rendered last
	if (Effects.Coloring->Enabled) Effects.Coloring->Render(Device, RenderTarget, RenderedSurface, false, false);
	if (Effects.Bloom->Enabled) Effects.Bloom->Render(Device, RenderTarget, RenderedSurface, false, true);
	if (Effects.Exposure->Enabled) Effects.Exposure->Render(Device, RenderTarget, RenderedSurface, false, true);
	if (Effects.DepthOfField->Enabled && ShaderConst.DepthOfField.Enabled) Effects.DepthOfField->Render(Device, RenderTarget, RenderedSurface, false, true);
	if (Effects.MotionBlur->Enabled && (ShaderConst.MotionBlur.Data.x || ShaderConst.MotionBlur.Data.y)) Effects.MotionBlur->Render(Device, RenderTarget, RenderedSurface, false, true);

	// lens effects
	if (Effects.BloodLens->Enabled && ShaderConst.BloodLens.Percent > 0.0f) Effects.BloodLens->Render(Device, RenderTarget, RenderedSurface, false, false);
	if (Effects.WaterLens->Enabled && ShaderConst.WaterLens.Percent > 0.0f) Effects.WaterLens->Render(Device, RenderTarget, RenderedSurface, false, false);
	if (Effects.LowHF->Enabled && ShaderConst.LowHF.Data.x) Effects.LowHF->Render(Device, RenderTarget, RenderedSurface, false, false);
	if (Effects.Sharpening->Enabled) Effects.Sharpening->Render(Device, RenderTarget, RenderedSurface, false, false);

	// cinema effect gets rendered very last because of vignetting/letterboxing
	if (Effects.Cinema->Enabled && (ShaderConst.Cinema.Data.x != 0.0f || ShaderConst.Cinema.Data.y != 0.0f)) Effects.Cinema->Render(Device, RenderTarget, RenderedSurface, false, true);

	//if (EffectsSettings->Extra) {
	//	for (EffectsList::iterator iter = Effects.ExtraEffects.begin(); iter != Effects.ExtraEffects.end(); ++iter) {
	//		if (iter->second->Enabled) {
	//			iter->second->Render(Device, RenderTarget, RenderedSurface, false, true);
	//		}
	//	}
	//}

	PreviousCell = currentCell;

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
	catch (std::out_of_range e){
		// shaders
		bool enabled = TheSettingManager->GetMenuShaderEnabled(Name);
		TheSettingManager->SetMenuShaderEnabled(Name, !enabled);
		DisposeShader(Name);
		if (enabled) CreateShader(Name);
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
	return (1 - t) * a + t * b;
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
