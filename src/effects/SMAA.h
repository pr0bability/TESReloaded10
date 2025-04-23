#pragma once

class SMAAEffect : public EffectRecord
{
public:
	SMAAEffect() : EffectRecord("SMAA") {};

	enum Input { INPUT_LUMA, INPUT_COLOR, INPUT_DEPTH };

	struct SMAASettings {
		Input EdgeDetection;
	};
	struct SMAASettingsStruct {
		SMAASettings	Main;
	};
	SMAASettingsStruct Settings;

	struct SMAAStruct {
		D3DXVECTOR4		Resolution;
	};
	SMAAStruct Constants;

	struct SMAATexturesStruct {
		IDirect3DTexture9* SMAA_Edges_Texture;
		IDirect3DSurface9* SMAA_Edges_Surface;
		IDirect3DTexture9* SMAA_Blend_Texture;
		IDirect3DSurface9* SMAA_Blend_Surface;
	};
	SMAATexturesStruct	Textures;

	void	UpdateConstants();
	void	RegisterConstants();
	void	RegisterTextures();
	void	UpdateSettings();

	void	Render(IDirect3DDevice9* Device, IDirect3DSurface9* RenderTarget, IDirect3DSurface9* RenderedSurface, UINT techniqueIndex, bool ClearRenderTarget, IDirect3DSurface9* SourceBuffer);

private:
	void EdgesDetectionPass(Input input);
	void BlendingWeightsCalculationPass();
	void NeighborhoodBlendingPass(IDirect3DSurface9* RenderTarget);
};
