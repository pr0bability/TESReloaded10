#pragma once
#define FrameFVF D3DFVF_XYZ | D3DFVF_TEX1


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


enum ShaderCompileType{
	AlwaysOff,
	AlwaysOn,
	RecompileChanged,
	RecompileInMenu,
	RecompileAbsent,
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
	struct ShadowStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ScreenSpaceData;
		D3DXVECTOR4		OrthoData;
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
	struct GodRaysStruct {
		D3DXVECTOR4		Ray;
		D3DXVECTOR4		RayColor;
		D3DXVECTOR4		Data;
	};
	struct DepthOfFieldStruct {
		bool			Enabled;
		D3DXVECTOR4		Blur;
		D3DXVECTOR4		Data;
	};
	struct ExposureStruct {
		D3DXVECTOR4		Data;
	};
	struct AmbientOcclusionStruct {
		bool			Enabled;
		D3DXVECTOR4		AOData;
		D3DXVECTOR4		Data;
	};
	struct ColoringStruct {
		D3DXVECTOR4		ColorCurve;
		D3DXVECTOR4		EffectGamma;
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Values;
	};
	struct CinemaStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Settings;
	};
	struct LensStruct {
		D3DXVECTOR4		Data;
	};
	struct BloomStruct {
		D3DXVECTOR4		BloomData;
		D3DXVECTOR4		BloomValues;
	};
	struct SnowAccumulationStruct {
		D3DXVECTOR4		Params;
	};
	struct BloodLensStruct {
		D3DXVECTOR4		Params;
		D3DXVECTOR4		BloodColor;
		D3DXVECTOR4		Time;
		float			Percent;
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
		D3DXVECTOR4		Data;
	};

	struct SpecularStruct {
		D3DXVECTOR4		EffectStrength;
		D3DXVECTOR4		Data;
	};

	struct SkyStruct {
		D3DXVECTOR4		SkyData;
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
	ExposureStruct			Exposure;
	AnimatorsStruct			Animators;
	ShadowMapStruct			ShadowMap;
	OcclusionMapStruct		OcclusionMap;
	WaterStruct				Water;
	HDRStruct				HDR;
	GrassStruct				Grass;
	POMStruct				POM;
	TerrainStruct			Terrain;
	SkinStruct				Skin;
	ShadowStruct			Shadow;
	RainStruct				Rain;
	SnowStruct				Snow;
	WaterLensStruct			WaterLens;
	GodRaysStruct			GodRays;
	ImageAdjustStruct		ImageAdjust;
	DepthOfFieldStruct		DepthOfField;
	AmbientOcclusionStruct	AmbientOcclusion;
	ColoringStruct			Coloring;
	CinemaStruct			Cinema;
	LensStruct				Lens;
	BloomStruct				Bloom;
	SnowAccumulationStruct	SnowAccumulation;
	BloodLensStruct			BloodLens;
	MotionBlurStruct		MotionBlur;
	LowHFStruct				LowHF;
	WetWorldStruct			WetWorld;
	SharpeningStruct		Sharpening;
	SpecularStruct			Specular;
	SkyStruct				Sky;
	VolumetricFogStruct		VolumetricFog;
	D3DXVECTOR4				DebugVar;
};

struct ShaderValue {
	UInt32				RegisterIndex;
	UInt32				RegisterCount;
	union {
	D3DXVECTOR4*		Value;
	TextureRecord*		Texture;
	};
};

class ShaderProgram {
public:
	ShaderProgram();
	virtual ~ShaderProgram();
	
	virtual void			SetCT() = 0;
	virtual void			CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable) = 0;

	void					SetConstantTableValue(LPCSTR Name, UInt32 Index);
	static bool ShouldCompileShader(const char* fileBin, const char* fileHlsl, ShaderCompileType CompileStatus);

	ShaderValue*			FloatShaderValues;
	UInt32					FloatShaderValuesCount;
	ShaderValue*			TextureShaderValues;
	UInt32					TextureShaderValuesCount;
};

class ShaderRecord : public ShaderProgram {
public:
	ShaderRecord();
	virtual ~ShaderRecord();
	
	virtual void			SetCT();
 	virtual void			CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable);
	virtual void			SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount) = 0;

	static ShaderRecord*	LoadShader(const char* Name, const char* SubPath);

	bool					HasRenderedBuffer; 
	bool					HasDepthBuffer;
	const char*				Name;
};

