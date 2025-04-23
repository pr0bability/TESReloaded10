#pragma once

class BloomEffect : public EffectRecord
{
public:
	BloomEffect() : EffectRecord("Bloom") {};

	static const int MaxPasses = 8;

	struct BloomSettings {
		float Strength;
		int Passes;
		float PassBlending;
	};
	struct BloomSettingsStruct {
		D3DXVECTOR4		Resolution[MaxPasses];
		BloomSettings	Main;
		BloomSettings	Night;
		BloomSettings	Interiors;
	};
	BloomSettingsStruct Settings;

	struct BloomStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ExtraData;
		D3DXVECTOR4		Resolution;
	};
	BloomStruct		Constants;

	struct BloomTexturesStruct {
		IDirect3DTexture9* BloomTexture[MaxPasses];
		IDirect3DSurface9* BloomSurface[MaxPasses];
		IDirect3DVertexBuffer9* BloomVertexBuffer[MaxPasses];
	};
	BloomTexturesStruct		Textures;

	void	UpdateConstants();
	void	RegisterConstants();
	void	RegisterTextures();
	void	UpdateSettings();
	bool    SwitchEffect();

	void    RenderPass(IDirect3DDevice9* Device, UINT techniqueIndex, bool ClearRenderTarget);
	void	RenderBloomBuffer(IDirect3DSurface9* RenderTarget);
};