#pragma once

class POMShaders : public ShaderCollection
{
public:
	POMShaders() : ShaderCollection("POM") {};

	struct POMStruct {
		D3DXVECTOR4		Data;
	};
	POMStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
};