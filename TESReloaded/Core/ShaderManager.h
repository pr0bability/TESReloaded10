#pragma once
#define FrameFVF D3DFVF_XYZ | D3DFVF_TEX1

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


class Animator {
public:
	Animator();
	virtual ~Animator();
	void				Initialize(float startValue);
	float				GetValue();
	void				Start(float duration, float finalValue);

	TimeGlobals* time;
	float				startValue;
	float				endValue;
	float				startTime;
	float				endTime;
	bool				running;
	bool				switched; // set to true/false to detect if animator has been run before
};



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
	struct WaterStruct {
		D3DXVECTOR4		waterCoefficients;
		D3DXVECTOR4		waveParams;
		D3DXVECTOR4		waterVolume;
		D3DXVECTOR4		waterSettings;
		D3DXVECTOR4		deepColor;
		D3DXVECTOR4		shallowColor;
		D3DXVECTOR4		shorelineParams;
	};
	struct HDRStruct {
		D3DXVECTOR4		ToneMapping;
		D3DXVECTOR4		BloomData;
		D3DXVECTOR4		SkyData;
		D3DXVECTOR4		HDRData;
		D3DXVECTOR4		LotteData;
		float			PointLightMult;
	};
	struct GrassStruct {
		D3DXVECTOR4		Scale;
	};
	struct POMStruct {
		D3DXVECTOR4		ParallaxData;
	};
	struct TerrainStruct {
		D3DXVECTOR4		Data;
	};
	struct SkinStruct {
		D3DXVECTOR4		SkinData;
		D3DXVECTOR4		SkinColor;
	};
	struct RainStruct{
		D3DXVECTOR4		RainData;
		D3DXVECTOR4		RainAspect;
	};
	struct SnowStruct{
		D3DXVECTOR4		SnowData;
	};
	struct WaterLensStruct {
		D3DXVECTOR4		Time;
		float			TimeAmount;
		float			Percent;
	};
	struct LensStruct {
		D3DXVECTOR4		Data;
	};
	struct SnowAccumulationStruct {
		D3DXVECTOR4		Params;
		D3DXVECTOR4		Color;
	};
	struct MotionBlurStruct {
		D3DXVECTOR4		BlurParams;
		D3DXVECTOR4		Data;
		float			oldAngleX;
		float			oldAngleZ;
		float			oldAmountX;
		float			oldAmountY;
		float			oldoldAmountX;
		float			oldoldAmountY;
	};
	struct LowHFStruct {
		D3DXVECTOR4		Data;
		float			HealthCoeff;
		float			FatigueCoeff;
	};
	struct ImageAdjustStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		DarkColor;
		D3DXVECTOR4		LightColor;
	};
	struct WetWorldStruct {
		D3DXVECTOR4		Coeffs;
		D3DXVECTOR4		Data;
	};
	struct SharpeningStruct {
		D3DXVECTOR4		Data;
	};
	struct VolumetricFogStruct {
		D3DXVECTOR4		LowFog;
		D3DXVECTOR4		GeneralFog;
		D3DXVECTOR4		SimpleFog;
		D3DXVECTOR4		Blend;
		D3DXVECTOR4		Height;
	};

	struct SpecularStruct {
		D3DXVECTOR4		EffectStrength;
		D3DXVECTOR4		Data;
	};

	struct SkyStruct {
		D3DXVECTOR4		SkyData;
		D3DXVECTOR4		SunsetColor;
		D3DXVECTOR4		CloudData;
	};

	struct AnimatorsStruct {
		Animator			RainAnimator;
		Animator			PuddlesAnimator;
		Animator			WaterLensAnimator;
		Animator			SnowAnimator;
		Animator			SnowAccumulationAnimator;
	};

	D3DXVECTOR4				ReciprocalResolution;
	D3DXVECTOR4				SunDir;
	D3DXVECTOR4				SunPosition;
	D3DXVECTOR4				SunTiming;
	D3DXVECTOR4				SunAmount;
	D3DXVECTOR4				ShadowFade;
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
	AnimatorsStruct			Animators;
	ShadowMapStruct			ShadowMap;
	OcclusionMapStruct		OcclusionMap;
	WaterStruct				Water;
	HDRStruct				HDR;
	GrassStruct				Grass;
	POMStruct				POM;
	TerrainStruct			Terrain;
	SkinStruct				Skin;
	RainStruct				Rain;
	SnowStruct				Snow;
	WaterLensStruct			WaterLens;
	ImageAdjustStruct		ImageAdjust;
	LensStruct				Lens;
	SnowAccumulationStruct	SnowAccumulation;
	MotionBlurStruct		MotionBlur;
	LowHFStruct				LowHF;
	WetWorldStruct			WetWorld;
	SharpeningStruct		Sharpening;
	SpecularStruct			Specular;
	SkyStruct				Sky;
	VolumetricFogStruct		VolumetricFog;
	D3DXVECTOR4				DebugVar;
};


typedef std::map<std::string, EffectRecord**> EffectsList;
typedef std::map<std::string, D3DXVECTOR4> CustomConstants;

struct		FrameVS { float x, y, z, u, v; };

__declspec(align(16)) class ShaderManager : public ShaderManagerBase { // Never disposed
public:
	static void Initialize();

	void* operator new(size_t i) { return _mm_malloc(i, 16); }

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
		EffectRecord*		ImageAdjust;
		EffectRecord*		Lens;
		EffectRecord*		LowHF;
		EffectRecord*		MotionBlur;
		EffectRecord*		Normals;
		EffectRecord*		PreTonemapper;
		EffectRecord*		Rain;
		EffectRecord*		Sharpening;
		EffectRecord*		Specular;
		EffectRecord*		Snow;
		EffectRecord*		SnowAccumulation;
		ShadowsExteriorEffect*		ShadowsExteriors;
		EffectRecord*		ShadowsInteriors;
		EffectRecord*		PointShadows;
		EffectRecord*		PointShadows2;
		EffectRecord*		SunShadows;
		EffectRecord*		Underwater;
		EffectRecord*		VolumetricFog;
		EffectRecord*		WaterLens;
		EffectRecord*		WetWorld;
		EffectsList			ExtraEffects;
	};

	EffectsStruct			Effects;
	EffectsList				EffectsNames;
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

