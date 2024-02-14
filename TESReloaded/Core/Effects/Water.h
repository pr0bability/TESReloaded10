#pragma once

class WaterShaders : public ShaderCollection
{
public:
	WaterShaders() : ShaderCollection("Water") {};

	struct WaterStruct {
		D3DXVECTOR4		waterCoefficients;
		D3DXVECTOR4		waveParams;
		D3DXVECTOR4		waterVolume;
		D3DXVECTOR4		waterSettings;
		D3DXVECTOR4		shorelineParams;
	};

	struct WaterConstants {
		WaterStruct		Default;
		WaterStruct		Placed;
		D3DXVECTOR4		Fog;
		D3DXVECTOR4		deepColor;
		D3DXVECTOR4		shallowColor;
		D3DXVECTOR4		LODColor;
	};
	WaterConstants		Constants;

	float	causticsStrength;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};