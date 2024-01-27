#pragma once


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


	TESObjectREFR*			GetRef(TESObjectREFR* Ref, ShadowsExteriorEffect::FormsStruct* Forms);
	void					AccumChildren(NiAVObject* NiObject, float MinRadius);
	void					AccumObject(std::stack<NiAVObject*>* containersAccum, NiAVObject* NiObject);
	void					RenderAccums();
	//void					RenderGeometry(NiGeometry* Geo);
	//void					RenderSkinnedGeometry(NiGeometry* Geo);
	//void					RenderSpeedTreeGeometry(NiGeometry* Geo);
//	void					DrawGeometryBuffer(NiGeometryBufferData* GeoData, UINT verticesCount);
	D3DXMATRIX				GetViewMatrix(D3DXVECTOR3* At, D3DXVECTOR4* Dir);
	void					RenderShadowMap(ShadowsExteriorEffect::ShadowMapSettings* ShadowMap, D3DMATRIX* ViewProj);
	void					RenderExteriorCell(TESObjectCELL* Cell, ShadowsExteriorEffect::ShadowMapSettings* ShadowMap);
	void					RenderShadowCubeMap(ShadowSceneLight** Lights, UInt32 LightIndex);
	void					RenderShadowMaps();
    void                    BlurShadowMap(ShadowsExteriorEffect::ShadowMapSettings* ShadowMap);

	ShadowRenderPass*				geometryPass;
	SkinnedGeoShadowRenderPass*		skinnedGeoPass;
	SpeedTreeShadowRenderPass*		speedTreePass;

	NiVector4				BillboardRight;
	NiVector4				BillboardUp;
	ShaderRecordVertex*		ShadowMapVertex;
	ShaderRecordPixel*		ShadowMapPixel;
	ShaderRecordVertex*		ShadowCubeMapVertex;
	ShaderRecordPixel*		ShadowCubeMapPixel;
    ShaderRecordVertex*		ShadowMapBlurVertex;
	ShaderRecordPixel*		ShadowMapBlurPixel;
	D3DVIEWPORT9			ShadowCubeMapViewPort;
	ShaderRecordVertex*		CurrentVertex;
	ShaderRecordPixel*		CurrentPixel;
	bool					AlphaEnabled;
	int						PointLightsNum;
	float					shadowMapsRenderTime;
};