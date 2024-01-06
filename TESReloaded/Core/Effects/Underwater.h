#pragma once

class UnderwaterEffect : public EffectRecord
{
public:
	UnderwaterEffect() : EffectRecord("Underwater") {};

	struct UnderwaterStruct {
		D3DXVECTOR4		Data;
	};
	UnderwaterStruct	Constants;

	void	UpdateConstants() {};
	void	RegisterConstants() {};
	void	UpdateSettings() {};
}; 