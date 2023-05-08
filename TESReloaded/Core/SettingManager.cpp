#include <../lib/tomlplusplus/include/toml++/toml.h>

void SettingManager::Configuration::Init() {

	char TomlFilename[MAX_PATH];
	char DefaultsFilename[MAX_PATH];

	configLoaded = false;
	GetCurrentDirectoryA(MAX_PATH, TomlFilename);
	GetCurrentDirectoryA(MAX_PATH, DefaultsFilename);
	strcat(TomlFilename, TomlSettingsFile);
	strcat(DefaultsFilename, DefaultsSettingsFile);

	Logger::Log("Loading settings from file %s", TomlFilename);
	Logger::Log("Loading defaults from file %s", DefaultsFilename);

	try {
		DefaultConfig = toml::parse_file((std::string_view)DefaultsFilename);

		// log config file contents
		//std::stringstream buffer;
		//buffer << DefaultConfig << std::endl;
		//Logger::Log("%s", buffer.str().c_str());
	}
	catch (const std::exception& e) {
		Logger::Log("error loading defaults toml: %s", e.what());
	}

	try {
		TomlConfig = toml::parse_file((std::string_view)TomlFilename);

		//// log config file contents
		//std::stringstream buffer;
		//buffer << TomlConfig << std::endl;
		//Logger::Log("%s", buffer.str().c_str());

	}
	catch(const std::exception& e){
		TomlConfig = toml::table();
		Logger::Log("error loading toml: %s, new config will be created from defaults.", e.what());
	}

	configLoaded = true;
}


/*
* Gets the value and type for the node based on section and key. If the section and key isn't found, value will be obtained from defaults.
*/ 
bool SettingManager::Configuration::FillNode(ConfigNode* Node, const char* Section, const char* Key) {
	
	std::string value; //at the moment settings are always stored as string. Should fix as it results in lots of conversions
	char path[MAX_PATH] = "_";
	strcat(path, Section);
	strcat(path, ".");
	strcat(path, Key);

	auto setting = TomlConfig.at_path(path);
	auto defaultSetting = DefaultConfig.at_path(path);
	bool fromDefault = false;

	//infer the type and convert the value to string, first from the settings, and if not found from the defaults.
	if (setting.is_integer()) {
		Node->Type = NodeType::Integer;
		value = ToString<int>((int)**setting.as_integer());
	}
	else if (setting.is_floating_point()) {
		Node->Type = NodeType::Float;
		value = ToString<float>((float)**setting.as_floating_point());
	}
	else if (setting.is_boolean()) {
		Node->Type = NodeType::Boolean;
		value = ToString<bool>((bool)**setting.as_boolean());
	}
	else if (setting.is_string()) {
		Node->Type = NodeType::String;
		value = *setting.value<std::string>();
	}
	else if (defaultSetting.is_integer()) {
		Node->Type = NodeType::Integer;
		value = ToString<int>((int)**defaultSetting.as_integer());
		fromDefault = true;
	}
	else if (defaultSetting.is_floating_point()) {
		Node->Type = NodeType::Float;
		value = ToString<float>((float)**defaultSetting.as_floating_point());
		fromDefault = true;
	}
	else if (defaultSetting.is_boolean()) {
		Node->Type = NodeType::Boolean;
		value = ToString<bool>((bool)**defaultSetting.as_boolean());
		fromDefault = true;
	}
	else if (defaultSetting.is_string()) {
		Node->Type = NodeType::String;
		value = *defaultSetting.value<std::string>();
		fromDefault = true;
	}
	else {
		// Key was not included in defaults, ignore
		return false;
	}

	// populate node fields
	strcpy(Node->Section, Section);
	strcpy(Node->Key, Key);
	strcpy(Node->Value, value.c_str());
	Node->Reboot = 0;

	//Logger::Log("FillNode %s value: %s (from defaults? %i)", path, value.c_str(), fromDefault);

	// write the value in case it was obtained from defaults
	if (fromDefault) SetValue(Node);

	return true;
}

/*
* Gathers the sub sections for a given parent section.
*/
void SettingManager::Configuration::FillSections(StringList* Sections, const char* ParentSection) {

	char path[256] = "_";
	toml::v3::table* sectionsTable = NULL;

	if (ParentSection == NULL) {
		sectionsTable = &DefaultConfig;
	}
	else{
		strcat(path, ParentSection); // add leading "_" TODO: Remove reliance on this leading "_"
		auto section = DefaultConfig.at_path(path);
		if (!section.is_table()) return; // table not found

		sectionsTable = DefaultConfig.at_path(path).as_table();
	}

	// iterate through the found keys in the table and adds the results to the list
	Sections->clear();
	for (auto& [key, value] : *sectionsTable) {
		const char* name = key.data();
		if (!memcmp(name, "_", 1)) name = name + 1; //discard first "_"
		Sections->push_back(name);
	}
}

/*
* Creates a list of settings node values from a given section
*/
void SettingManager::Configuration::FillSettings(SettingList* Nodes, const char* Section) {

	char path[256] = "_";
	strcat(path, Section);

	auto settingsTable = DefaultConfig.at_path(path);
	if (!settingsTable.is_table()) return; // table not found

	Nodes->clear();
	for (auto& [key, value] : *settingsTable.as_table()) {
		ConfigNode Node;
		FillNode(&Node, Section, key.data());
		Nodes->push_back(Node);
	}
}

/*
* Add the changes described by the node to the config. Will create the entry in the config if it only exists in defaults
*/
void SettingManager::Configuration::SetValue(ConfigNode* Node) {
	char path[256] = "_";
	strcat(path, Node->Section);

	auto section = TomlConfig.at_path(path);
	auto defaultSection = DefaultConfig.at_path(path);

	if (!defaultSection.is_table()) return; // setting not in defaults, ignore

	//setting values that don't exist in the config require building the section first.
	if (!section.is_table()) {
		// create the table
		StringList tables;
		SplitString(path, ".", &tables);
		auto table = &TomlConfig;
		for (auto address : tables) {
			// create table if not found
			if (!table->contains(address)) table->insert_or_assign(address, toml::v3::table());
			table = table->at_path(address).as_table();
		}
		section = TomlConfig.at_path(path);
	};

	// setting value based on type
	std::pair<toml::v3::table::iterator, bool> result;
	if (Node->Type == NodeType::Integer) {
		int value = atoi(Node->Value);
		result = section.as_table()->insert_or_assign(Node->Key, value);
	}else if (Node->Type == NodeType::Float) {
		float value = atof(Node->Value);
		result = section.as_table()->insert_or_assign(Node->Key, value);
	}else if (Node->Type == NodeType::String) {
		char* value = Node->Value;
		result = section.as_table()->insert_or_assign(Node->Key, value);
	}
	else if (Node->Type == NodeType::Boolean) {
		bool value = (bool)atoi(Node->Value);
		result = section.as_table()->insert_or_assign(Node->Key, value);
	}
}


void SettingManager::Configuration::CreateWeatherSection(const char* WeatherName, TESWeather* Weather) {
	// TODO: replace with toml

	/*
	char Section[4096];
	char SectionNode[256];
	char Value[80] = { NULL };
	SettingManager::Configuration::SectionPosition SectionPosition = GoToSection("Weathers");

	if (SectionPosition.Found) {
		strcpy(Section, "<");
		strcat(Section, WeatherName);
		strcat(Section, ">");
		strcat(Section, "<Colors>");
		for (UInt32 x = 0; x < TESWeather::kNumColorTypes; x++) {
			Value[0] = NULL;
			for (UInt32 y = 0; y < TESWeather::kNumTimeOfDay; y++) {
				strcat(Value, ToString(Weather->colors[x].colors[y].r).c_str());
				strcat(Value, ",");
				strcat(Value, ToString(Weather->colors[x].colors[y].g).c_str());
				strcat(Value, ",");
				strcat(Value, ToString(Weather->colors[x].colors[y].b).c_str());
				if (y < TESWeather::kNumTimeOfDay - 1) strcat(Value, " ");
			}
			sprintf(SectionNode, "<Color%i Value=\"%s\" Type=\"3\" Reboot=\"0\" Info=\"\" />", x, Value); strcat(Section, SectionNode);
		}
		strcat(Section, "</Colors>");
		strcat(Section, "<HDR>");
		Value[0] = NULL;
		for (UInt32 i = 0; i < 14; i++) {
			strcat(Value, ToString(Weather->GetHDR(i)).c_str());
			if (i < 13) strcat(Value, " ");
		}
		sprintf(SectionNode, "<HDR Value=\"%s\" Type=\"3\" Reboot=\"0\" Info=\"\" />", Value); strcat(Section, SectionNode);
		strcat(Section, "</HDR>");
		strcat(Section, "<Main>");
		sprintf(SectionNode, "<CloudSpeedLower Value=\"%i\" Type=\"1\" Reboot=\"0\" Info=\"\" />", Weather->GetCloudSpeedLower()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<CloudSpeedUpper Value=\"%i\" Type=\"1\" Reboot=\"0\" Info=\"\" />", Weather->GetCloudSpeedUpper()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<FogFarDay Value=\"%.1f\" Type=\"2\" Reboot=\"0\" Info=\"\" />", Weather->GetFogDayFar()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<FogFarNight Value=\"%.1f\" Type=\"2\" Reboot=\"0\" Info=\"\" />", Weather->GetFogNightFar()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<FogNearDay Value=\"%.1f\" Type=\"2\" Reboot=\"0\" Info=\"\" />", Weather->GetFogDayNear()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<FogNearNight Value=\"%.1f\" Type=\"2\" Reboot=\"0\" Info=\"\" />", Weather->GetFogNightNear()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<SunDamage Value=\"%i\" Type=\"1\" Reboot=\"0\" Info=\"\" />", Weather->GetSunDamage()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<SunGlare Value=\"%i\" Type=\"1\" Reboot=\"0\" Info=\"\" />", Weather->GetSunGlare()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<TransDelta Value=\"%i\" Type=\"1\" Reboot=\"0\" Info=\"\" />", Weather->GetTransDelta()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<WindSpeed Value=\"%i\" Type=\"1\" Reboot=\"0\" Info=\"\" />", Weather->GetWindSpeed()); strcat(Section, SectionNode);
		sprintf(SectionNode, "<UpperLayer Value=\"%s\" Type=\"3\" Reboot=\"0\" Info=\"\" />", Weather->textureLayers[0].ddsPath.m_data); strcat(Section, SectionNode);
		sprintf(SectionNode, "<LowerLayer Value=\"%s\" Type=\"3\" Reboot=\"0\" Info=\"\" />", Weather->textureLayers[1].ddsPath.m_data); strcat(Section, SectionNode);
		strcat(Section, "</Main>");
		strcat(Section, "</");
		strcat(Section, WeatherName);
		strcat(Section, ">");
		strncpy(ConfigB, Config, SectionPosition.End - Config);
		strcat(ConfigB, Section);
		strncat(ConfigB, SectionPosition.End, Config + strlen(Config) - SectionPosition.End);
		strcpy(Config, ConfigB);
		memset(ConfigB, NULL, FileSize);
	}*/

}

