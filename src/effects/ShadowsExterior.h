#pragma once

typedef struct frustum {
	enum PLANE {
		PLANENEAR = 0,
		PLANEFAR = 1,
		PLANELEFT = 2,
		PLANERIGHT = 3,
		PLANETOP = 4,
		PLANEBOTTOM = 5,
	};
	PLANE PLANE;
	D3DXPLANE plane[6];
} frustum;

class ShadowsExteriorEffect : public EffectRecord
{
public:
	ShadowsExteriorEffect() : EffectRecord("ShadowsExteriors") {};

	static const int Modes = 3;
	static const int FormatBits = 2;

	D3DFORMAT Formats[Modes][FormatBits] = {
		{D3DFMT_G16R16, D3DFMT_G32R32F},
		{D3DFMT_G16R16F, D3DFMT_G32R32F},
		{D3DFMT_A16B16G16R16F, D3DFMT_A32B32G32R32F},
	};
	
	enum ShadowMapTypeEnum {
		MapNear = 0,
		MapMiddle = 1,
		MapFar = 2,
		MapLod = 3,
		MapOrtho = 4,
	};

	//Constants
	struct ShadowStruct {
		D3DXVECTOR4		SmoothedSunDir;
		D3DXVECTOR4		Data;
		D3DXVECTOR4		FormatData;
		D3DXVECTOR4		ScreenSpaceData;
		D3DXVECTOR4		OrthoData;
		D3DXVECTOR4		ShadowFade;
		D3DXMATRIXA16	ShadowWorld;
		D3DXMATRIX		ShadowViewProj;
		D3DXMATRIX		ShadowCameraToLight[4];
		D3DXMATRIX		ShadowSpotlightCameraToLight[SpotLightsMax];
		D3DXVECTOR4		ShadowCubeMapLightPosition;
		D3DXVECTOR4		ShadowLightPosition[ShadowCubeMapsMax];
		D3DXVECTOR4		ShadowMapRadius;
	};

	// Settings
	struct FormsStruct {
		bool				AlphaEnabled;
		bool				Activators;
		bool				Actors;
		bool				Apparatus;
		bool				Books;
		bool				Containers;
		bool				Doors;
		bool				Furniture;
		bool				Misc;
		bool				Statics;
		bool				Terrain;
		bool				Trees;
		bool				Lod;
		float				MinRadius;
		float				OrigMinRadius;
	};

	struct ShadowMapSettings {
		D3DXMATRIX				ShadowCameraToLight;
		D3DVIEWPORT9			ShadowMapViewPort;
		frustum					ShadowMapFrustum;
		NiFrustumPlanes			ShadowMapFrustumPlanes;
		D3DXVECTOR4				ShadowMapCascadeCenterRadius;
		FormsStruct				Forms;
		float					ShadowMapInverseResolution;
		float					ShadowMapRadius;
		float					ShadowMapNear;
	};

	struct ShadowMapStruct {
		int					Mode;
		int					FormatBits;
		D3DFORMAT			Format;
		int					CascadeResolution;
		bool                Prefilter;
		bool				MSAA;
		bool				Mipmaps;
		int					Anisotropy;
		float				Distance;
		float				CascadeLambda;
		bool				LimitFrequency;
	};

	struct ExteriorsStruct {
		bool				Enabled;
		int					Quality;
		int					OrthoMapResolution;
		float				OrthoRadius;
		float				Darkness;
		float				NightMinDarkness;
		UInt8               ShadowMode;
		bool				UsePointShadowsDay;
		bool				UsePointShadowsNight;
	};

	struct InteriorsStruct {
		bool				Enabled;
		bool				TorchesCastShadows;
		FormsStruct			Forms;
		int					LightPoints;
		int					Quality;
		int					ShadowCubeMapSize;
		int					DrawDistance;
		float				Darkness;
		float				LightRadiusMult;
		bool				UseCastShadowFlag;
		bool				PlayerShadowThirdPerson;
		bool				PlayerShadowFirstPerson;
	};

	struct ScreenSpaceStruct {
		bool				Enabled;
		float				BlurRadius;
		float				RenderDistance;
	};

	struct SettingsShadowStruct {
		ShadowMapStruct     ShadowMaps;
		ScreenSpaceStruct	ScreenSpace;
		ExteriorsStruct		Exteriors;
		InteriorsStruct		Interiors;
	};
	SettingsShadowStruct	Settings;
	ShadowStruct			Constants;
	ShadowMapSettings		ShadowMaps[5];

	struct ShadowTextures {
		IDirect3DTexture9* ShadowPassTexture;
		IDirect3DSurface9* ShadowPassSurface;
		IDirect3DCubeTexture9* ShadowCubeMapTexture[ShadowCubeMapsMax];
		IDirect3DSurface9* ShadowCubeMapSurface[ShadowCubeMapsMax][6];
		IDirect3DTexture9* ShadowSpotlightTexture[SpotLightsMax];
		IDirect3DSurface9* ShadowSpotlightSurface[SpotLightsMax];
		IDirect3DSurface9* ShadowCubeMapDepthSurface;
	};
	ShadowTextures	Textures;

	// Main shadow atlas, used for cascades.
	IDirect3DTexture9* ShadowAtlasTexture;
	IDirect3DSurface9* ShadowAtlasSurface;
	IDirect3DSurface9* ShadowAtlasSurfaceMSAA;
	IDirect3DSurface9* ShadowAtlasDepthSurface;

	IDirect3DVertexBuffer9* ShadowAtlasVertexBuffer;
	float					ShadowAtlasCascadeTexelSize;

	// Ortho shadows for various effects.
	IDirect3DTexture9* ShadowMapOrthoTexture;
	IDirect3DSurface9* ShadowMapOrthoSurface;
	IDirect3DSurface9* ShadowMapOrthoDepthSurface;
	
	void		clearShadowsBuffer();
	void		UpdateConstants();
	void		UpdateSettings();
	void		RegisterConstants();
	void		RegisterTextures();
	void		RecreateTextures(bool cascades, bool ortho, bool cubemaps);

	D3DXVECTOR3	CalculateSmoothedSunDir();

	void		GetCascadeDepths();
	D3DXMATRIX	GetOrthoViewProj(D3DXMATRIX View);
	D3DXMATRIX	GetCascadeViewProj(ShadowMapSettings* ShadowMap, D3DXVECTOR3* SunDir);

private:
	bool		texturesInitialized;

	bool		UpdateSettingsFromQuality(int quality);
};
