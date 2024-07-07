#pragma once

bool (__cdecl* LoadForm)(TESForm*, UInt32) = (bool (__cdecl*)(TESForm*, UInt32))Hooks::LoadForm;
bool __cdecl LoadFormHook(TESForm* Form, UInt32 ModEntry) {
	
	bool r = LoadForm(Form, ModEntry);
	switch (Form->formType) {
		case TESForm::FormType::kFormType_Weather: {
				if (TheSettingManager->SettingsMain.WeatherMode.Enabled) {
					TESWeatherEx* Weather = (TESWeatherEx*)Form;
					memcpy(Weather->colorsb, Weather->colors, TESWeather::kNumColorTypes * TESWeather::kNumTimeOfDay * 4);
					TheSettingManager->SetSettingsWeather(Weather);
				}
			}
			break;
		default:
			break;
	}
	return r;

}