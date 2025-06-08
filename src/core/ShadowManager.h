#pragma once


enum ShadowMapTypeEnum {
	MapNear = 0,
	MapMiddle = 1,
	MapFar = 2,
	MapLod = 3,
	MapOrtho = 4,
};

enum class ShadowMapTypeMask : uint32_t {
	Near = 1 << 0,
	Middle = 1 << 1,
	Far = 1 << 2,
	Lod = 1 << 3,
	Ortho = 1 << 4,

	Primary = Near | Middle | Far,
};

struct ShadowAccumulator {
	ShadowMapTypeMask mask;

	std::vector<NiGeometry*> skinnedGeometry{ };
	std::vector<NiGeometry*> speedtreeGeometry{ };
	std::vector<NiGeometry*> terrainGeometry{ };
	std::vector<NiGeometry*> alphaGeometry{ };
	std::vector<NiGeometry*> geometry{ };

	ShadowsExteriorEffect::FormsStruct* pConfig;
	NiFrustumPlanes* pPlanes;
	ShadowsExteriorEffect::ShadowMapSettings* pShadowMap;
	D3DXMATRIX ViewProj;

	IDirect3DSurface9* pSurface;
	IDirect3DSurface9* pDepthSurface;

	uint32_t zIndex;

	void Init() {
		skinnedGeometry.reserve(1024);
		speedtreeGeometry.reserve(1024);
		terrainGeometry.reserve(1024);
		alphaGeometry.reserve(1024);
		geometry.reserve(1024);
	}
};

struct CullTask {
	std::vector<ShadowAccumulator> accums{};
	D3DXVECTOR3 SunDir{};
	uint32_t mask;

	void UpdateMask() {
		mask = {};
		for (const auto accum : accums) {
			mask |= static_cast<uint32_t>(accum.mask);
		}
	}

	uint32_t GetTerrainMask() const {
		uint32_t mask{};

		for (const auto accum : accums) {
			if (accum.pShadowMap->Forms.Terrain) {
				mask |= static_cast<uint32_t>(accum.mask);
			}
		}

		return mask & this->mask;
	}

	uint32_t GetLodMask() const {
		uint32_t mask{};

		for (const auto accum : accums) {
			if (accum.pShadowMap->Forms.Lod) {
				mask |= static_cast<uint32_t>(accum.mask);
			}
		}

		return mask & this->mask;
	}

	uint32_t GetMask() const {
		uint32_t mask{};

		for (const auto accum : accums) {
			mask |= static_cast<uint32_t>(accum.mask);
		}

		return mask;
	}
};

class ShadowManager {
	// Never disposed
public:
	static void Initialize();


	struct AccumNode {
		NiAVObject* pNode;
		uint32_t mask;
	};

	NiNode* GetRefNode(TESObjectREFR* Ref, ShadowsExteriorEffect::FormsStruct* Forms);
	void AccumChildren(std::vector<AccumNode>& nodes, CullTask* apTask, const bool abIsLand, const bool abIsLod);
	void AccumChildren(NiAVObject* apObject, CullTask* apTask, uint32_t mask, bool abIsLand, bool abIsLod);
	void AccumObject(NiGeometry* geo, CullTask* apTask, uint32_t mask, bool isLODLand);
	void RenderAccums(ShadowAccumulator& accum);
	void RenderShadowMap(CullTask* apTask, uint32_t mask);
	void AccumExteriorCell(CullTask* apTask, TESObjectCELL* Cell);
	void RenderShadowCubeMap(ShadowSceneLight** Lights, UInt32 LightIndex);
	void RenderShadowSpotlight(NiSpotLight** Lights, UInt32 LightIndex);
	void RenderShadowMaps();
	void ClearShadowCascade(D3DVIEWPORT9* ViewPort, D3DXVECTOR4* ClearColor);
	void BlurShadowAtlas();

	ShadowRenderPass* geometryPass;
	AlphaShadowRenderPass* alphaPass;
	SkinnedGeoShadowRenderPass* skinnedGeoPass;
	SpeedTreeShadowRenderPass* speedTreePass;
	TerrainLODPass* terrainLODPass;

	NiVector4 BillboardRight;
	NiVector4 BillboardUp;
	ShaderRecordVertex* ShadowMapVertex;
	ShaderRecordPixel* ShadowMapPixel;
	ShaderRecordVertex* ShadowCubeMapVertex;
	ShaderRecordPixel* ShadowCubeMapPixel;
	ShaderRecordVertex* ShadowMapBlurVertex;
	ShaderRecordPixel* ShadowMapBlurPixel;
	ShaderRecordPixel* ShadowMapClearPixel;
	D3DVIEWPORT9 ShadowCubeMapViewPort;
	ShaderRecordVertex* CurrentVertex;
	ShaderRecordPixel* CurrentPixel;
	bool AlphaEnabled;
	int PointLightsNum;
	float shadowMapsRenderTime;
	bool ShadowShadersLoaded;
	int FrameCounter;

private:
	bool CheckShaderFlags(NiGeometry* Geometry);
	void RecalculateBillboardVectors(D3DXVECTOR3* SunDir);
};
