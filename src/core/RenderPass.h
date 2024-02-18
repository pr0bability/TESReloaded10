#pragma once

class RenderPass {
public:
	RenderPass() {};
	virtual ~RenderPass() {
		VertexShader = NULL;
		PixelShader = NULL;
	};

	ShaderRecordVertex* VertexShader;
	ShaderRecordPixel* PixelShader;

	std::stack<NiGeometry*>	GeometryList;
	virtual bool AccumObject(NiGeometry* Geo) { return true; };
	virtual void UpdateConstants(NiGeometry* Geo) {};
	virtual void RenderGeometry(NiGeometry* Geo);
	virtual void RegisterConstants() {};

	void DrawGeometryBuffer(NiGeometryBufferData* GeoData, UINT verticesCount);
	void RenderAccum();
};


class ShadowRenderPass : public RenderPass {
public:
	ShadowRenderPass();

	struct ConstantsStruct {
	};
	ConstantsStruct Constants;

	bool AccumObject(NiGeometry* Geo);
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

	bool AccumObject(NiGeometry* Geo);
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

	bool AccumObject(NiGeometry* Geo);
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

	bool AccumObject(NiGeometry* Geo);
	void RegisterConstants(); 
	void UpdateConstants(NiGeometry* Geo);
};