void SettingManager::Initialize() {

	Logger::Log("Starting the settings manager...");
	TheSettingManager = new SettingManager();

	TheSettingManager->GameLoading = false;

}

void SettingManager::LoadSettings() {

	auto timer = TimeLogger();

	StringList List;
	StringList InnerList;
	SettingsWaterStruct SW{};
	SettingsColoringStruct SC{};
	SettingsWeatherStruct SE{};
	char Value[80];

	if (!Config.configLoaded) Config.Init();

	SettingsMain.Main.RemoveUnderwater = GetSettingI("Main.Main.Water", "RemoveUnderwater");
	SettingsMain.Main.RemovePrecipitations = GetSettingI("Main.Main.Precipitations", "RemovePrecipitations");
	SettingsMain.Main.ForceReflections = GetSettingI("Main.Main.Water", "ForceReflections");
	SettingsMain.Main.MemoryHeapManagement = GetSettingI("Main.Main.Memory", "HeapManagement");
	SettingsMain.Main.MemoryTextureManagement = GetSettingI("Main.Main.Memory", "TextureManagement");
	SettingsMain.Main.AnisotropicFilter = GetSettingI("Main.Main.Misc", "AnisotropicFilter");
	SettingsMain.Main.FarPlaneDistance = GetSettingF("Main.Main.Misc", "FarPlaneDistance");
	SettingsMain.Main.ScreenshotKey = GetSettingI("Main.Main.Misc", "ScreenshotKey");
	SettingsMain.Main.ReplaceIntro = GetSettingI("Main.Main.Misc", "ReplaceIntro");
	SettingsMain.Main.ForceMSAA = GetSettingI("Main.Main.Misc", "ForceMSAA");
	SettingsMain.Main.SkipFog = GetSettingI("Main.Main.Misc", "SkipFog");

	SettingsMain.FrameRate.SmartControl = GetSettingI("Main.FrameRate.SmartControl", "SmartControl");
	SettingsMain.FrameRate.SmartControlFPS = GetSettingI("Main.FrameRate.SmartControl", "SmartControlFPS");
	SettingsMain.FrameRate.FlowControl = GetSettingF("Main.FrameRate.SmartControl", "FlowControl");
	SettingsMain.FrameRate.SmartBackgroundProcess = GetSettingI("Main.FrameRate.Stuttering", "SmartBackgroundProcess");
	SettingsMain.FrameRate.BackgroundThreadPriority = GetSettingI("Main.FrameRate.Stuttering", "BackgroundThreadPriority");

	SettingsMain.CullingProcess.EnableCulling = GetSettingI("Main.CullingProcess.Main", "EnableCulling");
	SettingsMain.CullingProcess.EnableReflectionCulling = GetSettingI("Main.CullingProcess.Main", "EnableReflectionCulling");
	SettingsMain.CullingProcess.CullMinSize = GetSettingF("Main.CullingProcess.Main", "CullMinSize");
	SettingsMain.CullingProcess.CullMinSizeReflection = GetSettingF("Main.CullingProcess.Main", "CullMinSizeReflection");

	SettingsMain.OcclusionCulling.Enabled = GetSettingI("Main.OcclusionCulling.Main", "Enabled");
	SettingsMain.OcclusionCulling.OcclusionMapRatio = GetSettingI("Main.OcclusionCulling.Main", "OcclusionMapRatio");
	SettingsMain.OcclusionCulling.OccludingStatic = GetSettingI("Main.OcclusionCulling.Main", "OccludingStatic");
	SettingsMain.OcclusionCulling.OccludedStatic = GetSettingI("Main.OcclusionCulling.Main", "OccludedStatic");
	SettingsMain.OcclusionCulling.OccludedDistantStatic = GetSettingI("Main.OcclusionCulling.Main", "OccludedDistantStatic");
	SettingsMain.OcclusionCulling.OccludedDistantStaticIC = GetSettingI("Main.OcclusionCulling.Main", "OccludedDistantStaticIC");
	SettingsMain.OcclusionCulling.OccludingStaticMin = GetSettingF("Main.OcclusionCulling.Main", "OccludingStaticMin");
	SettingsMain.OcclusionCulling.OccludedStaticMin = GetSettingF("Main.OcclusionCulling.Main", "OccludedStaticMin");
	SettingsMain.OcclusionCulling.OccludedStaticMax = GetSettingF("Main.OcclusionCulling.Main", "OccludedStaticMax");
	SettingsMain.OcclusionCulling.OccludedDistantStaticMax = GetSettingF("Main.OcclusionCulling.Main", "OccludedDistantStaticMax");

	SettingsMain.CameraMode.Enabled = GetSettingI("Main.CameraMode.Main", "Enabled");
	SettingsMain.CameraMode.Crosshair = GetSettingI("Main.CameraMode.Main", "Crosshair");
	SettingsMain.CameraMode.ChasingFirst = GetSettingI("Main.CameraMode.Main", "ChasingFirst");
	SettingsMain.CameraMode.ChasingThird = GetSettingI("Main.CameraMode.Main", "ChasingThird");
	SettingsMain.CameraMode.FoV = GetSettingF("Main.CameraMode.Main", "FoV");
	SettingsMain.CameraMode.Offset.x = GetSettingF("Main.CameraMode.Positioning", "OffsetX");
	SettingsMain.CameraMode.Offset.y = GetSettingF("Main.CameraMode.Positioning", "OffsetY");
	SettingsMain.CameraMode.Offset.z = GetSettingF("Main.CameraMode.Positioning", "OffsetZ");
	SettingsMain.CameraMode.DialogOffset.x = GetSettingF("Main.CameraMode.Positioning", "DialogOffsetX");
	SettingsMain.CameraMode.DialogOffset.y = GetSettingF("Main.CameraMode.Positioning", "DialogOffsetY");
	SettingsMain.CameraMode.DialogOffset.z = GetSettingF("Main.CameraMode.Positioning", "DialogOffsetZ");
	SettingsMain.CameraMode.AimingOffset.x = GetSettingF("Main.CameraMode.Positioning", "AimingOffsetX");
	SettingsMain.CameraMode.AimingOffset.y = GetSettingF("Main.CameraMode.Positioning", "AimingOffsetY");
	SettingsMain.CameraMode.AimingOffset.z = GetSettingF("Main.CameraMode.Positioning", "AimingOffsetZ");

	SettingsMain.IKFoot.Enabled = GetSettingI("Main.IKFoot.Main", "Enabled");
	SettingsMain.IKFoot.Coeff = GetSettingF("Main.IKFoot.Main", "Coeff");
	SettingsMain.IKFoot.CoeffThigh = GetSettingF("Main.IKFoot.Main", "CoeffThigh");
	SettingsMain.IKFoot.CoeffCalf = GetSettingF("Main.IKFoot.Main", "CoeffCalf");
	SettingsMain.IKFoot.CoeffFoot = GetSettingF("Main.IKFoot.Main", "CoeffFoot");
	SettingsMain.IKFoot.CoeffToe = GetSettingF("Main.IKFoot.Main", "CoeffToe");

	SettingsMain.EquipmentMode.Enabled = GetSettingI("Main.EquipmentMode.Main", "Enabled");
	SettingsMain.EquipmentMode.TorchKey = GetSettingI("Main.EquipmentMode.Main", "TorchKey");
	SettingsMain.EquipmentMode.SleepingEquipment = GetSettingI("Main.EquipmentMode.Main", "SleepingEquipment");
	SettingsMain.EquipmentMode.SwimmingEquipment = GetSettingI("Main.EquipmentMode.Main", "SwimmingEquipment");
	SettingsMain.EquipmentMode.CombatEquipmentKey = GetSettingI("Main.EquipmentMode.Main", "CombatEquipmentKey");
	SettingsMain.EquipmentMode.DualBlockDelay = GetSettingF("Main.EquipmentMode.Main", "DualBlockDelay");
	SettingsMain.EquipmentMode.ShieldOnBackPos.x = GetSettingF("Main.EquipmentMode.Positioning", "ShieldOnBackPosX");
	SettingsMain.EquipmentMode.ShieldOnBackPos.y = GetSettingF("Main.EquipmentMode.Positioning", "ShieldOnBackPosY");
	SettingsMain.EquipmentMode.ShieldOnBackPos.z = GetSettingF("Main.EquipmentMode.Positioning", "ShieldOnBackPosZ");
	SettingsMain.EquipmentMode.ShieldOnBackRot.x = GetSettingF("Main.EquipmentMode.Positioning", "ShieldOnBackRotX");
	SettingsMain.EquipmentMode.ShieldOnBackRot.y = GetSettingF("Main.EquipmentMode.Positioning", "ShieldOnBackRotY");
	SettingsMain.EquipmentMode.ShieldOnBackRot.z = GetSettingF("Main.EquipmentMode.Positioning", "ShieldOnBackRotZ");
	SettingsMain.EquipmentMode.WeaponOnBackPos.x = GetSettingF("Main.EquipmentMode.Positioning", "WeaponOnBackPosX");
	SettingsMain.EquipmentMode.WeaponOnBackPos.y = GetSettingF("Main.EquipmentMode.Positioning", "WeaponOnBackPosY");
	SettingsMain.EquipmentMode.WeaponOnBackPos.z = GetSettingF("Main.EquipmentMode.Positioning", "WeaponOnBackPosZ");
	SettingsMain.EquipmentMode.WeaponOnBackRot.x = GetSettingF("Main.EquipmentMode.Positioning", "WeaponOnBackRotX");
	SettingsMain.EquipmentMode.WeaponOnBackRot.y = GetSettingF("Main.EquipmentMode.Positioning", "WeaponOnBackRotY");
	SettingsMain.EquipmentMode.WeaponOnBackRot.z = GetSettingF("Main.EquipmentMode.Positioning", "WeaponOnBackRotZ");
	SettingsMain.EquipmentMode.TwoHandWeaponOnBackPos.x = GetSettingF("Main.EquipmentMode.Positioning", "TwoHandWeaponOnBackPosX");
	SettingsMain.EquipmentMode.TwoHandWeaponOnBackPos.y = GetSettingF("Main.EquipmentMode.Positioning", "TwoHandWeaponOnBackPosY");
	SettingsMain.EquipmentMode.TwoHandWeaponOnBackPos.z = GetSettingF("Main.EquipmentMode.Positioning", "TwoHandWeaponOnBackPosZ");
	SettingsMain.EquipmentMode.TwoHandWeaponOnBackRot.x = GetSettingF("Main.EquipmentMode.Positioning", "TwoHandWeaponOnBackRotX");
	SettingsMain.EquipmentMode.TwoHandWeaponOnBackRot.y = GetSettingF("Main.EquipmentMode.Positioning", "TwoHandWeaponOnBackRotY");
	SettingsMain.EquipmentMode.TwoHandWeaponOnBackRot.z = GetSettingF("Main.EquipmentMode.Positioning", "TwoHandWeaponOnBackRotZ");
	SettingsMain.EquipmentMode.BowOnBackPos.x = GetSettingF("Main.EquipmentMode.Positioning", "BowOnBackPosX");
	SettingsMain.EquipmentMode.BowOnBackPos.y = GetSettingF("Main.EquipmentMode.Positioning", "BowOnBackPosY");
	SettingsMain.EquipmentMode.BowOnBackPos.z = GetSettingF("Main.EquipmentMode.Positioning", "BowOnBackPosZ");
	SettingsMain.EquipmentMode.BowOnBackRot.x = GetSettingF("Main.EquipmentMode.Positioning", "BowOnBackRotX");
	SettingsMain.EquipmentMode.BowOnBackRot.y = GetSettingF("Main.EquipmentMode.Positioning", "BowOnBackRotY");
	SettingsMain.EquipmentMode.BowOnBackRot.z = GetSettingF("Main.EquipmentMode.Positioning", "BowOnBackRotZ");
	SettingsMain.EquipmentMode.StaffOnBackPos.x = GetSettingF("Main.EquipmentMode.Positioning", "StaffOnBackPosX");
	SettingsMain.EquipmentMode.StaffOnBackPos.y = GetSettingF("Main.EquipmentMode.Positioning", "StaffOnBackPosY");
	SettingsMain.EquipmentMode.StaffOnBackPos.z = GetSettingF("Main.EquipmentMode.Positioning", "StaffOnBackPosZ");
	SettingsMain.EquipmentMode.StaffOnBackRot.x = GetSettingF("Main.EquipmentMode.Positioning", "StaffOnBackRotX");
	SettingsMain.EquipmentMode.StaffOnBackRot.y = GetSettingF("Main.EquipmentMode.Positioning", "StaffOnBackRotY");
	SettingsMain.EquipmentMode.StaffOnBackRot.z = GetSettingF("Main.EquipmentMode.Positioning", "StaffOnBackRotZ");
	SettingsMain.EquipmentMode.TorchOnBeltPos.x = GetSettingF("Main.EquipmentMode.Positioning", "TorchOnBeltPosX");
	SettingsMain.EquipmentMode.TorchOnBeltPos.y = GetSettingF("Main.EquipmentMode.Positioning", "TorchOnBeltPosY");
	SettingsMain.EquipmentMode.TorchOnBeltPos.z = GetSettingF("Main.EquipmentMode.Positioning", "TorchOnBeltPosZ");
	SettingsMain.EquipmentMode.TorchOnBeltRot.x = GetSettingF("Main.EquipmentMode.Positioning", "TorchOnBeltRotX");
	SettingsMain.EquipmentMode.TorchOnBeltRot.y = GetSettingF("Main.EquipmentMode.Positioning", "TorchOnBeltRotY");
	SettingsMain.EquipmentMode.TorchOnBeltRot.z = GetSettingF("Main.EquipmentMode.Positioning", "TorchOnBeltRotZ");

	SettingsMain.MountedCombat.Enabled = GetSettingI("Main.MountedCombat.Main", "Enabled");
	SettingsMain.MountedCombat.WeaponOnBackPos.x = GetSettingF("Main.MountedCombat.Positioning", "WeaponOnBackPosX");
	SettingsMain.MountedCombat.WeaponOnBackPos.y = GetSettingF("Main.MountedCombat.Positioning", "WeaponOnBackPosY");
	SettingsMain.MountedCombat.WeaponOnBackPos.z = GetSettingF("Main.MountedCombat.Positioning", "WeaponOnBackPosZ");
	SettingsMain.MountedCombat.WeaponOnBackRot.x = GetSettingF("Main.MountedCombat.Positioning", "WeaponOnBackRotX");
	SettingsMain.MountedCombat.WeaponOnBackRot.y = GetSettingF("Main.MountedCombat.Positioning", "WeaponOnBackRotY");
	SettingsMain.MountedCombat.WeaponOnBackRot.z = GetSettingF("Main.MountedCombat.Positioning", "WeaponOnBackRotZ");
	SettingsMain.MountedCombat.TwoHandWeaponOnBackPos.x = GetSettingF("Main.MountedCombat.Positioning", "TwoHandWeaponOnBackPosX");
	SettingsMain.MountedCombat.TwoHandWeaponOnBackPos.y = GetSettingF("Main.MountedCombat.Positioning", "TwoHandWeaponOnBackPosY");
	SettingsMain.MountedCombat.TwoHandWeaponOnBackPos.z = GetSettingF("Main.MountedCombat.Positioning", "TwoHandWeaponOnBackPosZ");
	SettingsMain.MountedCombat.TwoHandWeaponOnBackRot.x = GetSettingF("Main.MountedCombat.Positioning", "TwoHandWeaponOnBackRotX");
	SettingsMain.MountedCombat.TwoHandWeaponOnBackRot.y = GetSettingF("Main.MountedCombat.Positioning", "TwoHandWeaponOnBackRotY");
	SettingsMain.MountedCombat.TwoHandWeaponOnBackRot.z = GetSettingF("Main.MountedCombat.Positioning", "TwoHandWeaponOnBackRotZ");
	SettingsMain.MountedCombat.BowOnBackPos.x = GetSettingF("Main.MountedCombat.Positioning", "BowOnBackPosX");
	SettingsMain.MountedCombat.BowOnBackPos.y = GetSettingF("Main.MountedCombat.Positioning", "BowOnBackPosY");
	SettingsMain.MountedCombat.BowOnBackPos.z = GetSettingF("Main.MountedCombat.Positioning", "BowOnBackPosZ");
	SettingsMain.MountedCombat.BowOnBackRot.x = GetSettingF("Main.MountedCombat.Positioning", "BowOnBackRotX");
	SettingsMain.MountedCombat.BowOnBackRot.y = GetSettingF("Main.MountedCombat.Positioning", "BowOnBackRotY");
	SettingsMain.MountedCombat.BowOnBackRot.z = GetSettingF("Main.MountedCombat.Positioning", "BowOnBackRotZ");
	SettingsMain.MountedCombat.StaffOnBackPos.x = GetSettingF("Main.MountedCombat.Positioning", "StaffOnBackPosX");
	SettingsMain.MountedCombat.StaffOnBackPos.y = GetSettingF("Main.MountedCombat.Positioning", "StaffOnBackPosY");
	SettingsMain.MountedCombat.StaffOnBackPos.z = GetSettingF("Main.MountedCombat.Positioning", "StaffOnBackPosZ");
	SettingsMain.MountedCombat.StaffOnBackRot.x = GetSettingF("Main.MountedCombat.Positioning", "StaffOnBackRotX");
	SettingsMain.MountedCombat.StaffOnBackRot.y = GetSettingF("Main.MountedCombat.Positioning", "StaffOnBackRotY");
	SettingsMain.MountedCombat.StaffOnBackRot.z = GetSettingF("Main.MountedCombat.Positioning", "StaffOnBackRotZ");

	SettingsMain.GrassMode.Enabled = GetSettingI("Main.GrassMode.Main", "Enabled");

	SettingsMain.SleepingMode.Enabled = GetSettingI("Main.SleepingMode.Main", "Enabled");
	SettingsMain.SleepingMode.Mode = GetSettingI("Main.SleepingMode.Main", "Mode");

	SettingsMain.WeatherMode.Enabled = GetSettingI("Main.WeatherMode.Main", "Enabled");
	SettingsMain.WeatherMode.CoeffNight.x = GetSettingF("Main.WeatherMode.General", "CoeffNightR");
	SettingsMain.WeatherMode.CoeffNight.y = GetSettingF("Main.WeatherMode.General", "CoeffNightG");
	SettingsMain.WeatherMode.CoeffNight.z = GetSettingF("Main.WeatherMode.General", "CoeffNightB");
	SettingsMain.WeatherMode.CoeffFog.x = GetSettingF("Main.WeatherMode.General", "CoeffFogR");
	SettingsMain.WeatherMode.CoeffFog.y = GetSettingF("Main.WeatherMode.General", "CoeffFogG");
	SettingsMain.WeatherMode.CoeffFog.z = GetSettingF("Main.WeatherMode.General", "CoeffFogB");
	SettingsMain.WeatherMode.CoeffSun.x = GetSettingF("Main.WeatherMode.General", "CoeffSunR");
	SettingsMain.WeatherMode.CoeffSun.y = GetSettingF("Main.WeatherMode.General", "CoeffSunG");
	SettingsMain.WeatherMode.CoeffSun.z = GetSettingF("Main.WeatherMode.General", "CoeffSunB");

	SettingsMain.ShadowMode.NearQuality = GetSettingF("Main.ShadowMode.Main", "NearQuality");

	strcpy(SettingsMain.Menu.TextFont, GetSettingS("Main.Menu.Style", "TextFont", Value));
	SettingsMain.Menu.TextSize = GetSettingI("Main.Menu.Style", "TextSize");
	strcpy(SettingsMain.Menu.TextFontStatus, GetSettingS("Main.Menu.Style", "TextFontStatus", Value));
	SettingsMain.Menu.TextSizeStatus = GetSettingI("Main.Menu.Style", "TextSizeStatus");
	FillFromString(GetSettingS("Main.Menu.Style", "TextColorNormal", Value), ",", SettingsMain.Menu.TextColorNormal);
	FillFromString(GetSettingS("Main.Menu.Style", "TextShadowColorNormal", Value), ",", SettingsMain.Menu.TextShadowColorNormal);
	FillFromString(GetSettingS("Main.Menu.Style", "TextColorSelected", Value), ",", SettingsMain.Menu.TextColorSelected);
	FillFromString(GetSettingS("Main.Menu.Style", "TextShadowColorSelected", Value), ",", SettingsMain.Menu.TextShadowColorSelected);
	FillFromString(GetSettingS("Main.Menu.Style", "TextColorEditing", Value), ",", SettingsMain.Menu.TextColorEditing);
	FillFromString(GetSettingS("Main.Menu.Style", "TextShadowColorEditing", Value), ",", SettingsMain.Menu.TextShadowColorEditing);
	FillFromString(GetSettingS("Main.Menu.Style", "TextColorEnabled", Value), ",", SettingsMain.Menu.TextColorEnabled);
	FillFromString(GetSettingS("Main.Menu.Style", "TextShadowColorEnabled", Value), ",", SettingsMain.Menu.TextShadowColorEnabled);
	SettingsMain.Menu.PositionX = GetSettingI("Main.Menu.Style", "PositionX");
	SettingsMain.Menu.PositionY = GetSettingI("Main.Menu.Style", "PositionY");
	SettingsMain.Menu.TitleColumnSize = GetSettingI("Main.Menu.Style", "TitleColumnSize");
	SettingsMain.Menu.MainItemColumnSize = GetSettingI("Main.Menu.Style", "MainItemColumnSize");
	SettingsMain.Menu.ItemColumnSize = GetSettingI("Main.Menu.Style", "ItemColumnSize");
	SettingsMain.Menu.RowSpace = GetSettingI("Main.Menu.Style", "RowSpace");
	SettingsMain.Menu.RowsPerPage = GetSettingI("Main.Menu.Style", "RowsPerPage");
	SettingsMain.Menu.KeyEnable = GetSettingI("Main.Menu.Keys", "KeyEnable");
	SettingsMain.Menu.KeyUp = GetSettingI("Main.Menu.Keys", "KeyUp");
	SettingsMain.Menu.KeyDown = GetSettingI("Main.Menu.Keys", "KeyDown");
	SettingsMain.Menu.KeyLeft = GetSettingI("Main.Menu.Keys", "KeyLeft");
	SettingsMain.Menu.KeyRight = GetSettingI("Main.Menu.Keys", "KeyRight");
	SettingsMain.Menu.KeyPageUp = GetSettingI("Main.Menu.Keys", "KeyPageUp");
	SettingsMain.Menu.KeyPageDown = GetSettingI("Main.Menu.Keys", "KeyPageDown");
	SettingsMain.Menu.KeyAdd = GetSettingI("Main.Menu.Keys", "KeyAdd");
	SettingsMain.Menu.KeySubtract = GetSettingI("Main.Menu.Keys", "KeySubtract");
	SettingsMain.Menu.KeySave = GetSettingI("Main.Menu.Keys", "KeySave");
	SettingsMain.Menu.KeyEditing = GetSettingI("Main.Menu.Keys", "KeyEditing");

	SettingsMain.Purger.Enabled = GetSettingI("Main.Purger.Main", "Enabled");
	SettingsMain.Purger.Time = GetSettingI("Main.Purger.Main", "Time");
	SettingsMain.Purger.PurgeTextures = GetSettingI("Main.Purger.Main", "PurgeTextures");
	SettingsMain.Purger.PurgeCells = GetSettingI("Main.Purger.Main", "PurgeCells");
	SettingsMain.Purger.Key = GetSettingI("Main.Purger.Main", "Key");

	SettingsMain.Gravity.Enabled = GetSettingI("Main.Gravity.Main", "Enabled");
	SettingsMain.Gravity.Value = GetSettingF("Main.Gravity.Main", "Value");

	SettingsMain.Dodge.Enabled = GetSettingI("Main.Dodge.Main", "Enabled");
	SettingsMain.Dodge.AcrobaticsLevel = GetSettingI("Main.Dodge.Main", "AcrobaticsLevel");
	SettingsMain.Dodge.DoubleTap = GetSettingI("Main.Dodge.Main", "DoubleTap");
	SettingsMain.Dodge.DoubleTapTime = GetSettingF("Main.Dodge.Main", "DoubleTapTime");

	SettingsMain.FlyCam.Enabled = GetSettingI("Main.FlyCam.Main", "Enabled");
	SettingsMain.FlyCam.ScrollMultiplier = GetSettingF("Main.FlyCam.Main", "ScrollMultiplier");
	SettingsMain.FlyCam.KeyAdd = GetSettingI("Main.FlyCam.Main", "KeyAdd");
	SettingsMain.FlyCam.KeySubtract = GetSettingI("Main.FlyCam.Main", "KeySubtract");
	SettingsMain.FlyCam.StepValue = GetSettingF("Main.FlyCam.Main", "StepValue");

	SettingsMain.Develop.CompileShaders = GetSettingI("Main.Develop.Main", "CompileShaders");
	SettingsMain.Develop.CompileEffects = GetSettingI("Main.Develop.Main", "CompileEffects");
	SettingsMain.Develop.DebugMode = GetSettingI("Main.Develop.Main", "DebugMode");
	SettingsMain.Develop.TraceShaders = GetSettingI("Main.Develop.Main", "TraceShaders");


	Config.FillSections(&List, "Weathers"); // get the list of weathers
	for (StringList::iterator Iter = List.begin(); Iter != List.end(); ++Iter) {
		const char* WeatherSection = Iter->c_str();
		char SectionName[80];
		char Key[12];
		UInt8 Color[3];
		strcpy(SectionName, "Weathers.");
		strcat(SectionName, WeatherSection);
		strcat(SectionName, ".Colors");
		for (int x = 0; x < TESWeather::kNumColorTypes; x++) {
			sprintf(Key, "Color%i", x);
			SplitString(GetSettingS(SectionName, Key, Value), " ", &InnerList);
			for (int y = 0; y < TESWeather::kNumTimeOfDay; y++) {
				FillFromString(InnerList[y].c_str(), ",", Color);
				SE.Colors[x].colors[y].r = Color[0];
				SE.Colors[x].colors[y].g = Color[1];
				SE.Colors[x].colors[y].b = Color[2];
			}
		}
		strcpy(SectionName, "Weathers.");
		strcat(SectionName, WeatherSection);
		strcat(SectionName, ".HDR");
		FillFromString(GetSettingS(SectionName, "HDR", Value), " ", SE.HDR);
		strcpy(SectionName, "Weathers.");
		strcat(SectionName, WeatherSection);
		strcat(SectionName, ".Main");
		SE.FogNearDay = GetSettingF(SectionName, "FogNearDay");
		SE.FogNearNight = GetSettingF(SectionName, "FogNearNight");
		SE.FogFarDay = GetSettingF(SectionName, "FogFarDay");
		SE.FogFarNight = GetSettingF(SectionName, "FogFarNight");
		SE.WindSpeed = GetSettingI(SectionName, "WindSpeed");
		SE.CloudSpeedLower = GetSettingI(SectionName, "CloudSpeedLower");
		SE.CloudSpeedUpper = GetSettingI(SectionName, "CloudSpeedUpper");
		SE.TransDelta = GetSettingI(SectionName, "TransDelta");
		SE.SunGlare = GetSettingI(SectionName, "SunGlare");
		SE.SunDamage = GetSettingI(SectionName, "SunDamage");
		strcpy(SE.LowerLayer, GetSettingS(SectionName, "LowerLayer", Value));
		strcpy(SE.UpperLayer, GetSettingS(SectionName, "UpperLayer", Value));
		SettingsWeather[WeatherSection] = SE;
	}

	Config.FillSections(&List, "Shaders.Coloring"); // get the list of coloring sections
	for (StringList::iterator Iter = List.begin(); Iter != List.end(); ++Iter) {
		const char* ColoringSection = Iter->c_str();
		if (strcmp(ColoringSection, "Status")) {
			char SectionName[80];
			strcpy(SectionName, "Shaders.Coloring.");
			strcat(SectionName, ColoringSection);
			SC.Strength = GetSettingF(SectionName, "Strength");
			SC.BaseGamma = GetSettingF(SectionName, "BaseGamma");
			SC.Fade = GetSettingF(SectionName, "Fade");
			SC.Contrast = GetSettingF(SectionName, "Contrast");
			SC.Saturation = GetSettingF(SectionName, "Saturation");
			SC.Bleach = GetSettingF(SectionName, "Bleach");
			SC.BleachLuma = GetSettingF(SectionName, "BleachLuma");
			SC.ColorCurve = GetSettingF(SectionName, "ColorCurve");
			SC.ColorCurveR = GetSettingF(SectionName, "ColorCurveR");
			SC.ColorCurveG = GetSettingF(SectionName, "ColorCurveG");
			SC.ColorCurveB = GetSettingF(SectionName, "ColorCurveB");
			SC.EffectGamma = GetSettingF(SectionName, "EffectGamma");
			SC.EffectGammaR = GetSettingF(SectionName, "EffectGammaR");
			SC.EffectGammaG = GetSettingF(SectionName, "EffectGammaG");
			SC.EffectGammaB = GetSettingF(SectionName, "EffectGammaB");
			SC.Linearization = GetSettingF(SectionName, "Linearization");
			SettingsColoring[ColoringSection] = SC;
		}
	}

	// Generic exterior shadows settings
	SettingsShadows.Exteriors.Enabled = GetSettingI("Shaders.ShadowsExteriors.Main", "Enabled");
	SettingsShadows.Exteriors.Quality = GetSettingI("Shaders.ShadowsExteriors.Main", "Quality");
	SettingsShadows.Exteriors.Darkness = GetSettingF("Shaders.ShadowsExteriors.Main", "Darkness");
	SettingsShadows.Exteriors.NightMinDarkness = GetSettingF("Shaders.ShadowsExteriors.Main", "NightMinDarkness");
	SettingsShadows.Exteriors.ShadowRadius = GetSettingF("Shaders.ShadowsExteriors.Main", "ShadowsRadius");
	SettingsShadows.Exteriors.ShadowMapResolution = GetSettingI("Shaders.ShadowsExteriors.Main", "ShadowMapResolution");
	SettingsShadows.Exteriors.ShadowMapFarPlane = GetSettingF("Shaders.ShadowsExteriors.Main", "ShadowMapFarPlane");
	SettingsShadows.Exteriors.ShadowMode = GetSettingI("Shaders.ShadowsExteriors.Main", "ShadowMode");
	SettingsShadows.Exteriors.BlurShadowMaps = GetSettingI("Shaders.ShadowsExteriors.Main", "BlurShadowMaps");

	SettingsShadows.ScreenSpace.Enabled = GetSettingI("Shaders.ShadowsExteriors.ScreenSpace", "Enabled");
	SettingsShadows.ScreenSpace.BlurRadius = GetSettingI("Shaders.ShadowsExteriors.ScreenSpace", "BlurRadius");
	SettingsShadows.ScreenSpace.RenderDistance = GetSettingI("Shaders.ShadowsExteriors.ScreenSpace", "RenderDistance");

	//Shadows Cascade settings
	for (int shadowType = 0; shadowType <= ShadowManager::ShadowMapTypeEnum::MapOrtho; shadowType++) {
		char sectionName[256] = "Shaders.ShadowsExteriors.";
		switch (shadowType) {
		case ShadowManager::ShadowMapTypeEnum::MapNear:
			strcat(sectionName, "Near");
			break;
		case ShadowManager::ShadowMapTypeEnum::MapMiddle:
			strcat(sectionName, "Middle");
			break;
		case ShadowManager::ShadowMapTypeEnum::MapFar:
			strcat(sectionName, "Far");
			break;
		case ShadowManager::ShadowMapTypeEnum::MapLod:
			strcat(sectionName, "Lod");
			break;
		case ShadowManager::ShadowMapTypeEnum::MapOrtho:
			strcat(sectionName, "Ortho");
			break;
		}

		SettingsShadows.Exteriors.AlphaEnabled[shadowType] = GetSettingI(sectionName, "AlphaEnabled");
		SettingsShadows.Exteriors.Forms[shadowType].Activators = GetSettingI(sectionName, "Activators");
		SettingsShadows.Exteriors.Forms[shadowType].Actors = GetSettingI(sectionName, "Actors");
		SettingsShadows.Exteriors.Forms[shadowType].Apparatus = GetSettingI(sectionName, "Apparatus");
		SettingsShadows.Exteriors.Forms[shadowType].Books = GetSettingI(sectionName, "Books");
		SettingsShadows.Exteriors.Forms[shadowType].Containers = GetSettingI(sectionName, "Containers");
		SettingsShadows.Exteriors.Forms[shadowType].Doors = GetSettingI(sectionName, "Doors");
		SettingsShadows.Exteriors.Forms[shadowType].Furniture = GetSettingI(sectionName, "Furniture");
		SettingsShadows.Exteriors.Forms[shadowType].Misc = GetSettingI(sectionName, "Misc");
		SettingsShadows.Exteriors.Forms[shadowType].Statics = GetSettingI(sectionName, "Statics");
		SettingsShadows.Exteriors.Forms[shadowType].Terrain = GetSettingI(sectionName, "Terrain");
		SettingsShadows.Exteriors.Forms[shadowType].Trees = GetSettingI(sectionName, "Trees");
		SettingsShadows.Exteriors.Forms[shadowType].Lod = GetSettingI(sectionName, "Lod");
	};

	Config.FillSections(&List, "Shaders.ShadowsExteriors.ExcludedFormID"); // get the list of excluded formIDs
	if (List.size()) SettingsShadows.Exteriors.ExcludedForms.reserve(List.size());
	for (StringList::iterator Iter = List.begin(); Iter != List.end(); ++Iter) {
		SettingsShadows.Exteriors.ExcludedForms.push_back(strtol(Iter->c_str(), NULL, 16));
	}
	if (List.size()) std::sort(SettingsShadows.Exteriors.ExcludedForms.begin(), SettingsShadows.Exteriors.ExcludedForms.end());

	SettingsShadows.Interiors.Enabled = GetSettingI("Shaders.ShadowsInteriors.Main", "Enabled");
	SettingsShadows.Interiors.AlphaEnabled = GetSettingI("Shaders.ShadowsInteriors.Main", "AlphaEnabled");
	SettingsShadows.Interiors.Forms.Activators = GetSettingI("Shaders.ShadowsInteriors.Main", "Activators");
	SettingsShadows.Interiors.Forms.Actors = GetSettingI("Shaders.ShadowsInteriors.Main", "Actors");
	SettingsShadows.Interiors.Forms.Apparatus = GetSettingI("Shaders.ShadowsInteriors.Main", "Apparatus");
	SettingsShadows.Interiors.Forms.Books = GetSettingI("Shaders.ShadowsInteriors.Main", "Books");
	SettingsShadows.Interiors.Forms.Containers = GetSettingI("Shaders.ShadowsInteriors.Main", "Containers");
	SettingsShadows.Interiors.Forms.Doors = GetSettingI("Shaders.ShadowsInteriors.Main", "Doors");
	SettingsShadows.Interiors.Forms.Furniture = GetSettingI("Shaders.ShadowsInteriors.Main", "Furniture");
	SettingsShadows.Interiors.Forms.Misc = GetSettingI("Shaders.ShadowsInteriors.Main", "Misc");
	SettingsShadows.Interiors.Forms.Statics = GetSettingI("Shaders.ShadowsInteriors.Main", "Statics");
	SettingsShadows.Interiors.Forms.MinRadius = GetSettingF("Shaders.ShadowsInteriors.Main", "MinRadius");
	SettingsShadows.Interiors.Quality = GetSettingI("Shaders.ShadowsInteriors.Main", "Quality");
	SettingsShadows.Interiors.LightPoints = GetSettingI("Shaders.ShadowsInteriors.Main", "LightPoints");
	SettingsShadows.Interiors.TorchesCastShadows = GetSettingI("Shaders.ShadowsInteriors.Main", "TorchesCastShadows");
	SettingsShadows.Interiors.ShadowCubeMapSize = GetSettingI("Shaders.ShadowsInteriors.Main", "ShadowCubeMapSize");
	SettingsShadows.Interiors.Darkness = GetSettingF("Shaders.ShadowsInteriors.Main", "Darkness");
	SettingsShadows.Interiors.LightRadiusMult = GetSettingF("Shaders.ShadowsInteriors.Main", "LightRadiusMult");

	Config.FillSections(&List, "Shaders.ShadowsInteriors.ExcludedFormID");
	SettingsShadows.Interiors.ExcludedForms.reserve(List.size());
	for (StringList::iterator Iter = List.begin(); Iter != List.end(); ++Iter) {
		SettingsShadows.Interiors.ExcludedForms.push_back(strtol(Iter->c_str(), NULL, 16));
	}
	if (List.size()) std::sort(SettingsShadows.Interiors.ExcludedForms.begin(), SettingsShadows.Interiors.ExcludedForms.end());

	Config.FillSections(&List, "Shaders.Water"); // get the list of waters
	for (StringList::iterator Iter = List.begin(); Iter != List.end(); ++Iter) {
		const char* WaterSection = Iter->c_str();
		if (strcmp(WaterSection, "Status")) {
			char SectionName[80];
			strcpy(SectionName, "Shaders.Water.");
			strcat(SectionName, WaterSection);
			SW.choppiness = GetSettingF(SectionName, "choppiness");
			SW.waveWidth = GetSettingF(SectionName, "waveWidth");
			SW.waveSpeed = GetSettingF(SectionName, "waveSpeed");
			SW.reflectivity = GetSettingF(SectionName, "reflectivity");
			SW.causticsStrength = GetSettingF(SectionName, "causticsStrength");
			SW.causticsStrengthS = GetSettingF(SectionName, "causticsStrengthS");
			SW.shoreFactor = GetSettingF(SectionName, "shoreFactor");
			SW.shoreMovement = GetSettingF(SectionName, "shoreMovement");
			SW.turbidity = GetSettingF(SectionName, "turbidity");
			SW.inScattCoeff = GetSettingF(SectionName, "inScattCoeff");
			SW.inExtCoeff_R = GetSettingF(SectionName, "inExtCoeff_R");
			SW.inExtCoeff_G = GetSettingF(SectionName, "inExtCoeff_G");
			SW.inExtCoeff_B = GetSettingF(SectionName, "inExtCoeff_B");
			SW.depthDarkness = GetSettingF(SectionName, "depthDarkness");
			SettingsWater[WaterSection] = SW;
		}
	}

	timer.LogTime("SettingsManager::InitSettings for loop");
}

