#pragma once

class CinemaEffect : public EffectRecord
{
public:
	CinemaEffect() : EffectRecord("Cinema") {};

	struct CinemaStruct {
		D3DXVECTOR4		Data;
		D3DXVECTOR4		Settings;
	};

	float	Mode;
	float	aspectRatio;
	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();


	CinemaStruct	Constants;
};