#pragma once
#include <unordered_set>

struct ShadowAccumulator;

class RenderPass {
public:
	RenderPass() {};
	virtual ~RenderPass() {
		VertexShader = NULL;
		PixelShader = NULL;
	};

	ShaderRecordVertex* VertexShader;
	ShaderRecordPixel* PixelShader;

	std::unordered_set<NiGeometry*>	GeometryList;
	virtual bool AccumObject(NiGeometry* Geo) { return true; };
	virtual void UpdateConstants(NiGeometry* Geo) {};
	virtual void RenderGeometry(NiGeometry* Geo);
	virtual void RegisterConstants() {};

	void DrawGeometryBuffer(NiGeometry* Geo, NiGeometryBufferData* GeoData);
	void DrawSkinnedGeometryBuffer(NiGeometry* Geo, NiGeometryBufferData* GeoData, NiSkinPartition::Partition* Partition);
	void RenderAccum(const std::vector<NiGeometry*>& geometry);
};


class ShadowRenderPass : public RenderPass {
public:
	ShadowRenderPass();

	struct ConstantsStruct {
	};
	ConstantsStruct Constants;

	bool AccumObject(NiGeometry* Geo, bool* append);
	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
};

class AlphaShadowRenderPass : public RenderPass {
public:
	AlphaShadowRenderPass();

	struct ConstantsStruct {
		IDirect3DBaseTexture9* DiffuseMap;
	};
	ConstantsStruct Constants;

	bool AccumObject(NiGeometry* Geo, bool* append);
	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
};


class SkinnedGeoShadowRenderPass : public RenderPass {
public:
	SkinnedGeoShadowRenderPass();

	struct ConstantsStruct {
		D3DXVECTOR4* BoneMatrices;
	};
	ConstantsStruct Constants;

	bool AccumObject(NiGeometry* Geo, bool* append);
	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
	void RenderGeometry(NiGeometry* Geo);
};


class SpeedTreeShadowRenderPass : public RenderPass {
public:
	SpeedTreeShadowRenderPass();

	struct ConstantsStruct {
		D3DXVECTOR4* BillboardRight;
		D3DXVECTOR4* BillboardUp;
		D3DXVECTOR4* LeafBase;
		IDirect3DBaseTexture9* DiffuseMap;
	};
	ConstantsStruct Constants;

	bool AccumObject(NiGeometry* Geo, bool* append);
	void RegisterConstants(); 
	void UpdateConstants(NiGeometry* Geo);
};


class TerrainLODPass : public RenderPass {
public:
	TerrainLODPass();

	struct ConstantsStruct {
		D3DXMATRIX	WorldTranspose;
		D3DXVECTOR4 LODLandParams;
		D3DXVECTOR4 HighDetailRange;
	};
	ConstantsStruct Constants;

	bool AccumObject(NiGeometry* Geo, bool* append);
	void UpdateConstants(NiGeometry* Geo);
};