#pragma once

class FlashlightEffect : public EffectRecord
{
public:
	FlashlightEffect() : EffectRecord("Flashlight") {};

	struct FlashlightSettingsStruct {
		NiColor		Color;
		float		Dimmer;
		float		ConeAngle;
		float		Distance;
	};
	FlashlightSettingsStruct	Settings;

	struct FlashlightStruct {
	};
	FlashlightStruct	Constants;

	NiSpotLight* SpotLight;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};