/*
* Saves the state of the config to the config file
*/
void SettingManager::SaveSettings() {

	char Filename[MAX_PATH];

	GetCurrentDirectoryA(MAX_PATH, Filename);
	strcat(Filename, TomlSettingsFile);
	std::ofstream ConfigurationFile(Filename, std::ios::trunc | std::ios::binary);

	// log config file contents
	//std::stringstream buffer;
	//buffer << Config.TomlConfig << std::endl;
	//Logger::Log("%s", buffer.str().c_str());

	ConfigurationFile << Config.TomlConfig << std::endl;
	ConfigurationFile.close();

}

int SettingManager::GetSettingI(const char* Section, const char* Key) {
	Configuration::ConfigNode Node;
	int Value = 0; //default in case the setting doesn't exist

	if (Config.FillNode(&Node, Section, Key)) Value = atoi(Node.Value); // convert back from string to final type (TODO: store as native type)
	return Value;
}

float SettingManager::GetSettingF(const char* Section, const char* Key) {
	Configuration::ConfigNode Node;
	float Value = 0.0f; //default in case the setting doesn't exist

	if (Config.FillNode(&Node, Section, Key)) Value = atof(Node.Value); // convert back from string to final type (TODO: store as native type)
	return Value;
}

char* SettingManager::GetSettingS(const char* Section, const char* Key, char* Value) {
	Configuration::ConfigNode Node;
	
	strcpy(Value, "\""); //default in case the setting doesn't exist
	if (Config.FillNode(&Node, Section, Key)) strcpy(Value, Node.Value);
	return Value;
}

