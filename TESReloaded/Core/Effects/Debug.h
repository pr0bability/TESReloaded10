#pragma once

class DebugEffect : public EffectRecord
{
public:
	DebugEffect() : EffectRecord("Debug") {};

	struct DebugStruct {
	};
	DebugStruct	Constants;

	void	UpdateConstants();

};