#include "DepthOfField.h"

void DepthOfFieldEffect::UpdateConstants() {
	TheShaderManager->avglumaRequired = true;

	const char* sectionName = "Shaders.DepthOfField.FirstPersonView";
	if (TheCameraManager->IsVanity())
		sectionName = "Shaders.DepthOfField.VanityView";
	else if (!TheCameraManager->IsFirstPerson())
		sectionName = "Shaders.DepthOfField.ThirdPersonView";

	int Mode = TheSettingManager->GetSettingI(sectionName, "Mode");
	bool dofActive = TheSettingManager->GetSettingI(sectionName, "Enabled");

	// disable based on settings/context
	if ((Mode == 1 && (TheShaderManager->isDialog || TheShaderManager->isPersuasion)) ||
		(Mode == 2 && (!TheShaderManager->isDialog)) ||
		(Mode == 3 && (!TheShaderManager->isPersuasion)) ||
		(Mode == 4 && (!TheShaderManager->isDialog || !TheShaderManager->isPersuasion))) dofActive = false;

	Constants.Enabled = dofActive;

	if (dofActive) {
		Constants.Blur.x = TheSettingManager->GetSettingF(sectionName, "DistantBlur");
		Constants.Blur.y = TheSettingManager->GetSettingF(sectionName, "DistantBlurStartRange");
		Constants.Blur.z = TheSettingManager->GetSettingF(sectionName, "DistantBlurEndRange");
		Constants.Blur.w = TheSettingManager->GetSettingF(sectionName, "BaseBlurRadius");
		Constants.Data.x = TheSettingManager->GetSettingF(sectionName, "BlurFallOff");
		Constants.Data.y = TheSettingManager->GetSettingF(sectionName, "Radius");
		Constants.Data.z = TheSettingManager->GetSettingF(sectionName, "DiameterRange");
		Constants.Data.w = TheSettingManager->GetSettingF(sectionName, "NearBlurCutOff");
	}

}