class ShaderRecordVertex : public ShaderRecord {
public:
	ShaderRecordVertex(const char* shaderName);
	virtual ~ShaderRecordVertex();
	
	virtual void			SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount);

	IDirect3DVertexShader9* ShaderHandle;
};

class ShaderRecordPixel : public ShaderRecord {
public:
	ShaderRecordPixel(const char* shaderName);
	virtual ~ShaderRecordPixel();
	
	virtual void			SetShaderConstantF(UInt32 RegisterIndex, D3DXVECTOR4* Value, UInt32 RegisterCount);

	IDirect3DPixelShader9*	ShaderHandle;
};

class NiD3DVertexShaderEx : public NiD3DVertexShader {
public:
	void					SetupShader(IDirect3DVertexShader9* CurrentVertexHandle);
	void					DisposeShader();

	ShaderRecordVertex*		ShaderProg;
	ShaderRecordVertex*		ShaderProgE;
	ShaderRecordVertex*		ShaderProgI;
	IDirect3DVertexShader9*	ShaderHandleBackup;
	char					ShaderName[40];
};

class NiD3DPixelShaderEx : public NiD3DPixelShader {
public:
	void					SetupShader(IDirect3DPixelShader9* CurrentPixelHandle);
	void					DisposeShader();

	ShaderRecordPixel*		ShaderProg;
	ShaderRecordPixel*		ShaderProgE;
	ShaderRecordPixel*		ShaderProgI;
	IDirect3DPixelShader9*	ShaderHandleBackup;
	char					ShaderName[40];
};

class EffectRecord : public ShaderProgram {
public:
	EffectRecord(const char* effectName);
	virtual ~EffectRecord();
	
	virtual void			SetCT();
	virtual void			CreateCT(ID3DXBuffer* ShaderSource, ID3DXConstantTable* ConstantTable);
	bool					SwitchEffect();
	void					Render(IDirect3DDevice9* Device, IDirect3DSurface9* RenderTarget, IDirect3DSurface9* RenderedSurface, UINT techniqueIndex, bool ClearRenderTarget, bool useSourceBuffer);
	void					DisposeEffect();
	bool					LoadEffect(bool alwaysCompile = false); 
	
	static EffectRecord*	LoadEffect(const char* Name);
	bool 					IsLoaded();
	bool					Enabled;

	ID3DXEffect*			Effect;
	std::string*			Path;
	std::string*			SourcePath;
	const char*				Name;
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
	void					PrepareShaderIncludes();
	void					InitializeConstants();
	void					UpdateConstants();
	bool					CreateShader(const char* Name);
	bool					LoadShader(NiD3DVertexShader* VertexShader);
	bool					LoadShader(NiD3DPixelShader* PixelShader);
	void					DisposeShader(const char* Name);
	EffectRecord*			CreateEffect(const char* Name, bool setEnabled);
	void					DisposeEffect(EffectRecord** Effect);  // unused?
	void					RenderEffects(IDirect3DSurface9* RenderTarget);
	void					RenderEffectToRT(IDirect3DSurface9* RenderTarget, EffectRecord* Effect, bool clearRenderTarget);
	void					SwitchShaderStatus(const char* Name);
	void					SetCustomConstant(const char* Name, D3DXVECTOR4 Value);
	void					SetExtraEffectEnabled(const char* Name, bool Value);
	static float			lerp(float a, float b, float t);
	static float			invLerp(float a, float b, float t);
	static float			smoothStep(float a, float b, float t);
	static float			clamp(float a, float b, float t);
		
	struct	EffectsStruct {
		EffectRecord*		AvgLuma;
		EffectRecord*		AmbientOcclusion;
		EffectRecord*		BloodLens;
		EffectRecord*		Bloom;
		EffectRecord*		Coloring;
		EffectRecord*		Cinema;
		EffectRecord*		Exposure;
		EffectRecord*		DepthOfField;
		EffectRecord*		Debug;
		EffectRecord*		GodRays;
		EffectRecord*		ImageAdjust;
		EffectRecord*		Lens;
		EffectRecord*		LowHF;
		EffectRecord*		MotionBlur;
		EffectRecord*		Normals;
		EffectRecord*		Rain;
		EffectRecord*		Sharpening;
		EffectRecord*		Specular;
		EffectRecord*		Snow;
		EffectRecord*		SnowAccumulation;
		EffectRecord*		ShadowsExteriors;
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
	D3DXVECTOR4				LightPosition[TrackedLightsMax];
	D3DXVECTOR4				LightAttenuation[TrackedLightsMax];
};

