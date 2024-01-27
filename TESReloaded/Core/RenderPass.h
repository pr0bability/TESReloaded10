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

	bool AccumObject(NiGeometry* Geo);
	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
};

class SkinnedGeoShadowRenderPass : public RenderPass {
public:
	SkinnedGeoShadowRenderPass();

	bool AccumObject(NiGeometry* Geo);
	void RegisterConstants();
	void UpdateConstants(NiGeometry* Geo);
	void RenderGeometry(NiGeometry* Geo);
};


class SpeedTreeShadowRenderPass : public RenderPass {
public:
	SpeedTreeShadowRenderPass();

	bool AccumObject(NiGeometry* Geo);
	void RegisterConstants(); 
	void UpdateConstants(NiGeometry* Geo);
};