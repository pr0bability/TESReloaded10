#pragma once

class DepthOfFieldEffect : public EffectRecord
{
public:
	DepthOfFieldEffect() : EffectRecord("DepthOfField") {};

	struct ValuesStruct {
		float DistantBlur;
		float DistantBlurStartRange;
		float DistantBlurEndRange;
		float BaseBlurRadius;
		float BlurFallOff;
		float Radius;
		float DiameterRange;
		float NearBlurCutOff;
		int Mode;
		bool Enabled;
	};

	struct DepthOfFieldSettingsStruct {
		ValuesStruct	FirstPerson;
		ValuesStruct	ThirdPerson;
		ValuesStruct	VanityView;
	};
	DepthOfFieldSettingsStruct Settings;

	struct DepthOfFieldStruct {
		bool			Enabled;
		D3DXVECTOR4		Blur;
		D3DXVECTOR4		Data;
	};
	DepthOfFieldStruct	Constants;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};