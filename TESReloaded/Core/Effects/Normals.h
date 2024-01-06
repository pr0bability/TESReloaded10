#pragma once

class NormalsEffect : public EffectRecord
{
public:
	NormalsEffect() : EffectRecord("Normals") {};

	struct NormalsStruct {
		D3DXVECTOR4		Data;
	};
	NormalsStruct	Constants;

	void	UpdateConstants() {};
	void	UpdateSettings() {};
	void	RegisterConstants();

};
