#pragma once
#define FrameFVF D3DFVF_XYZ | D3DFVF_TEX1

#include "Effects/Animator.h"
#include "Effects/ShaderRecord.h"
#include "Effects/AmbientOcclusion.h"
#include "Effects/ShadowsExterior.h"
#include "Effects/AvgLuma.h"
#include "Effects/BloodLens.h"
#include "Effects/BloomLegacy.h"
#include "Effects/Cinema.h"
#include "Effects/Coloring.h"
#include "Effects/DepthOfField.h"
#include "Effects/Exposure.h"
#include "Effects/Debug.h"
#include "Effects/GodRays.h"
#include "Effects/ImageAdjust.h"
#include "Effects/Lens.h"
#include "Effects/LowHF.h"
#include "Effects/MotionBlur.h"
#include "Effects/Normals.h"
#include "Effects/Rain.h"
#include "Effects/Sharpening.h"
#include "Effects/Specular.h"
#include "Effects/SunShadows.h"
#include "Effects/PointShadows.h"
#include "Effects/PointShadows2.h"
#include "Effects/Pretonemapper.h"
#include "Effects/ShadowsInteriors.h"
#include "Effects/SnowAccumulation.h"
#include "Effects/Snow.h"
#include "Effects/Underwater.h"
#include "Effects/VolumetricFog.h"
#include "Effects/WaterLens.h"
#include "Effects/WetWorld.h"

#include "Effects/Water.h"
#include "Effects/Tonemapping.h"
#include "Effects/Sky.h"
#include "Effects/Skin.h"
#include "Effects/Terrain.h"


struct ShaderConstants {
	
	struct ShadowMapStruct {
		D3DXMATRIXA16	ShadowWorld;
		D3DXMATRIX		ShadowViewProj;
		D3DXMATRIX		ShadowCameraToLight[5];
		D3DXVECTOR4		ShadowCubeMapLightPosition;
		D3DXVECTOR4		ShadowLightPosition[ShadowCubeMapsMax];
		D3DXVECTOR4		ShadowCubeMapBlend;
		D3DXVECTOR4		ShadowMapRadius;
	};
	struct OcclusionMapStruct {
		D3DXMATRIX		OcclusionWorldViewProj;
	};

	struct GrassStruct {
		D3DXVECTOR4		Scale;
	};
	struct POMStruct {
		D3DXVECTOR4		ParallaxData;
	};
	D3DXVECTOR4				ReciprocalResolution;
	D3DXVECTOR4				SunDir;
	D3DXVECTOR4				SunPosition;
	D3DXVECTOR4				SunTiming;
	D3DXVECTOR4				SunAmount;
	D3DXVECTOR4				ViewSpaceLightDir;
	D3DXVECTOR4				ScreenSpaceLightDir;
	D3DXVECTOR4				GameTime;
	D3DXVECTOR4				FrameTime;
	TESWeather*				pWeather;
	float					sunGlare;
	float					windSpeed;
	D3DXVECTOR4				fogColor;
	D3DXVECTOR4				horizonColor;
	D3DXVECTOR4				skyLowColor;
	D3DXVECTOR4				sunColor;
	D3DXVECTOR4				sunAmbient;
	D3DXVECTOR4				skyColor;
	D3DXVECTOR4				fogData;
	D3DXVECTOR4				fogDistance;
	float					fogStart;
	float					fogEnd;
	float					fogPower;
	ShadowMapStruct			ShadowMap;
	OcclusionMapStruct		OcclusionMap;
	GrassStruct				Grass;
	POMStruct				POM;
	D3DXVECTOR4				DebugVar;
};


typedef std::map<std::string, EffectRecord**> EffectsList;
typedef std::map<std::string, ShaderCollection**> ShaderList;
typedef std::map<std::string, D3DXVECTOR4> CustomConstants;

struct		FrameVS { float x, y, z, u, v; };

