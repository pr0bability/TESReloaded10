#include "MotionBlur.h"

void MotionBlurEffect::UpdateConstants() {
	ValuesStruct category = Settings.FirstPerson;
	if (!TheCameraManager->IsFirstPerson()) category = Settings.ThirdPerson;

	float AngleZ = D3DXToDegree(Player->rot.z);
	float AngleX = D3DXToDegree(Player->rot.x);
	float fMotionBlurAmtX = Constants.oldAngleZ - AngleZ;
	float fMotionBlurAmtY = Constants.oldAngleX - AngleX;
	float fBlurDistScratchpad = fMotionBlurAmtX + 360.0f;
	float fBlurDistScratchpad2 = (AngleZ - Constants.oldAngleZ + 360.0f) * -1.0f;

	if (abs(fMotionBlurAmtX) > abs(fBlurDistScratchpad))
		fMotionBlurAmtX = fBlurDistScratchpad;
	else if (abs(fMotionBlurAmtX) > abs(fBlurDistScratchpad2))
		fMotionBlurAmtX = fBlurDistScratchpad2;

	if (pow(fMotionBlurAmtX, 2) + pow(fMotionBlurAmtY, 2) < category.BlurCutOff) {
		fMotionBlurAmtX = 0.0f;
		fMotionBlurAmtY = 0.0f;
	}

	Constants.Data.x = (Constants.oldoldAmountX + Constants.oldAmountX + fMotionBlurAmtX) / 3.0f;
	Constants.Data.y = (Constants.oldoldAmountY + Constants.oldAmountY + fMotionBlurAmtY) / 3.0f;
	Constants.oldAngleZ = AngleZ;
	Constants.oldAngleX = AngleX;
	Constants.oldoldAmountX = Constants.oldAmountX;
	Constants.oldoldAmountY = Constants.oldAmountY;
	Constants.oldAmountX = fMotionBlurAmtX;
	Constants.oldAmountY = fMotionBlurAmtY;
	Constants.BlurParams.x = category.GaussianWeight;
	Constants.BlurParams.y = category.BlurScale;
	Constants.BlurParams.z = category.BlurOffsetMax;
}

void MotionBlurEffect::UpdateSettings() {
	Settings.FirstPerson.BlurCutOff = TheSettingManager->GetSettingF("Shaders.MotionBlur.FirstPersonView", "BlurCutOff");
	Settings.FirstPerson.GaussianWeight = TheSettingManager->GetSettingF("Shaders.MotionBlur.FirstPersonView", "GaussianWeight");
	Settings.FirstPerson.BlurScale = TheSettingManager->GetSettingF("Shaders.MotionBlur.FirstPersonView", "BlurScale");
	Settings.FirstPerson.BlurOffsetMax = TheSettingManager->GetSettingF("Shaders.MotionBlur.FirstPersonView", "BlurOffsetMax");

	Settings.ThirdPerson.BlurCutOff = TheSettingManager->GetSettingF("Shaders.MotionBlur.ThirdPersonView", "BlurCutOff");
	Settings.ThirdPerson.GaussianWeight = TheSettingManager->GetSettingF("Shaders.MotionBlur.ThirdPersonView", "GaussianWeight");
	Settings.ThirdPerson.BlurScale = TheSettingManager->GetSettingF("Shaders.MotionBlur.ThirdPersonView", "BlurScale");
	Settings.ThirdPerson.BlurOffsetMax = TheSettingManager->GetSettingF("Shaders.MotionBlur.ThirdPersonView", "BlurOffsetMax");
}

void MotionBlurEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_MotionBlurParams", &Constants.BlurParams);
	TheShaderManager->RegisterConstant("TESR_MotionBlurData", &Constants.Data);
}

bool MotionBlurEffect::ShouldRender()
{
	return Constants.Data.x || Constants.Data.y; 
};