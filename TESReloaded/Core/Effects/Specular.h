#pragma once

class SpecularEffect : public EffectRecord
{
public:
	SpecularEffect() : EffectRecord("Specular") {};

	struct SpecularStruct {
		D3DXVECTOR4		EffectStrength;
		D3DXVECTOR4		Data;
	};
	SpecularStruct	Constants;

	struct SettingsSpecularStruct {
		struct ExteriorStruct {
			float SpecularStrength;
			float BlurMultiplier;
			float Glossiness;
			float DistanceFade;
			float FresnelStrength;
			float SkyTintStrength;
			float SpecLumaTreshold;
			float SkyTintSaturation;
		};

		struct RainStruct {
			float SpecularStrength;
			float BlurMultiplier;
			float Glossiness;
			float DistanceFade;
			float FresnelStrength;
			float SkyTintStrength;
			float SpecLumaTreshold;
			float SkyTintSaturation;
		};

		ExteriorStruct Exterior;
		RainStruct Rain;

	};
	SettingsSpecularStruct Settings;

	void	UpdateConstants();
	void	RegisterConstants();
	void	UpdateSettings();
	bool	ShouldRender();
};