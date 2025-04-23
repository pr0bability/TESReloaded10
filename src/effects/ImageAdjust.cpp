#include "ImageAdjust.h"

void ImageAdjustEffect::UpdateConstants() {
	Constants.Data.x = TheShaderManager->GetTransitionValue(Settings.Main.Brightness, Settings.Night.Brightness, Settings.Interiors.Brightness);
	Constants.Data.y = TheShaderManager->GetTransitionValue(Settings.Main.Contrast, Settings.Night.Contrast, Settings.Interiors.Contrast);
	Constants.Data.z = TheShaderManager->GetTransitionValue(Settings.Main.Saturation, Settings.Night.Saturation, Settings.Interiors.Saturation);
	Constants.Data.w = TheShaderManager->GetTransitionValue(Settings.Main.Strength, Settings.Night.Strength, Settings.Interiors.Strength);
	
	Constants.DarkColor.x = TheShaderManager->GetTransitionValue(Settings.Main.DarkColorR, Settings.Night.DarkColorR, Settings.Interiors.DarkColorR);
	Constants.DarkColor.y = TheShaderManager->GetTransitionValue(Settings.Main.DarkColorG, Settings.Night.DarkColorG, Settings.Interiors.DarkColorG);
	Constants.DarkColor.z = TheShaderManager->GetTransitionValue(Settings.Main.DarkColorB, Settings.Night.DarkColorB, Settings.Interiors.DarkColorB);

	Constants.LightColor.x = TheShaderManager->GetTransitionValue(Settings.Main.LightColorR, Settings.Night.LightColorR, Settings.Interiors.LightColorR);
	Constants.LightColor.y = TheShaderManager->GetTransitionValue(Settings.Main.LightColorG, Settings.Night.LightColorG, Settings.Interiors.LightColorG);
	Constants.LightColor.z = TheShaderManager->GetTransitionValue(Settings.Main.LightColorB, Settings.Night.LightColorB, Settings.Interiors.LightColorB);
}

void ImageAdjustEffect::UpdateSettings(){
	Settings.Main.Brightness = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "Brightness");
	Settings.Main.Contrast = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "Contrast");
	Settings.Main.Saturation = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "Saturation");
	Settings.Main.Strength = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "Strength");
	Settings.Main.DarkColorR = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "DarkColorR");
	Settings.Main.DarkColorG = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "DarkColorG");
	Settings.Main.DarkColorB = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "DarkColorB");
	Settings.Main.LightColorR = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "LightColorR");
	Settings.Main.LightColorG = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "LightColorG");
	Settings.Main.LightColorB = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Main", "LightColorB");

	Settings.Night.Brightness = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "Brightness");
	Settings.Night.Contrast = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "Contrast");
	Settings.Night.Saturation = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "Saturation");
	Settings.Night.Strength = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "Strength");
	Settings.Night.DarkColorR = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "DarkColorR");
	Settings.Night.DarkColorG = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "DarkColorG");
	Settings.Night.DarkColorB = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "DarkColorB");
	Settings.Night.LightColorR = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "LightColorR");
	Settings.Night.LightColorG = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "LightColorG");
	Settings.Night.LightColorB = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Night", "LightColorB");

	Settings.Interiors.Brightness = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "Brightness");
	Settings.Interiors.Contrast = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "Contrast");
	Settings.Interiors.Saturation = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "Saturation");
	Settings.Interiors.Strength = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "Strength");
	Settings.Interiors.DarkColorR = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "DarkColorR");
	Settings.Interiors.DarkColorG = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "DarkColorG");
	Settings.Interiors.DarkColorB = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "DarkColorB");
	Settings.Interiors.LightColorR = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "LightColorR");
	Settings.Interiors.LightColorG = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "LightColorG");
	Settings.Interiors.LightColorB = TheSettingManager->GetSettingF("Shaders.ImageAdjust.Interiors", "LightColorB");
}

void ImageAdjustEffect::RegisterConstants() {
	TheShaderManager->RegisterConstant("TESR_ImageAdjustData", &Constants.Data);
	TheShaderManager->RegisterConstant("TESR_DarkAdjustColor", &Constants.DarkColor);
	TheShaderManager->RegisterConstant("TESR_LightAdjustColor", &Constants.LightColor);

}