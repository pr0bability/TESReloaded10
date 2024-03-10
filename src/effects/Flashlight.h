#pragma once

class FlashlightEffect : public EffectRecord
{
public:
	FlashlightEffect() : EffectRecord("Flashlight") {
		spotLightActive = false;
		SpotLight = nullptr;
	};

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
	bool	spotLightActive;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};