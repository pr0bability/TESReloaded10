#pragma once

class SkyShaders : public ShaderCollection
{
public:
	SkyShaders() : ShaderCollection("Sky") {};

	struct SkyStruct {
		D3DXVECTOR4		SkyData;
		D3DXVECTOR4		SunsetColor;
		D3DXVECTOR4		CloudData;
	};
	SkyStruct Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};