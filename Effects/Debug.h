#pragma once

class DebugEffect : public EffectRecord
{
public:
	DebugEffect() : EffectRecord("Debug") {};

	struct DebugStruct {
		D3DXVECTOR4				DebugVar;
	};
	DebugStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();

};