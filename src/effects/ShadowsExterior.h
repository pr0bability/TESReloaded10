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
	
	enum ShadowMapTypeEnum {
		MapNear = 0,
		MapMiddle = 1,
		MapFar = 2,
		MapLod = 3,
		MapOrtho = 4,
	};

	struct ShadowStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ScreenSpaceData;
		D3DXVECTOR4		OrthoData;
		D3DXVECTOR4		ShadowFade;
	
		D3DXMATRIXA16	ShadowWorld;
		D3DXMATRIX		ShadowViewProj;
		D3DXMATRIX		ShadowCameraToLight[4];
		D3DXVECTOR4		ShadowCubeMapLightPosition;
		D3DXVECTOR4		ShadowLightPosition[ShadowCubeMapsMax];
		D3DXVECTOR4		ShadowCubeMapBlend;
		D3DXVECTOR4		ShadowMapRadius;
	};

	typedef std::vector<UInt32> ExcludedFormsList;

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
	};

	struct ShadowMapSettings {
		D3DXMATRIX				ShadowCameraToLight;
		IDirect3DTexture9*		ShadowMapTexture;
		IDirect3DSurface9*		ShadowMapSurface;
		IDirect3DSurface9*		ShadowMapDepthSurface;
		D3DVIEWPORT9			ShadowMapViewPort;
		frustum					ShadowMapFrustum;
		IDirect3DVertexBuffer9* BlurShadowVertexBuffer;
		FormsStruct				Forms;
		float					ShadowMapInverseResolution;
		float					ShadowMapRadius;
		float					ShadowMapNear;
	};

	struct ExteriorsStruct {
		bool				Enabled;
		bool                BlurShadowMaps;
		int					Quality;
		int					ShadowMapResolution;
		float				ShadowRadius;
		float				Darkness;
		float				NightMinDarkness;
		UInt8               ShadowMode;
		float				ShadowMapFarPlane;
		bool				UsePointShadowsDay;
		bool				UsePointShadowsNight;
		ExcludedFormsList	ExcludedForms;
	};

	struct InteriorsStruct {
		bool				Enabled;
		FormsStruct			Forms;
		bool				AlphaEnabled;
		bool				TorchesCastShadows;
		int					LightPoints;
		int					Quality;
		int					ShadowCubeMapSize;
		int					DrawDistance;
		float				Darkness;
		float				LightRadiusMult;
		bool				UseCastShadowFlag;
		ExcludedFormsList	ExcludedForms;
		bool				PlayerShadowThirdPerson;
		bool				PlayerShadowFirstPerson;
	};

	struct ScreenSpaceStruct {
		bool				Enabled;
		float				BlurRadius;
		float				RenderDistance;
	};

	struct SettingsShadowStruct {
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
		IDirect3DSurface9* ShadowCubeMapDepthSurface;
	};
	ShadowTextures	Textures;

	void		clearShadowsBuffer();
	void		UpdateConstants();
	void		UpdateSettings();
	void		RegisterConstants();
	void		RegisterTextures();

	void		GetCascadeDepths();
	D3DXMATRIX	GetOrthoViewProj(D3DXMATRIX View);
	D3DXMATRIX	GetCascadeViewProj(ShadowMapSettings* ShadowMap, D3DXMATRIX View);
};
