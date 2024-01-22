#include "Cinema.h"

void CinemaEffect::UpdateConstants() {

	Constants.Data.x = 1.0f; // set cinema aspect ratio to native ar
	if (Enabled) {
		// disable based on settings/context
		if ((Mode == 1 && (TheShaderManager->GameState.isDialog || TheShaderManager->GameState.isPersuasion)) ||
			(Mode == 2 && (!TheShaderManager->GameState.isDialog)) ||
			(Mode == 3 && (!TheShaderManager->GameState.isPersuasion)) ||
			(Mode == 4 && (!TheShaderManager->GameState.isDialog && !TheShaderManager->GameState.isPersuasion))) Constants.Data.x = 1.0f;
		else
			Constants.Data.x = aspectRatio;
	}

}

void CinemaEffect::UpdateSettings() {
	Mode = TheSettingManager->GetSettingI("Shaders.Cinema.Main", "Mode");
	aspectRatio = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "AspectRatio");
	Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "VignetteRadius");
	Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "VignetteDarkness");
	Constants.Data.w = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "OverlayStrength");
	Constants.Settings.y = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "FilmGrainAmount");
	Constants.Settings.z = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "ChromaticAberration");
	Constants.Settings.w = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "LetterBoxDepth");
}

void CinemaEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_CinemaData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_CinemaSettings", &Constants.Settings);
}