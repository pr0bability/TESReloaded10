#include "Coloring.h"

void ColoringEffect::UpdateConstants() {
	SettingsColoringStruct* scs = TheSettingManager->GetSettingsColoring(Player->parentCell->GetEditorName());
	if (!scs && TheShaderManager->GameState.isExterior) scs = TheSettingManager->GetSettingsColoring(Player->GetWorldSpace()->GetEditorName());
	if (!scs) scs = TheSettingManager->GetSettingsColoring("Default");

	Constants.Data.x = scs->Strength;
	Constants.Data.y = scs->BaseGamma;
	Constants.Data.z = scs->Fade;
	Constants.Data.w = scs->Contrast;
	Constants.Values.x = scs->Saturation;
	Constants.Values.y = scs->Bleach;
	Constants.Values.z = scs->BleachLuma;
	Constants.Values.w = scs->Linearization;
	Constants.ColorCurve.x = scs->ColorCurve;
	Constants.ColorCurve.y = scs->ColorCurveR;
	Constants.ColorCurve.z = scs->ColorCurveG;
	Constants.ColorCurve.w = scs->ColorCurveB;
	Constants.EffectGamma.x = scs->EffectGamma;
	Constants.EffectGamma.y = scs->EffectGammaR;
	Constants.EffectGamma.z = scs->EffectGammaG;
	Constants.EffectGamma.w = scs->EffectGammaB;
}

void ColoringEffect::UpdateSettings() {}

void ColoringEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_ColoringColorCurve", &Constants.ColorCurve);
	TheShaderManager->RegisterConstant("TESR_ColoringEffectGamma", &Constants.EffectGamma);
	TheShaderManager->RegisterConstant("TESR_ColoringData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_ColoringValues", &Constants.Values);
}