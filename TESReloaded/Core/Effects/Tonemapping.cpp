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
		Constants.ToneMapping.x = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "HighlightSaturation", isExterior, transitionCurve);
		Constants.ToneMapping.y = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "WeatherContrast", isExterior, transitionCurve);
		Constants.ToneMapping.z = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "ToneMappingColor", isExterior, transitionCurve);
		Constants.ToneMapping.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "Linearization", isExterior, transitionCurve);
		Constants.BloomData.x = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "BloomStrength", isExterior, transitionCurve);
		Constants.BloomData.y = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "BloomExponent", isExterior, transitionCurve);
		Constants.BloomData.z = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "WeatherModifier", isExterior, transitionCurve);
		Constants.BloomData.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapWhitePoint", isExterior, transitionCurve);
		Constants.HDRData.x = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemappingMode", isExterior, transitionCurve);
		Constants.HDRData.y = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "Exposure", isExterior, transitionCurve);
		Constants.HDRData.z = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "Saturation", isExterior, transitionCurve);
		Constants.HDRData.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "Gamma", isExterior, transitionCurve);

		Constants.LotteData.x = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapContrast", isExterior, transitionCurve);
		Constants.LotteData.y = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapBrightness", isExterior, transitionCurve);
		Constants.LotteData.z = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapMidpoint", isExterior, transitionCurve);
		Constants.LotteData.w = TheSettingManager->GetSettingTransition("Shaders.Tonemapping", "TonemapShoulder", isExterior, transitionCurve);

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

void TonemappingShaders::UpdateSettings() {}