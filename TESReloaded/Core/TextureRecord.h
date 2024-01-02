#pragma once

class TextureRecord {
public:
	TextureRecord();

	enum TextureRecordType {
		None,
		PlanarBuffer,
		VolumeBuffer,
		CubeBuffer,
		SourceBuffer,
		RenderedBuffer,
		DepthBuffer,
		NormalsBuffer,
		AvgLumaBuffer,
		ShadowMapBufferNear,
		ShadowMapBufferMiddle,
		ShadowMapBufferFar,
		ShadowMapBufferLod,
		OrthoMapBuffer,
		ShadowCubeMapBuffer0,
		ShadowCubeMapBuffer1,
		ShadowCubeMapBuffer2,
		ShadowCubeMapBuffer3,
		ShadowCubeMapBuffer4,
		ShadowCubeMapBuffer5,
		ShadowCubeMapBuffer6,
		ShadowCubeMapBuffer7,
		ShadowCubeMapBuffer8,
		ShadowCubeMapBuffer9,
		ShadowCubeMapBuffer10,
		ShadowCubeMapBuffer11,
		PointShadowBuffer,
		WaterHeightMapBuffer, /*Textures assigned after init*/
		WaterReflectionMapBuffer,
	};
	static TextureRecordType	GetTextureType(UINT Type, const char* Name, bool* HasRenderedBuffer, bool* HasDepthBuffer);

	bool						LoadTexture(TextureRecordType Type, const char* Filename);

	IDirect3DBaseTexture9*		Texture;
	DWORD						SamplerStates[SamplerStatesMax];
};
