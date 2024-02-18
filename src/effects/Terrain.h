#pragma once

class TerrainShaders : public ShaderCollection
{
public:
	TerrainShaders() : ShaderCollection("Terrain") {};

	struct TerrainStruct {
		D3DXVECTOR4		Data;
	};
	TerrainStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};