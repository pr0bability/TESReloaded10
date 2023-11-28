#include "MotionBlur.h"

void MotionBlurEffect::UpdateConstants() {
	const char* sectionName = "Shaders.MotionBlur.FirstPersonView";
	if (!TheCameraManager->IsFirstPerson()) sectionName = "Shaders.MotionBlur.ThirdPersonView";

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

	if (pow(fMotionBlurAmtX, 2) + pow(fMotionBlurAmtY, 2) < TheSettingManager->GetSettingF(sectionName, "BlurCutOff")) {
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
	Constants.BlurParams.x = TheSettingManager->GetSettingF(sectionName, "GaussianWeight");
	Constants.BlurParams.y = TheSettingManager->GetSettingF(sectionName, "BlurScale");
	Constants.BlurParams.z = TheSettingManager->GetSettingF(sectionName, "BlurOffsetMax");
}