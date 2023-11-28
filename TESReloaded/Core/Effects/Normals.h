#pragma once

class NormalsEffect : public EffectRecord
{
public:
	NormalsEffect() : EffectRecord("Normals") {};

	struct NormalsStruct {
	};
	NormalsStruct	Constants;

	void	UpdateConstants() {};
};
