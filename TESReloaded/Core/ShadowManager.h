#pragma once
#include <stack>

#define ShadowsSettings ShadowsExteriorEffect::SettingsShadowStruct

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


class ShadowManager { // Never disposed
public:
	static void Initialize();
    
	enum ShadowMapTypeEnum {
		MapNear = 0,
		MapMiddle = 1,
		MapFar = 2,
		MapLod = 3,
		MapOrtho = 4,
	};

	enum PlaneEnum {
		PlaneNear	= 0,
		PlaneFar	= 1,
		PlaneLeft	= 2,
		PlaneRight	= 3,
		PlaneTop	= 4,
		PlaneBottom	= 5,
	};


	TESObjectREFR*			GetRef(TESObjectREFR* Ref, ShadowsSettings::FormsStruct* Forms, ShadowsSettings::ExcludedFormsList* ExcludedForms);
	void					AccumChildren(NiAVObject* NiObject, float MinRadius);
	void					AccumObject(std::stack<NiAVObject*>* containersAccum, NiAVObject* NiObject);
	void					RenderAccums();
	void					RenderGeometry(NiGeometry* Geo);
	void					RenderSkinnedGeometry(NiGeometry* Geo);
	void					RenderSpeedTreeGeometry(NiGeometry* Geo);
	void					DrawGeometryBuffer(NiGeometryBufferData* GeoData, UINT verticesCount);
	void					RenderShadowMap(ShadowMapTypeEnum ShadowMapType, ShadowsSettings::ExteriorsStruct* ShadowsExteriors, D3DXVECTOR3* At, D3DXVECTOR4* SunDir);
	void					RenderExteriorCell(TESObjectCELL* Cell, ShadowsSettings::ExteriorsStruct* ShadowsExteriors, ShadowMapTypeEnum ShadowMapType);
	void					RenderShadowCubeMap(ShadowSceneLight** Lights, UInt32 LightIndex, ShadowsSettings::InteriorsStruct* ShadowsInteriors);
	void					RenderShadowMaps();
    void                    BlurShadowMap(ShadowMapTypeEnum ShadowMapType);    

	std::stack<NiGeometry*> geometryAccum;
	std::stack<NiGeometry*> skinnedGeoAccum;
	std::stack<NiGeometry*> speedTreeAccum;

    ShaderRecordVertex*		ShadowMapVertex;
	ShaderRecordPixel*		ShadowMapPixel;
	D3DVIEWPORT9			ShadowMapViewPort[5];
	frustum					ShadowMapFrustum[5];
	NiVector4				BillboardRight;
	NiVector4				BillboardUp;
	ShaderRecordVertex*		ShadowCubeMapVertex;
	ShaderRecordPixel*		ShadowCubeMapPixel;
	int						PointLightsNum;

	float					shadowMapsRenderTime;
    
	float					ShadowCascadesDepth[3];

    ShaderRecordVertex*		ShadowMapBlurVertex;
	ShaderRecordPixel*		ShadowMapBlurPixel;
    IDirect3DVertexBuffer9* BlurShadowVertex[4];
    float                   ShadowMapInverseResolution[5];
    
	D3DVIEWPORT9			ShadowCubeMapViewPort;
	NiPointLight*			ShadowCubeMapLights[ShadowCubeMapsMax];
	ShaderRecordVertex*		CurrentVertex;
	ShaderRecordPixel*		CurrentPixel;
	bool					AlphaEnabled;
};