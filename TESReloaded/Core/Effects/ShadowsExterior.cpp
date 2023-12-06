#include "ShadowsExterior.h"

void ShadowsExteriorEffect::UpdateConstants() {

	Constants.ShadowFade.x = 0; // Fade 1.0 == no shadows
	if (TheShaderManager->isExterior) {
		Constants.ShadowFade.x = TheShaderManager->smoothStep(0.3, 0, abs(TheShaderManager->dayLight - 0.5)); // fade shadows to 0 at sunrise/sunset.  

		TimeGlobals* GameTimeGlobals = TimeGlobals::Get();
		float DaysPassed = GameTimeGlobals->GameDaysPassed ? GameTimeGlobals->GameDaysPassed->data : 1.0f;

		if(!TheShaderManager->isDayTime) {
			// at night time, fade based on moonphase
			// moonphase goes from 0 to 8
			float MoonPhase = (fmod(DaysPassed, 8 * Tes->sky->firstClimate->phaseLength & 0x3F)) / (Tes->sky->firstClimate->phaseLength & 0x3F);

			float PI = 3.1416; // use cos curve to fade moon light shadows strength
			MoonPhase = std::lerp(-PI, PI, MoonPhase / 8) - PI / 4; // map moonphase to 1/2PI/2PI + 1/2

			// map MoonVisibility to MinNightDarkness/1 range
			float nightMinDarkness = 1 - TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.Main", "NightMinDarkness");
			float MoonVisibility = std::lerp((float)0.0, nightMinDarkness, (float)(cos(MoonPhase) * 0.5 + 0.5));
			Constants.ShadowFade.x = std::lerp(MoonVisibility, (float)1.0, Constants.ShadowFade.x);
		}

		if (TheShaderManager->isDayTimeChanged) {
			// pass the enabled/disabled property of the pointlight shadows to the shadowfade constant
			const char* PointLightsSettingName = (TheShaderManager->isDayTime > 0.5) ? "UsePointShadowsDay" : "UsePointShadowsNight";
			bool usePointLights = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.Main", PointLightsSettingName);
			Constants.ShadowFade.z = usePointLights;
		}
		Constants.ShadowFade.y = TheSettingManager->SettingsShadows.Exteriors.Enabled && Enabled;
		Constants.ShadowFade.w = TheShaderManager->ShaderConst.ShadowMap.ShadowMapRadius.w; //furthest distance for point lights shadows
	}
	else {
		// pass the enabled/disabled property of the shadow maps to the shadowfade constant
		Constants.ShadowFade.y = TheShaderManager->Effects.ShadowsInteriors->Enabled;
		Constants.ShadowFade.z = 1; // z enables point lights
		Constants.ShadowFade.w = TheSettingManager->SettingsShadows.Interiors.DrawDistance; //furthest distance for point lights shadows
	}

	if (TheSettingManager->SettingsChanged) {
		Constants.ScreenSpaceData.x = TheSettingManager->GetSettingI("Shaders.ShadowsExteriors.ScreenSpace", "Enabled") && Enabled;
		Constants.ScreenSpaceData.y = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "BlurRadius");
		Constants.ScreenSpaceData.z = TheSettingManager->GetSettingF("Shaders.ShadowsExteriors.ScreenSpace", "RenderDistance");
		//Constants.ScreenSpaceData.z = 0;
	}
}