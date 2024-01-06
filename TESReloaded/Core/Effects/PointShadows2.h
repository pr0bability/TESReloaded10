#pragma once

class PointShadows2Effect : public EffectRecord
{
public:
	PointShadows2Effect() : EffectRecord("PointShadows2") {};

	struct PointShadows2Struct {
	};
	PointShadows2Struct	Constants;

	void	UpdateConstants() {};
	void	RegisterConstants() {};
	void	UpdateSettings() {};

};