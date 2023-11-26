#include "Cinema.h"

void CinemaEffect::UpdateConstants() {

	Constants.Data.x = 1.0f; // set cinema aspect ratio to native ar
	if (Enabled) {
		int Mode = TheSettingManager->GetSettingI("Shaders.Cinema.Main", "Mode");
		float aspectRatio = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "AspectRatio");

		// disable based on settings/context
		if ((Mode == 1 && (TheShaderManager->isDialog || TheShaderManager->isPersuasion)) ||
			(Mode == 2 && (!TheShaderManager->isDialog)) ||
			(Mode == 3 && (!TheShaderManager->isPersuasion)) ||
			(Mode == 4 && (!TheShaderManager->isDialog && !TheShaderManager->isPersuasion))) aspectRatio = 1.0f;

		Constants.Data.x = aspectRatio;
	}

	if (TheSettingManager->SettingsChanged) {
		Constants.Data.y = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "VignetteRadius");
		Constants.Data.z = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "VignetteDarkness");
		Constants.Data.w = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "OverlayStrength");
		Constants.Settings.y = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "FilmGrainAmount");
		Constants.Settings.z = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "ChromaticAberration");
		Constants.Settings.w = TheSettingManager->GetSettingF("Shaders.Cinema.Main", "LetterBoxDepth");
	}

}