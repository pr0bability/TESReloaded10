#include "DepthOfField.h"

void DepthOfFieldEffect::UpdateConstants() {
	TheShaderManager->avglumaRequired = true;

	ValuesStruct* category = &Settings.FirstPerson;
	if (TheCameraManager->IsVanity())
		category = &Settings.VanityView;
	else if (!TheCameraManager->IsFirstPerson())
		category = &Settings.FirstPerson;

	int Mode = category->Mode;
	int dofActive = category->Enabled;

	// disable based on settings/context
	if ((Mode == 1 && (TheShaderManager->GameState.isDialog || TheShaderManager->GameState.isPersuasion)) ||
		(Mode == 2 && (!TheShaderManager->GameState.isDialog)) ||
		(Mode == 3 && (!TheShaderManager->GameState.isPersuasion)) ||
		(Mode == 4 && (!TheShaderManager->GameState.isDialog || !TheShaderManager->GameState.isPersuasion))) dofActive = 0;

	Constants.Enabled = dofActive;

	Constants.Blur.x = category->DistantBlur;
	Constants.Blur.y = category->DistantBlurStartRange;
	Constants.Blur.z = category->DistantBlurEndRange;
	Constants.Blur.w = category->BaseBlurRadius;
	Constants.Data.x = category->focusDistance * dofActive;
	Constants.Data.y = category->Radius;
	Constants.Data.z = category->DiameterRange;
	Constants.Data.w = category->NearBlurCutOff;

}

void DepthOfFieldEffect::UpdateSettings() {
	Settings.FirstPerson.DistantBlur = TheSettingManager->GetSettingF("Shaders.DepthOfField.FirstPersonView", "DistantBlur");
	Settings.FirstPerson.DistantBlurStartRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.FirstPersonView", "DistantBlurStartRange");
	Settings.FirstPerson.DistantBlurEndRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.FirstPersonView", "DistantBlurEndRange");
	Settings.FirstPerson.BaseBlurRadius = TheSettingManager->GetSettingF("Shaders.DepthOfField.FirstPersonView", "BaseBlurRadius");
	Settings.FirstPerson.focusDistance = TheSettingManager->GetSettingF("Shaders.DepthOfField.FirstPersonView", "BlurFallOff");
	Settings.FirstPerson.Radius = TheSettingManager->GetSettingF("Shaders.DepthOfField.FirstPersonView", "Radius");
	Settings.FirstPerson.DiameterRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.FirstPersonView", "DiameterRange");
	Settings.FirstPerson.NearBlurCutOff = TheSettingManager->GetSettingF("Shaders.DepthOfField.FirstPersonView", "NearBlurCutOff");
	Settings.FirstPerson.Mode = TheSettingManager->GetSettingI("Shaders.DepthOfField.FirstPersonView", "Mode");
	Settings.FirstPerson.Enabled = TheSettingManager->GetSettingI("Shaders.DepthOfField.FirstPersonView", "Enabled");

	Settings.ThirdPerson.DistantBlur = TheSettingManager->GetSettingF("Shaders.DepthOfField.ThirdPersonView", "DistantBlur");
	Settings.ThirdPerson.DistantBlurStartRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.ThirdPersonView", "DistantBlurStartRange");
	Settings.ThirdPerson.DistantBlurEndRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.ThirdPersonView", "DistantBlurEndRange");
	Settings.ThirdPerson.BaseBlurRadius = TheSettingManager->GetSettingF("Shaders.DepthOfField.ThirdPersonView", "BaseBlurRadius");
	Settings.ThirdPerson.focusDistance = TheSettingManager->GetSettingF("Shaders.DepthOfField.ThirdPersonView", "BlurFallOff");
	Settings.ThirdPerson.Radius = TheSettingManager->GetSettingF("Shaders.DepthOfField.ThirdPersonView", "Radius");
	Settings.ThirdPerson.DiameterRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.ThirdPersonView", "DiameterRange");
	Settings.ThirdPerson.NearBlurCutOff = TheSettingManager->GetSettingF("Shaders.DepthOfField.ThirdPersonView", "NearBlurCutOff");
	Settings.ThirdPerson.Mode = TheSettingManager->GetSettingI("Shaders.DepthOfField.ThirdPersonView", "Mode");
	Settings.ThirdPerson.Enabled = TheSettingManager->GetSettingI("Shaders.DepthOfField.ThirdPersonView", "Enabled");

	Settings.VanityView.DistantBlur = TheSettingManager->GetSettingF("Shaders.DepthOfField.VanityView", "DistantBlur");
	Settings.VanityView.DistantBlurStartRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.VanityView", "DistantBlurStartRange");
	Settings.VanityView.DistantBlurEndRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.VanityView", "DistantBlurEndRange");
	Settings.VanityView.BaseBlurRadius = TheSettingManager->GetSettingF("Shaders.DepthOfField.VanityView", "BaseBlurRadius");
	Settings.VanityView.focusDistance = TheSettingManager->GetSettingF("Shaders.DepthOfField.VanityView", "BlurFallOff");
	Settings.VanityView.Radius = TheSettingManager->GetSettingF("Shaders.DepthOfField.VanityView", "Radius");
	Settings.VanityView.DiameterRange = TheSettingManager->GetSettingF("Shaders.DepthOfField.VanityView", "DiameterRange");
	Settings.VanityView.NearBlurCutOff = TheSettingManager->GetSettingF("Shaders.DepthOfField.VanityView", "NearBlurCutOff");
	Settings.VanityView.Mode = TheSettingManager->GetSettingI("Shaders.DepthOfField.VanityView", "Mode");
	Settings.VanityView.Enabled = TheSettingManager->GetSettingI("Shaders.DepthOfField.VanityView", "Enabled");
}

void DepthOfFieldEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_DepthOfFieldBlur", &Constants.Blur);
	TheShaderManager->RegisterConstant("TESR_DepthOfFieldData", &Constants.Data);
}

bool DepthOfFieldEffect::ShouldRender() {
	return Constants.Enabled || Constants.Blur.x;
};