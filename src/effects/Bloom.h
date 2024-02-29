#pragma once

class BloomEffect : public EffectRecord
{
public:
	BloomEffect() : EffectRecord("Bloom") {};

	struct BloomSettings {
		float Treshold;
		float Scale;
		float Strength;
	};
	struct BloomSettingsStruct {
		D3DXVECTOR4		Resolution[6];
		BloomSettings	Main;
		BloomSettings	Night;
		BloomSettings	Interiors;
	};
	BloomSettingsStruct Settings;

	struct BloomStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Resolution;
	};
	BloomStruct		Constants;

	struct BloomTexturesStruct {
		IDirect3DTexture9* BloomTexture[6];
		IDirect3DSurface9* BloomSurface[6];
	};
	BloomTexturesStruct		Textures;

	void	UpdateConstants();
	void	RegisterConstants();
	void	RegisterTextures();
	void	UpdateSettings();
	bool	ShouldRender();

	void	RenderBloomBuffer(IDirect3DSurface9* RenderTarget);
};