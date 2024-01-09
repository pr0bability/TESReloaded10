#pragma once

class TonemappingShaders : public ShaderCollection
{
public:
	TonemappingShaders() : ShaderCollection("Tonemapping") {};

	struct TonemappingStruct {
		D3DXVECTOR4		ToneMapping;
		D3DXVECTOR4		BloomData;
		D3DXVECTOR4		SkyData;
		D3DXVECTOR4		HDRData;
		D3DXVECTOR4		LotteData;
		float			PointLightMult;
	};
	TonemappingStruct Constants;
	
	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};