void SettingManager::SetSetting(const char* Section, const char* Key, UINT8 Value) {

	Configuration::ConfigNode Node;
	CreateNode(&Node, Section, Key, Value, false);
	SetSetting(&Node);
}

void SettingManager::SetSetting(const char* Section, const char* Key, bool Value) {

	Configuration::ConfigNode Node;
	CreateNode(&Node, Section, Key, Value, false);
	SetSetting(&Node);
}

void SettingManager::SetSetting(const char* Section, const char* Key, float Value) {

	Configuration::ConfigNode Node;
	CreateNode(&Node, Section, Key, Value, false);

	SetSetting(&Node);
}

/*
* Builds a node and sets the value
*/
void SettingManager::SetSettingS(const char* Section, const char* Key, char* Value) {

	Configuration::ConfigNode Node;

	Config.FillNode(&Node, Section, Key);
	strcpy(Node.Value, Value);
	SetSetting(&Node);

}

/*
* Applies the changes made to the node value into the actual setting saving system
*/
void SettingManager::SetSetting(Configuration::ConfigNode* Node) {

	Config.SetValue(Node);
	LoadSettings();

}

void SettingManager::SetSettingWeather(const char* Section, const char* Key, float Value) {

	Configuration::ConfigNode Node;
	SettingsWeatherStruct* SettingsWeather = NULL;
	const char* WeatherName = NULL;
	TESWeather* Weather = NULL;
	StringList Values;
	char AttributeValue[80] = { NULL };
	char ColorSection[40];
	char ColorNode[8];

	SplitString(Section, ".", &Values);
	WeatherName = Values[1].c_str();
	Weather = (TESWeather*)DataHandler->GetFormByName(WeatherName, TESForm::FormType::kFormType_Weather);
	SettingsWeather = GetSettingsWeather(WeatherName);
	if (!SettingsWeather) {
		Config.CreateWeatherSection(WeatherName, Weather);
		LoadSettings();
		SettingsWeather = GetSettingsWeather(WeatherName);
	}
	if (Values[2] == "Main") {
		Config.FillNode(&Node, Section, Key);
		switch (Node.Type) {
		case Configuration::NodeType::Boolean:
			strcpy(Node.Value, ToString<bool>(Value).c_str());
			break;
		case Configuration::NodeType::Integer:
			strcpy(Node.Value, ToString<int>(Value).c_str());
			break;
		case Configuration::NodeType::Float:
			strcpy(Node.Value, ToString<float>(Value).c_str());
			break;
		}
	}
	else if (Values[2] == "HDR") {
		AttributeValue[0] = NULL;
		for (UInt32 i = 0; i < 14; i++) {
			if (!strcmp(WeatherHDRTypes[i], Key))
				strcat(AttributeValue, ToString(Value).c_str());
			else
				strcat(AttributeValue, ToString(SettingsWeather->HDR[i]).c_str());
			if (i < 13) strcat(AttributeValue, " ");
		}
		CreateNodeS(&Node, Section, "HDR", AttributeValue, 0);
	}
	else {
		AttributeValue[0] = NULL;
		for (UInt32 x = 0; x < TESWeather::kNumColorTypes; x++) {
			if (Values[2] == WeatherColorTypes[x]) {
				for (UInt32 y = 0; y < TESWeather::kNumTimeOfDay; y++) {
					if (strstr(Key, WeatherTimesOfDay[y])) {
						if (!memcmp(Key + strlen(Key) - 1, "R", 1)) {
							strcat(AttributeValue, ToString(Value).c_str());
							strcat(AttributeValue, ",");
							strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].g).c_str());
							strcat(AttributeValue, ",");
							strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].b).c_str());
						}
						else if (!memcmp(Key + strlen(Key) - 1, "G", 1)) {
							strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].r).c_str());
							strcat(AttributeValue, ",");
							strcat(AttributeValue, ToString(Value).c_str());
							strcat(AttributeValue, ",");
							strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].b).c_str());
						}
						else if (!memcmp(Key + strlen(Key) - 1, "B", 1)) {
							strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].r).c_str());
							strcat(AttributeValue, ",");
							strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].g).c_str());
							strcat(AttributeValue, ",");
							strcat(AttributeValue, ToString(Value).c_str());
						}
					}
					else {
						strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].r).c_str());
						strcat(AttributeValue, ",");
						strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].g).c_str());
						strcat(AttributeValue, ",");
						strcat(AttributeValue, ToString(SettingsWeather->Colors[x].colors[y].b).c_str());
					}
					if (y < TESWeather::kNumTimeOfDay - 1) strcat(AttributeValue, " ");
				}
				strcpy(ColorSection, Values[0].c_str());
				strcat(ColorSection, ".");
				strcat(ColorSection, Values[1].c_str());
				strcat(ColorSection, ".");
				strcat(ColorSection, "Colors");
				sprintf(ColorNode, "Color%i", x);
				CreateNodeS(&Node, ColorSection, ColorNode, AttributeValue, 0);
				break;
			}
		}
	}
	SetSetting(&Node);
	SetSettingsWeather(Weather);

}