__declspec(align(16)) class ShaderManager : public ShaderManagerBase { // Never disposed
public:
	static void Initialize();

	void* operator new(size_t i) { return _mm_malloc(i, 16); }

	int						GetVertexShaders(const char* Name, NiD3DVertexShader*** Shader);
	int						GetPixelShaders(const char* Name, NiD3DPixelShader*** Shader);
	void					CreateFrameVertex(UInt32 Width, UInt32 Height, IDirect3DVertexBuffer9** FrameVertex);
	void					CreateEffects();
	void					InitializeConstants();
	void					UpdateConstants();
	void					GetNearbyLights(NiPointLight* ShadowLightsList[], NiPointLight* LightsList[]);
	bool					CreateShader(const char* Name);
	bool					LoadShader(NiD3DVertexShader* VertexShader);
	bool					LoadShader(NiD3DPixelShader* PixelShader);
	void					DisposeShader(const char* Name);
	EffectRecord*			CreateEffect(const char* Name);
	ShaderCollection*		CreateCollection(const char* Name);
	ShaderCollection*		GetShaderCollection(const char* Name);
	void					LoadEffects();
	void					DisposeEffect(EffectRecord** Effect);  // unused?
	void					RenderEffects(IDirect3DSurface9* RenderTarget);
	void					RenderEffectsPreTonemapping(IDirect3DSurface9* RenderTarget);
	void					RenderEffectToRT(IDirect3DSurface9* RenderTarget, EffectRecord* Effect, bool clearRenderTarget);
	void					SwitchShaderStatus(const char* Name);
	void					SetCustomConstant(const char* Name, D3DXVECTOR4 Value);
	void					SetExtraEffectEnabled(const char* Name, bool Value);
	static float			lerp(float a, float b, float t);
	static D3DXVECTOR4		lerp(D3DXVECTOR4 a, D3DXVECTOR4 b, float t);
	static float			invLerp(float a, float b, float t);
	static float			step(float a, float b, float t);
	static float			smoothStep(float a, float b, float t);
	static float			clamp(float a, float b, float t);
		
	struct	EffectsStruct {
		AmbientOcclusionEffect*	AmbientOcclusion;
		AvgLumaEffect*			AvgLuma;
		BloodLensEffect*		BloodLens;
		BloomLegacyEffect*		BloomLegacy;
		ColoringEffect*			Coloring;
		CinemaEffect*			Cinema;
		ExposureEffect*			Exposure;
		DepthOfFieldEffect*		DepthOfField;
		DebugEffect*			Debug;
		GodRaysEffect*			GodRays;
		ImageAdjustEffect*		ImageAdjust;
		LensEffect*				Lens;
		LowHFEffect*			LowHF;
		MotionBlurEffect*		MotionBlur;
		NormalsEffect*			Normals;
		PretonemapperEffect*	Pretonemapper;
		RainEffect*				Rain;
		SharpeningEffect*		Sharpening;
		SpecularEffect*			Specular;
		SnowEffect*				Snow;
		SnowAccumulationEffect*	SnowAccumulation;
		ShadowsExteriorEffect*	ShadowsExteriors;
		ShadowsInteriorsEffect*	ShadowsInteriors;
		PointShadowsEffect*		PointShadows;
		PointShadows2Effect*	PointShadows2;
		SunShadowsEffect*		SunShadows;
		UnderwaterEffect*		Underwater;
		VolumetricFogEffect*	VolumetricFog;
		WaterLensEffect*		WaterLens;
		WetWorldEffect*			WetWorld;
		EffectsList				ExtraEffects;
	};

	struct ShadersStruct{
		WaterShaders*			Water;
		TonemappingShaders*		Tonemapping;
		ShaderCollection*		POM;
		ShaderCollection*		Blood;
		SkyShaders*				Sky;
		SkinShaders*			Skin;
		ShaderCollection*		Grass;
		TerrainShaders*			Terrain;
		ShaderCollection*		ExtraShaders;
	};

	EffectsStruct			Effects;
	ShadersStruct			Shaders;
	EffectsList				EffectsNames;
	ShaderList				ShaderNames;
	ShaderConstants			ShaderConst;
	CustomConstants			CustomConst;
	std::map<const char*, D3DXVECTOR4*>	ConstantsTable;
	IDirect3DVertexBuffer9*	FrameVertex;
	NiD3DVertexShader*		WaterVertexShaders[51];
	NiD3DPixelShader*		WaterPixelShaders[51];
    TESObjectCELL*          PreviousCell;
    bool                    IsMenuSwitch;
    bool                    orthoRequired;
    bool                    avglumaRequired;
	float					isDayTime;
	bool					isDayTimeChanged;
	float					transitionCurve;
	float					dayLight;
	bool					isExterior;
	bool					isUnderwater;
	bool					isDialog;
	bool					isPersuasion;
	bool					isCellChanged;
	bool					VATSIsOn;
	bool					PipBoyIsOn;
	bool					isRainy;
	bool					isSnow;
	bool					OverlayIsOn;
	D3DXVECTOR4				LightPosition[TrackedLightsMax];
	D3DXVECTOR4				LightAttenuation[TrackedLightsMax];
};

