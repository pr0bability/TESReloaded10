#pragma once

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
		D3DXMATRIX		ShadowCameraToLight[5];
		D3DXVECTOR4		ShadowCubeMapLightPosition;
		D3DXVECTOR4		ShadowLightPosition[ShadowCubeMapsMax];
		D3DXVECTOR4		ShadowCubeMapBlend;
		D3DXVECTOR4		ShadowMapRadius;
	};

	struct SettingsShadowStruct {
		typedef std::vector<UInt32> ExcludedFormsList;

		struct FormsStruct {
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

		struct ExteriorsStruct {
			bool				Enabled;
			bool                BlurShadowMaps;
			FormsStruct			Forms[5];
			bool				AlphaEnabled[5];
			int					Quality;
			int					ShadowMapResolution;
			float				ShadowRadius;
			float				Darkness;
			float				NightMinDarkness;
			UInt8               ShadowMode;
			float				ShadowMapRadius[5];
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
		};

		struct ScreenSpaceStruct {
			bool				Enabled;
			float				BlurRadius;
			float				RenderDistance;
		};

		ScreenSpaceStruct	ScreenSpace;
		ExteriorsStruct		Exteriors;
		InteriorsStruct		Interiors;
	};
	SettingsShadowStruct	Settings;
	ShadowStruct			Constants;

	struct ShadowTextures {
		IDirect3DTexture9* ShadowPassTexture;
		IDirect3DSurface9* ShadowPassSurface;
		IDirect3DTexture9* ShadowMapTexture[5];
		IDirect3DSurface9* ShadowMapSurface[5];
		IDirect3DSurface9* ShadowMapDepthSurface[5];
		IDirect3DCubeTexture9* ShadowCubeMapTexture[ShadowCubeMapsMax];
		IDirect3DSurface9* ShadowCubeMapSurface[ShadowCubeMapsMax][6];
		IDirect3DSurface9* ShadowCubeMapDepthSurface;
	};
	ShadowTextures	Textures;

	void		UpdateConstants();
	void		UpdateSettings();
	void		RegisterConstants();
	void		RegisterTextures();

	void		GetCascadeDepths();
	D3DXMATRIX	GetCascadeViewProj(ShadowMapTypeEnum ShadowMapType, SettingsShadowStruct::ExteriorsStruct* ShadowsExteriors, D3DXMATRIX View);
};
