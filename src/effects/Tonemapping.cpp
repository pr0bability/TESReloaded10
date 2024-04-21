#include "Tonemapping.h"


void TonemappingShaders::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_HDRBloomData", &Constants.BloomData);
	TheShaderManager->RegisterConstant("TESR_HDRData", &Constants.HDRData);
	TheShaderManager->RegisterConstant("TESR_LotteData", &Constants.LotteData);
	TheShaderManager->RegisterConstant("TESR_ToneMapping", &Constants.ToneMapping);
}


void TonemappingShaders::UpdateConstants() {
	bool isExterior = TheShaderManager->GameState.isExterior;
	float transitionCurve = TheShaderManager->GameState.transitionCurve;

	if (TheSettingManager->SettingsChanged || TheShaderManager->GameState.isDayTimeChanged) {
		// Point light multiplier disabled for now
		//Constants.PointLightMult = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "PointLightMultiplier", isExterior, transitionCurve);
		Constants.ToneMapping.x = TheShaderManager->GetTransitionValue(Settings.Main.HighlightSaturation, Settings.Night.HighlightSaturation, Settings.Interiors.HighlightSaturation);
		Constants.ToneMapping.y = TheShaderManager->GetTransitionValue(Settings.Main.WeatherContrast, Settings.Night.WeatherContrast, Settings.Interiors.WeatherContrast);
		Constants.ToneMapping.z = TheShaderManager->GetTransitionValue(Settings.Main.ToneMappingColor, Settings.Night.ToneMappingColor, Settings.Interiors.ToneMappingColor);
		Constants.ToneMapping.w = TheShaderManager->GetTransitionValue(Settings.Main.Linearization, Settings.Night.Linearization, Settings.Interiors.Linearization);
		Constants.BloomData.x = TheShaderManager->GetTransitionValue(Settings.Main.BloomStrength, Settings.Night.BloomStrength, Settings.Interiors.BloomStrength);
		Constants.BloomData.y = TheShaderManager->GetTransitionValue(Settings.Main.BloomExponent, Settings.Night.BloomExponent, Settings.Interiors.BloomExponent);
		Constants.BloomData.z = TheShaderManager->GetTransitionValue(Settings.Main.WeatherModifier, Settings.Night.WeatherModifier, Settings.Interiors.WeatherModifier);
		Constants.BloomData.w = TheShaderManager->GetTransitionValue(Settings.Main.TonemapWhitePoint, Settings.Night.TonemapWhitePoint, Settings.Interiors.TonemapWhitePoint);
		Constants.HDRData.x = TheShaderManager->GetTransitionValue(Settings.Main.TonemappingMode, Settings.Night.TonemappingMode, Settings.Interiors.TonemappingMode);
		Constants.HDRData.y = TheShaderManager->GetTransitionValue(Settings.Main.Exposure, Settings.Night.Exposure, Settings.Interiors.Exposure);
		Constants.HDRData.z = TheShaderManager->GetTransitionValue(Settings.Main.Saturation, Settings.Night.Saturation, Settings.Interiors.Saturation);
		Constants.HDRData.w = TheShaderManager->GetTransitionValue(Settings.Main.Gamma, Settings.Night.Gamma, Settings.Interiors.Gamma);

		Constants.LotteData.x = TheShaderManager->GetTransitionValue(Settings.Main.TonemapContrast, Settings.Night.TonemapContrast, Settings.Interiors.TonemapContrast);
		Constants.LotteData.y = TheShaderManager->GetTransitionValue(Settings.Main.TonemapBrightness, Settings.Night.TonemapBrightness, Settings.Interiors.TonemapBrightness);
		Constants.LotteData.z = TheShaderManager->GetTransitionValue(Settings.Main.TonemapMidpoint, Settings.Night.TonemapMidpoint, Settings.Interiors.TonemapMidpoint);
		Constants.LotteData.w = TheShaderManager->GetTransitionValue(Settings.Main.TonemapShoulder, Settings.Night.TonemapShoulder, Settings.Interiors.TonemapShoulder);

		// Scale values for VTLottes/AMDLottes
		if (Constants.HDRData.x == 1.0) {
			float hdrMax = max(1.0, Constants.BloomData.w * 100.0);
			float contrast = max(0.01, Constants.LotteData.x * 1.35);
			float shoulder = max(0.0, (min(1.0, Constants.LotteData.w * 0.993))); // Shoulder should not! exceed 1.0
			float midIn = max(0.01, Constants.LotteData.z * 0.18);
			float midOut = max(0.01, (Constants.LotteData.y * 0.18) / shoulder);
			float colToneB = -((-pows(midIn, contrast) + (midOut * (pows(hdrMax, contrast * shoulder) * pows(midIn, contrast) -
				pows(hdrMax, contrast) * pows(midIn, contrast * shoulder) * midOut)) /
				(pows(hdrMax, contrast * shoulder) * midOut - pows(midIn, contrast * shoulder) * midOut)) /
				(pows(midIn, contrast * shoulder) * midOut));

			float colToneC = (pows(hdrMax, contrast * shoulder) * pows(midIn, contrast) - pows(hdrMax, contrast) * pows(midIn, contrast * shoulder) * midOut) /
				(pows(hdrMax, contrast * shoulder) * midOut - pows(midIn, contrast * shoulder) * midOut);

			Constants.LotteData.x = contrast;
			Constants.LotteData.w = shoulder;
			Constants.LotteData.z = colToneB;
			Constants.LotteData.y = colToneC;
		}
	}
}

