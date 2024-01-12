#pragma once

class LensEffect : public EffectRecord
{
public:
	LensEffect() : EffectRecord("Lens") {};

	struct LensStruct {
		D3DXVECTOR4		Data;
	};
	LensStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};