/*
* Gets the subsections for a given section, used by the menu to list the children of a section.
*/
void SettingManager::FillMenuSections(StringList* Sections, const char* ParentSection) {

	if (ParentSection == NULL || memcmp(ParentSection, "Weathers", 8)) {
		Config.FillSections(Sections, ParentSection);
	}
	else {
		if (strlen(ParentSection) == 8) {
			DataHandler->FillNames(Sections, TESForm::FormType::kFormType_Weather);
			std::sort(Sections->begin(), Sections->end());
		}
		else {
			Sections->clear();
			Sections->push_back("Main");
			for (UInt32 i = 0; i < TESWeather::kNumColorTypes; i++) {
				Sections->push_back(WeatherColorTypes[i]);
			}
			Sections->push_back("HDR");
		}
	}

}

/*
* Gathers the settings values for a given section of the menu, in order to display them in the menu.
*/
void SettingManager::FillMenuSettings(Configuration::SettingList* Settings, const char* Section) {

	SettingsWeatherStruct* SettingsWeather = NULL;
	Configuration::ConfigNode Node;
	StringList Values;

	Settings->clear();
	if (memcmp(Section, "Weathers", 8)) {
		// not weather, simply gather key/value pairs for each settings of the section
		Config.FillSettings(Settings, Section);
	}
	else {
		// handle generated values from game data for weather sections
		SplitString(Section, ".", &Values);
		SettingsWeather = GetSettingsWeather(Values[1].c_str());
		if (SettingsWeather) {
			if (Values[2] == "Main") {
				Config.FillSettings(Settings, Section);
			}
			else if (Values[2] == "HDR") {
				for (UInt32 i = 0; i < 14; i++) {
					CreateNode(&Node, Section, WeatherHDRTypes[i], SettingsWeather->HDR[i], 0);
					Settings->push_back(Node);
				}
			}
			else {
				for (UInt32 i = 0; i < TESWeather::kNumColorTypes; i++) {
					if (Values[2] == WeatherColorTypes[i]) {
						CreateNode(&Node, Section, "DayR", SettingsWeather->Colors[i].colors[TESWeather::eTime_Day].r, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "DayG", SettingsWeather->Colors[i].colors[TESWeather::eTime_Day].g, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "DayB", SettingsWeather->Colors[i].colors[TESWeather::eTime_Day].b, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "NightR", SettingsWeather->Colors[i].colors[TESWeather::eTime_Night].r, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "NightG", SettingsWeather->Colors[i].colors[TESWeather::eTime_Night].g, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "NightB", SettingsWeather->Colors[i].colors[TESWeather::eTime_Night].b, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunriseR", SettingsWeather->Colors[i].colors[TESWeather::eTime_Sunrise].r, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunriseG", SettingsWeather->Colors[i].colors[TESWeather::eTime_Sunrise].g, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunriseB", SettingsWeather->Colors[i].colors[TESWeather::eTime_Sunrise].b, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunsetR", SettingsWeather->Colors[i].colors[TESWeather::eTime_Sunset].r, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunsetG", SettingsWeather->Colors[i].colors[TESWeather::eTime_Sunset].g, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunsetB", SettingsWeather->Colors[i].colors[TESWeather::eTime_Sunset].b, 0);
						Settings->push_back(Node);
					}
				}
			}
		}
		else {
			TESWeather* Weather = (TESWeather*)DataHandler->GetFormByName(Values[1].c_str(), TESForm::FormType::kFormType_Weather);
			if (Values[2] == "Main") {
				CreateNode(&Node, Section, "CloudSpeedLower", Weather->GetCloudSpeedLower(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "CloudSpeedUpper", Weather->GetCloudSpeedUpper(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "FogFarDay", Weather->GetFogDayFar(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "FogFarNight", Weather->GetFogNightFar(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "FogNearDay", Weather->GetFogDayNear(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "FogNearNight", Weather->GetFogNightNear(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "SunDamage", Weather->GetSunDamage(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "SunGlare", Weather->GetSunGlare(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "TransDelta", Weather->GetTransDelta(), 0);
				Settings->push_back(Node);
				CreateNode(&Node, Section, "WindSpeed", Weather->GetWindSpeed(), 0);
				Settings->push_back(Node);
			}
			else if (Values[2] == "HDR") {
				for (UInt32 i = 0; i < 14; i++) {
					CreateNode(&Node, Section, WeatherHDRTypes[i], Weather->GetHDR(i), 0);
					Settings->push_back(Node);
				}
			}
			else {
				for (UInt32 i = 0; i < TESWeather::kNumColorTypes; i++) {
					if (Values[2] == WeatherColorTypes[i]) {
						CreateNode(&Node, Section, "DayR", Weather->colors[i].colors[TESWeather::eTime_Day].r, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "DayG", Weather->colors[i].colors[TESWeather::eTime_Day].g, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "DayB", Weather->colors[i].colors[TESWeather::eTime_Day].b, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "NightR", Weather->colors[i].colors[TESWeather::eTime_Night].r, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "NightG", Weather->colors[i].colors[TESWeather::eTime_Night].g, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "NightB", Weather->colors[i].colors[TESWeather::eTime_Night].b, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunriseR", Weather->colors[i].colors[TESWeather::eTime_Sunrise].r, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunriseG", Weather->colors[i].colors[TESWeather::eTime_Sunrise].g, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunriseB", Weather->colors[i].colors[TESWeather::eTime_Sunrise].b, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunsetR", Weather->colors[i].colors[TESWeather::eTime_Sunset].r, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunsetG", Weather->colors[i].colors[TESWeather::eTime_Sunset].g, 0);
						Settings->push_back(Node);
						CreateNode(&Node, Section, "SunsetB", Weather->colors[i].colors[TESWeather::eTime_Sunset].b, 0);
						Settings->push_back(Node);
					}
				}
			}
		}
	}

}


/*
* Creates a Config Node to hold the info for a given setting, based on section, key, type and value.
*/
void SettingManager::CreateNode(Configuration::ConfigNode* Node, const char* Section, const char* Key, float Value, bool Reboot) {
	strcpy(Node->Section, Section);
	strcpy(Node->Key, Key);
	strcpy(Node->Value, ToString<float>(Value).c_str()); // convert to string to store value (TODO: store values in native format)
	Node->Reboot = Reboot;
	Node->Type = Configuration::NodeType::Float;
}


/*
* Creates a Config Node to hold the info for a given setting, based on section, key, type and value.
*/
void SettingManager::CreateNode(Configuration::ConfigNode* Node, const char* Section, const char* Key, UINT8 Value, bool Reboot) {
	strcpy(Node->Section, Section);
	strcpy(Node->Key, Key);
	strcpy(Node->Value, ToString<int>(Value).c_str()); // convert to string to store value (TODO: store values in native format)
	Node->Reboot = Reboot;
	Node->Type = Configuration::NodeType::Integer;
}

/*
* Creates a Config Node to hold the info for a given setting, based on section, key, type and value.
*/
void SettingManager::CreateNode(Configuration::ConfigNode* Node, const char* Section, const char* Key, bool Value, bool Reboot) {
	strcpy(Node->Section, Section);
	strcpy(Node->Key, Key);
	strcpy(Node->Value, ToString<bool>(Value).c_str()); // convert to string to store value (TODO: store values in native format)
	Node->Reboot = Reboot;
	Node->Type = Configuration::NodeType::Boolean;
}

/*
* Creates a Config Node to hold the info for a given setting, based on section, key, type and value.
*/
void SettingManager::CreateNodeS(Configuration::ConfigNode* Node, const char* Section, const char* Key, const char* Value, bool Reboot) {
	strcpy(Node->Section, Section);
	strcpy(Node->Key, Key);
	strcpy(Node->Value, Value);
	Node->Reboot = Reboot;
	Node->Type = Configuration::NodeType::String;
}


bool SettingManager::GetMenuShaderEnabled(const char* Name) {
	char settingString[256];
	strcpy(settingString, "Shaders.");
	strcat(settingString, Name);

	// handle enabling shaders that don't appear in settings (always on)
	char path[256] = "_";
	strcat(path, settingString);
	if (!Config.TomlConfig.at_path(path).is_table() && !Config.DefaultConfig.at_path(path).is_table()) {
		Logger::Log("No Shader setting for %s, defaults to true", settingString);
		return true;
	}

	strcat(settingString, ".Status");
	// handle special case of shadows (To fix: remove this special case)
	char settingName[12] = "";
	if (!memcmp(Name, "Shadows", 7)) {
		strcpy(settingName, "PostProcess");
	}
	else {
		strcpy(settingName, "Enabled");
	}

	return (bool*)GetSettingI(settingString, settingName);
}


void SettingManager::SetMenuShaderEnabled(const char* Name, bool enabled) {
	char settingString[256];
	strcpy(settingString, "Shaders.");
	strcat(settingString, Name);
	strcat(settingString, ".Status");

	// handle special case of shadows (To fix: remove this special case)
	char settingName[12] = "";
	if (!memcmp(Name, "Shadows", 7)) {
		strcpy(settingName, "PostProcess");
	}
	else {
		strcpy(settingName, "Enabled");
	}

	SetSetting(settingString, settingName, enabled);
}


SettingsWaterStruct* SettingManager::GetSettingsWater(const char* PlayerLocation) {

	SettingsWaterMap::iterator v = SettingsWater.find(std::string(PlayerLocation));
	if (v == SettingsWater.end())
		return NULL;
	else
		return &v->second;

}

SettingsColoringStruct* SettingManager::GetSettingsColoring(const char* PlayerLocation) {

	SettingsColoringMap::iterator v = SettingsColoring.find(std::string(PlayerLocation));
	if (v == SettingsColoring.end())
		return NULL;
	else
		return &v->second;

}

SettingsWeatherStruct* SettingManager::GetSettingsWeather(const char* WeatherName) {

	SettingsWeatherMap::iterator v = SettingsWeather.find(std::string(WeatherName));
	if (v == SettingsWeather.end())
		return NULL;
	else
		return &v->second;

}

void SettingManager::SetSettingsWeather(TESWeather* Weather) {

	TESWeatherEx* WeatherEx = (TESWeatherEx*)Weather;
	SettingsWeatherStruct* SettingsWeather = GetSettingsWeather(WeatherEx->EditorName);

	if (SettingsWeather) {
		SetTextureAndHDR(WeatherEx, SettingsWeather->UpperLayer, SettingsWeather->LowerLayer, SettingsWeather->HDR);
		WeatherEx->SetWindSpeed(SettingsWeather->WindSpeed);
		WeatherEx->SetCloudSpeedLower(SettingsWeather->CloudSpeedLower);
		WeatherEx->SetCloudSpeedUpper(SettingsWeather->CloudSpeedUpper);
		WeatherEx->SetTransDelta(SettingsWeather->TransDelta);
		WeatherEx->SetSunGlare(SettingsWeather->SunGlare);
		WeatherEx->SetSunDamage(SettingsWeather->SunDamage);
		WeatherEx->SetFogDayNear(SettingsWeather->FogNearDay);
		WeatherEx->SetFogDayFar(SettingsWeather->FogFarDay);
		WeatherEx->SetFogNightNear(SettingsWeather->FogNearNight);
		WeatherEx->SetFogNightFar(SettingsWeather->FogFarNight);
		memcpy(WeatherEx->colors, SettingsWeather->Colors, TESWeather::kNumColorTypes * TESWeather::kNumTimeOfDay * 4);
		memcpy(WeatherEx->colorsb, SettingsWeather->Colors, TESWeather::kNumColorTypes * TESWeather::kNumTimeOfDay * 4);
	}
	for (int c = 0; c < TESWeatherEx::eColor_Lighting; c++) {
		WeatherEx->colors[c].colors[TESWeatherEx::eTime_Night].r = WeatherEx->colorsb[c].colors[TESWeatherEx::eTime_Night].r * SettingsMain.WeatherMode.CoeffNight.x;
		WeatherEx->colors[c].colors[TESWeatherEx::eTime_Night].g = WeatherEx->colorsb[c].colors[TESWeatherEx::eTime_Night].g * SettingsMain.WeatherMode.CoeffNight.y;
		WeatherEx->colors[c].colors[TESWeatherEx::eTime_Night].b = WeatherEx->colorsb[c].colors[TESWeatherEx::eTime_Night].b * SettingsMain.WeatherMode.CoeffNight.z;
	}
	for (int t = 0; t < TESWeatherEx::eTime_Night; t++) {
		WeatherEx->colors[TESWeatherEx::eColor_Fog].colors[t].r = WeatherEx->colorsb[TESWeatherEx::eColor_Fog].colors[t].r * SettingsMain.WeatherMode.CoeffFog.x;
		WeatherEx->colors[TESWeatherEx::eColor_Fog].colors[t].g = WeatherEx->colorsb[TESWeatherEx::eColor_Fog].colors[t].g * SettingsMain.WeatherMode.CoeffFog.y;
		WeatherEx->colors[TESWeatherEx::eColor_Fog].colors[t].b = WeatherEx->colorsb[TESWeatherEx::eColor_Fog].colors[t].b * SettingsMain.WeatherMode.CoeffFog.z;
		WeatherEx->colors[TESWeatherEx::eColor_Sunlight].colors[t].r = WeatherEx->colorsb[TESWeatherEx::eColor_Sunlight].colors[t].r * SettingsMain.WeatherMode.CoeffSun.x;
		WeatherEx->colors[TESWeatherEx::eColor_Sunlight].colors[t].g = WeatherEx->colorsb[TESWeatherEx::eColor_Sunlight].colors[t].g * SettingsMain.WeatherMode.CoeffSun.y;
		WeatherEx->colors[TESWeatherEx::eColor_Sunlight].colors[t].b = WeatherEx->colorsb[TESWeatherEx::eColor_Sunlight].colors[t].b * SettingsMain.WeatherMode.CoeffSun.z;
		WeatherEx->colors[TESWeatherEx::eColor_Sun].colors[t].r = WeatherEx->colorsb[TESWeatherEx::eColor_Sun].colors[t].r * SettingsMain.WeatherMode.CoeffSun.x;
		WeatherEx->colors[TESWeatherEx::eColor_Sun].colors[t].g = WeatherEx->colorsb[TESWeatherEx::eColor_Sun].colors[t].g * SettingsMain.WeatherMode.CoeffSun.y;
		WeatherEx->colors[TESWeatherEx::eColor_Sun].colors[t].b = WeatherEx->colorsb[TESWeatherEx::eColor_Sun].colors[t].b * SettingsMain.WeatherMode.CoeffSun.z;
	}

}

template<typename T>
std::string SettingManager::ToString(const T Value) {

	std::ostringstream os;

	os << (float)Value;
	return os.str();

}

template<typename T>
T SettingManager::FromString(const char* Value) {

	float Result = 0.0f;
	std::istringstream is(Value);

	is >> Result;
	return Result;

}

void SettingManager::SplitString(const char* String, const char* Delimiter, StringList* Values) {

	char StringB[256];
	char* Finder = NULL;

	strcpy(StringB, String);
	Finder = strtok(StringB, Delimiter);
	Values->clear();
	while (Finder != NULL) {
		Values->push_back(Finder);
		Finder = strtok(NULL, Delimiter);
	}

}

template<typename T>
void SettingManager::FillFromString(const char* String, const char* Delimiter, T* Values) {

	char StringB[256];
	char* Finder = NULL;
	UInt32 c = 0;

	strcpy(StringB, String);
	Finder = strtok(StringB, Delimiter);
	while (Finder != NULL) {
		Values[c] = FromString<T>(Finder);
		Finder = strtok(NULL, Delimiter);
		c++;
	}

}