void TonemappingShaders::UpdateSettings() {
	Settings.Main.HighlightSaturation = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "HighlightSaturation");
	Settings.Main.WeatherContrast = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "WeatherContrast");
	Settings.Main.ToneMappingColor = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "ToneMappingColor");
	Settings.Main.Linearization = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "Linearization");
	Settings.Main.BloomStrength = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "Blending");
	Settings.Main.BloomExponent = TheSettingManager->GetSettingF("Shaders.Bloom.Main", "BloomExponent");
	Settings.Main.WeatherModifier = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "WeatherModifier");
	Settings.Main.TonemapWhitePoint = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "TonemapWhitePoint");
	Settings.Main.TonemappingMode = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "TonemappingMode");
	Settings.Main.Exposure = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "Exposure");
	Settings.Main.Saturation = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "Saturation");
	Settings.Main.Gamma = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "Gamma");

	Settings.Main.TonemapContrast = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "TonemapContrast");
	Settings.Main.TonemapBrightness = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "TonemapBrightness");
	Settings.Main.TonemapMidpoint = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "TonemapMidpoint");
	Settings.Main.TonemapShoulder = TheSettingManager->GetSettingF("Shaders.Tonemapping.Main", "TonemapShoulder");

	Settings.Night.HighlightSaturation = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "HighlightSaturation");
	Settings.Night.WeatherContrast = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "WeatherContrast");
	Settings.Night.ToneMappingColor = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "ToneMappingColor");
	Settings.Night.Linearization = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "Linearization");
	Settings.Night.BloomStrength = TheSettingManager->GetSettingF("Shaders.Bloom.Night", "Blending");
	Settings.Night.BloomExponent = TheSettingManager->GetSettingF("Shaders.Bloom.Night", "BloomExponent");
	Settings.Night.WeatherModifier = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "WeatherModifier");
	Settings.Night.TonemapWhitePoint = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "TonemapWhitePoint");
	Settings.Night.TonemappingMode = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "TonemappingMode");
	Settings.Night.Exposure = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "Exposure");
	Settings.Night.Saturation = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "Saturation");
	Settings.Night.Gamma = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "Gamma");

	Settings.Night.TonemapContrast = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "TonemapContrast");
	Settings.Night.TonemapBrightness = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "TonemapBrightness");
	Settings.Night.TonemapMidpoint = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "TonemapMidpoint");
	Settings.Night.TonemapShoulder = TheSettingManager->GetSettingF("Shaders.Tonemapping.Night", "TonemapShoulder");

	Settings.Interiors.HighlightSaturation = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "HighlightSaturation");
	Settings.Interiors.WeatherContrast = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "WeatherContrast");
	Settings.Interiors.ToneMappingColor = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "ToneMappingColor");
	Settings.Interiors.Linearization = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "Linearization");
	Settings.Interiors.BloomStrength = TheSettingManager->GetSettingF("Shaders.Bloom.Interiors", "Blending");
	Settings.Interiors.BloomExponent = TheSettingManager->GetSettingF("Shaders.Bloom.Interiors", "BloomExponent");
	Settings.Interiors.WeatherModifier = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "WeatherModifier");
	Settings.Interiors.TonemapWhitePoint = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "TonemapWhitePoint");
	Settings.Interiors.TonemappingMode = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "TonemappingMode");
	Settings.Interiors.Exposure = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "Exposure");
	Settings.Interiors.Saturation = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "Saturation");
	Settings.Interiors.Gamma = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "Gamma");

	Settings.Interiors.TonemapContrast = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "TonemapContrast");
	Settings.Interiors.TonemapBrightness = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "TonemapBrightness");
	Settings.Interiors.TonemapMidpoint = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "TonemapMidpoint");
	Settings.Interiors.TonemapShoulder = TheSettingManager->GetSettingF("Shaders.Tonemapping.Interiors", "TonemapShoulder");

}