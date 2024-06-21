#pragma once

class TerrainShaders : public ShaderCollection
{
public:
	TerrainShaders() : ShaderCollection("Terrain") {};

	struct TerrainStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		ExtraData;
	};
	TerrainStruct	Constants;

	bool			usePBR;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};