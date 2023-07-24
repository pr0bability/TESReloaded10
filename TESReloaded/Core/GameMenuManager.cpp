#define TextColorNormal D3DCOLOR_XRGB(MenuSettings->TextColorNormal[0], MenuSettings->TextColorNormal[1], MenuSettings->TextColorNormal[2])
#define TextShadowColorNormal D3DCOLOR_XRGB(MenuSettings->TextShadowColorNormal[0], MenuSettings->TextShadowColorNormal[1], MenuSettings->TextShadowColorNormal[2])
#define TextColorSelected D3DCOLOR_XRGB(MenuSettings->TextColorSelected[0], MenuSettings->TextColorSelected[1], MenuSettings->TextColorSelected[2])
#define TextShadowColorSelected D3DCOLOR_XRGB(MenuSettings->TextShadowColorSelected[0], MenuSettings->TextShadowColorSelected[1], MenuSettings->TextShadowColorSelected[2])
#define TextColorEditing D3DCOLOR_XRGB(MenuSettings->TextColorEditing[0], MenuSettings->TextColorEditing[1], MenuSettings->TextColorEditing[2])
#define TextShadowColorEditing D3DCOLOR_XRGB(MenuSettings->TextShadowColorEditing[0], MenuSettings->TextShadowColorEditing[1], MenuSettings->TextShadowColorEditing[2])
#define TextColorEnabled D3DCOLOR_XRGB(MenuSettings->TextColorEnabled[0], MenuSettings->TextColorEnabled[1], MenuSettings->TextColorEnabled[2])
#define TextShadowColorEnabled D3DCOLOR_XRGB(MenuSettings->TextShadowColorEnabled[0], MenuSettings->TextShadowColorEnabled[1], MenuSettings->TextShadowColorEnabled[2])
#define PositionX MenuSettings->PositionX
#define PositionY MenuSettings->PositionY
#define TitleColumnSize MenuSettings->TitleColumnSize
#define MainItemColumnSize MenuSettings->MainItemColumnSize
#define ItemColumnSize MenuSettings->ItemColumnSize
#define RowSpace MenuSettings->RowSpace
#define RowsPerPage MenuSettings->RowsPerPage

void GameMenuManager::Initialize() {

	Logger::Log("Starting the menu manager...");
	TheGameMenuManager = new GameMenuManager();

	SettingsMainStruct::MenuStruct* MenuSettings = &TheSettingManager->SettingsMain.Menu;

	TheGameMenuManager->SelectedColumn = 0;
	TheGameMenuManager->SelectedRow[4] = { 0 };
	TheGameMenuManager->SelectedPage[4] = { 0 };
	TheGameMenuManager->Enabled = false;
	TheGameMenuManager->EditingMode = false;
	TheGameMenuManager->MainMenuOn = false;

	D3DXCreateFontA(TheRenderManager->device, MenuSettings->TextSize, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, MenuSettings->TextFont, &TheGameMenuManager->FontNormal);
	D3DXCreateFontA(TheRenderManager->device, MenuSettings->TextSize, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, MenuSettings->TextFont, &TheGameMenuManager->FontSelected);
	D3DXCreateFontA(TheRenderManager->device, MenuSettings->TextSizeStatus, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, MenuSettings->TextFontStatus, &TheGameMenuManager->FontStatus);

}

void GameMenuManager::Render() {

	SettingsMainStruct::MenuStruct* MenuSettings = &TheSettingManager->SettingsMain.Menu;
	StringList Sections;
	StringList::iterator Item;
	SettingManager::Configuration::SettingList Settings;
	SettingManager::Configuration::SettingList::iterator Setting;
	const char* Text = NULL;
	char TextShaderStatus[20];
	char SettingText[80];
	char MidSection[40];
	size_t ListSize = 0;

	bool isShaderSection = !memcmp(SelectedNode.Section, "Shaders", 7);
	bool isStatusSection = !memcmp(SelectedNode.Section + strlen(SelectedNode.Section) - 6, "Status", 6);
	bool isWeatherSection = !memcmp(SelectedNode.Section, "Weathers", 8);

	if (InterfaceManager->IsActive(Menu::MenuType::kMenuType_Main)) {
		// on main menu, only draw the bottom text as signal the mod has loaded
		if (!MainMenuOn) {
			MainMenuOn = true;
			MainMenuStartTime = std::chrono::system_clock::now();
		}

		std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - MainMenuStartTime;
		if (elapsed_seconds.count() > 3.0) return; // only show message at the bottom of the screen for 3 seconds

		SetRect(&Rect, 0, TheRenderManager->height - MenuSettings->TextSize - 10, TheRenderManager->width, TheRenderManager->height + MenuSettings->TextSize);
		SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
		FontNormal->DrawTextA(NULL, PluginVersion::VersionString, -1, &RectShadow, DT_CENTER, TextShadowColorNormal);
		FontNormal->DrawTextA(NULL, PluginVersion::VersionString, -1, &Rect, DT_CENTER, TextColorNormal);
		return;
	}

	// check for menu key to toggle display of the menu
	if (Global->OnKeyDown(MenuSettings->KeyEnable)) {
		Enabled = !Enabled;
		EditingMode = false;
	}
	if (!InterfaceManager->IsActive(Menu::MenuType::kMenuType_None)) {
		Enabled = false;
		EditingMode = false;
	}

	if (!Enabled) return; // skip rendering of menu if it isn't enabled

	bool useNumpad = TheSettingManager->GetSettingI("Main.Menu.Keys", "EntryUseNumpad");

	//handle entry using numpad keys
	int startEntry = useNumpad ? MenuSettings->KeyEditing : 13;
	int key0 = useNumpad ? 82 : 11;
	int key1 = useNumpad ? 79 : 2;
	int key2 = useNumpad ? 80 : 3;
	int key3 = useNumpad ? 81 : 4;
	int key4 = useNumpad ? 75 : 5;
	int key5 = useNumpad ? 76 : 6;
	int key6 = useNumpad ? 77 : 7;
	int key7 = useNumpad ? 71 : 8;
	int key8 = useNumpad ? 72 : 9;
	int key9 = useNumpad ? 73 : 10;
	int keyDot = useNumpad ? 83 : 52;
	int keyMinus = useNumpad ? 74 : 12;

	// value editing mode toggle (disabled in Status sections because it's more complex to deal with switching effects)
	if (Global->OnKeyDown(startEntry) && SelectedColumn == 3 && !isStatusSection) {
		if (!EditingMode) {
			// start editing, copy the current value to the edited string
			strcpy(EditingValue, SelectedNode.Value);
		}
		else {
			// finished editing, sets the value. 
			// convert the value from the string to the given type and back to the string to ensure the right type format
			switch (SelectedNode.Type){
			case SettingManager::Configuration::NodeType::Boolean:
				strcpy(SelectedNode.Value, TheSettingManager->ToString<bool>(atof(EditingValue)).c_str());
				break;
			case SettingManager::Configuration::NodeType::Integer:
				strcpy(SelectedNode.Value, TheSettingManager->ToString<int>(atof(EditingValue)).c_str());
				break;
			case SettingManager::Configuration::NodeType::Float:
				strcpy(SelectedNode.Value, TheSettingManager->ToString<float>(atof(EditingValue)).c_str());
				break;
			default:
				break;
			}
			TheSettingManager->SetSetting(&SelectedNode);
			TheSettingManager->LoadSettings(); // refresh settings struct
		}

		EditingMode = !EditingMode;
	}

	if (EditingMode) {

		if (Global->OnKeyDown(key0))
			strcat(EditingValue, "0");
		else if (Global->OnKeyDown(key1))
			strcat(EditingValue, "1");
		else if (Global->OnKeyDown(key2))
			strcat(EditingValue, "2");
		else if (Global->OnKeyDown(key3))
			strcat(EditingValue, "3");
		else if (Global->OnKeyDown(key4))
			strcat(EditingValue, "4");
		else if (Global->OnKeyDown(key5))
			strcat(EditingValue, "5");
		else if (Global->OnKeyDown(key6))
			strcat(EditingValue, "6");
		else if (Global->OnKeyDown(key7))
			strcat(EditingValue, "7");
		else if (Global->OnKeyDown(key8))
			strcat(EditingValue, "8");
		else if (Global->OnKeyDown(key9))
			strcat(EditingValue, "9");
		else if (Global->OnKeyDown(keyDot))
			strcat(EditingValue, ".");
		else if (Global->OnKeyDown(keyMinus))
			strcat(EditingValue, "-");
		if (strlen(EditingValue) > 0 && Global->OnKeyDown(14)) EditingValue[strlen(EditingValue) - 1] = NULL;
	}
	else {
		// handle other types of user input
		if (Global->OnKeyDown(MenuSettings->KeySave)) {
			TheSettingManager->SaveSettings();
			InterfaceManager->ShowMessage("Settings saved.");
		}
		else if (SelectedColumn == 0) {
			// header is a column tilted to the side (column 0). Left and Right change rows and Down moves to column 1 (or the actual menu)
			if (Global->OnKeyDown(MenuSettings->KeyDown)) {
				SelectedColumn = 1;
			}
			else if (Global->OnKeyDown(MenuSettings->KeyLeft) && SelectedRow[SelectedColumn] > 0) {
				SelectedRow[SelectedColumn] -= 1;
			}
			else if (Global->OnKeyDown(MenuSettings->KeyRight) && SelectedRow[SelectedColumn] < Rows[SelectedColumn] - 1) {
				SelectedRow[SelectedColumn] += 1;
			}
			SelectedPage[1] = SelectedPage[2] = SelectedPage[3] = 0;
		}
		else {
			// handle navigation
			if (Global->OnKeyDown(MenuSettings->KeyUp)) {
				if (SelectedRow[SelectedColumn] > 0) SelectedRow[SelectedColumn] -= 1; else SelectedColumn = 0; // move up in the column or go to header
			}
			else if (Global->OnKeyDown(MenuSettings->KeyDown) && SelectedRow[SelectedColumn] < Rows[SelectedColumn] - 1) {
				SelectedRow[SelectedColumn] += 1;
			}
			else if (Global->OnKeyDown(MenuSettings->KeyLeft)) {
				SelectedRow[SelectedColumn] = 0;
				SelectedColumn -= 1;
			}
			else if (Global->OnKeyDown(MenuSettings->KeyRight) && SelectedColumn < 3) {
				SelectedColumn += 1;
			}
			else if (Global->OnKeyDown(MenuSettings->KeyPageUp) && SelectedPage[SelectedColumn] > 0) {
				SelectedPage[SelectedColumn] -= 1;
				SelectedRow[SelectedColumn] = 0;
			}
			else if (Global->OnKeyDown(MenuSettings->KeyPageDown) && SelectedPage[SelectedColumn] < Pages[SelectedColumn]) {
				SelectedPage[SelectedColumn] += 1;
				SelectedRow[SelectedColumn] = 0;
			}
			else {
				// handle value add/subtract keys
				if (Global->OnKeyDown(MenuSettings->KeyAdd)) {
					//Logger::Log("Add for %s.%s, isShader Section? %i, isStatusSection? %i, Column %i",SelectedNode.Section, SelectedNode.Key, isShaderSection, isStatusSection, SelectedColumn);

					// react to user key input to reduce the value of the setting
					if (isShaderSection && (SelectedColumn == 1 || (SelectedColumn == 3 && isStatusSection))) {
						// enable shaders and effects
						GetMidSection(MidSection);
						bool ShaderEnabled = TheSettingManager->GetMenuShaderEnabled(MidSection);
						if (!ShaderEnabled) TheShaderManager->SwitchShaderStatus(MidSection);
					}
					else if (SelectedColumn == 1 && isWeatherSection) {
						GetMidSection(MidSection);
						TESWeather* Weather = (TESWeather*)DataHandler->GetFormByName(MidSection, TESForm::FormType::kFormType_Weather);
						Tes->sky->ForceWeather(Weather);
					}
					else if (SelectedColumn == 3) {
						TheSettingManager->Increment(SelectedNode.Section, SelectedNode.Key);
					}
					TheSettingManager->LoadSettings(); //update constants stored in Settings structs
				}
				else if (Global->OnKeyDown(MenuSettings->KeySubtract)) {
					//Logger::Log("Subtract for %s.%s, isShader Section? %i, isStatusSection? %i, Column %i", SelectedNode.Section, SelectedNode.Key, isShaderSection, isStatusSection, SelectedColumn);

					// react to user key input to reduce the value of the setting
					if (isShaderSection && (SelectedColumn == 1 || (SelectedColumn == 3 && isStatusSection))) {
						// disable shaders and effects
						GetMidSection(MidSection);
						bool ShaderEnabled = TheSettingManager->GetMenuShaderEnabled(MidSection);
						if (ShaderEnabled) TheShaderManager->SwitchShaderStatus(MidSection);
					}
					else if (SelectedColumn == 3) {
						TheSettingManager->Decrement(SelectedNode.Section, SelectedNode.Key);
					}
					TheSettingManager->LoadSettings(); //update constants stored in Settings structs
				}
			}
		}
	}

	Text = TitleMenu;
	SetRect(&Rect, PositionX, PositionY, PositionX + TitleColumnSize, PositionY + MenuSettings->TextSize);
	SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
	FontNormal->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
	FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorNormal);

	SetRect(&Rect, Rect.left, Rect.bottom + RowSpace, Rect.right, Rect.bottom + RowSpace + 2);
	TheRenderManager->device->Clear(1L, (D3DRECT*)&Rect, D3DCLEAR_TARGET, TextColorNormal, 0.0f, 0L);

	D3DXCOLOR textColor = TextColorNormal;
	D3DXCOLOR shadowColor = TextShadowColorNormal;

	// render header
	Rows[0] = 0;
	SetRect(&Rect, Rect.left + MainItemColumnSize * 0, Rect.bottom + RowSpace, Rect.left + MainItemColumnSize * 1, Rect.bottom + RowSpace + MenuSettings->TextSize);
	TheSettingManager->FillMenuSections(&Sections, NULL);
	ListSize = Sections.size();
	Pages[0] = ListSize / RowsPerPage;
	Item = Sections.begin();
	for (UInt32 i = 0; i < ListSize; i++) {
		Text = Item->c_str();
		SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
		if (SelectedRow[0] == Rows[0]) {
			strcpy(SelectedNode.Section, Text);
			FontSelected->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorSelected);
			FontSelected->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorSelected);
		}
		else {
			FontNormal->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
			FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorNormal);
		}
		Rect.left += MainItemColumnSize;
		Rect.right += MainItemColumnSize;
		Rows[0]++;
		Item++;
	}

	SetRect(&Rect, PositionX, Rect.bottom + RowSpace, PositionX + TitleColumnSize, Rect.bottom + RowSpace + 2);
	TheRenderManager->device->Clear(1L, (D3DRECT*)&Rect, D3DCLEAR_TARGET, TextColorNormal, 0.0f, 0L);

	// render left column (shaders/menu category names)
	int MenuRectX = PositionX;
	int MenuRectY = Rect.bottom + RowSpace;
	Rows[1] = 0;
	SetRect(&Rect, MenuRectX + ItemColumnSize * 0, MenuRectY, MenuRectX + ItemColumnSize * 1, MenuRectY + MenuSettings->TextSize);
	TheSettingManager->FillMenuSections(&Sections, SelectedNode.Section);
	ListSize = Sections.size();
	Pages[1] = ListSize / RowsPerPage;
	Item = Sections.begin();
	for (UInt32 i = 0; i < ListSize; i++) {
		if (i >= RowsPerPage * SelectedPage[1] && i < RowsPerPage * (SelectedPage[1] + 1)) {
			Text = Item->c_str();
			Rect.top += MenuSettings->TextSize + RowSpace;
			Rect.bottom += MenuSettings->TextSize + RowSpace;
			SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
			if (SelectedRow[1] == Rows[1]) {
				strcat(SelectedNode.Section, ".");
				strcat(SelectedNode.Section, Text);

				textColor = (SelectedColumn >= 1) ? TextColorSelected : TextColorNormal;
				shadowColor = (SelectedColumn >= 1) ? TextShadowColorSelected : TextShadowColorNormal;
			}
			else {
				textColor = TextColorNormal;
				shadowColor = TextShadowColorNormal;
			}

			FontSelected->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, shadowColor);
			FontSelected->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, textColor);

			// if in shader mode, add indication wether each shader is activated
			if (!memcmp(SelectedNode.Section, "Shaders", 7)) {
				if (SelectedRow[1] == Rows[1] && SelectedColumn >= 1)
					FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_CALCRECT, TextColorSelected);
				else
					FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_CALCRECT, TextColorNormal);
				Rect.left = Rect.right + 1;
				Rect.right += 100;
				SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
				bool enabled = TheSettingManager->GetMenuShaderEnabled(Text);

				if (SelectedRow[1] == Rows[1] && SelectedColumn >= 1) {
					textColor = enabled ? TextColorEnabled : TextColorSelected;
					shadowColor = enabled ? TextShadowColorEnabled : TextShadowColorSelected;
				}
				else {
					textColor = enabled ? TextColorEnabled : TextColorNormal;
					shadowColor = enabled ? TextShadowColorEnabled : TextShadowColorNormal;
				}

				strcpy(TextShaderStatus, enabled ? "ENABLED" : "DISABLED");
				FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &RectShadow, DT_LEFT, shadowColor);
				FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &Rect, DT_LEFT, textColor);

				Rect.left = MenuRectX + ItemColumnSize * 0;
				Rect.right = MenuRectX + ItemColumnSize * 1;
			}
			Rows[1]++;
		}
		Item++;
	}


	// render middle column (shader/menu subsection names)
	Rows[2] = 0;
	SetRect(&Rect, MenuRectX + ItemColumnSize * 1, MenuRectY, MenuRectX + ItemColumnSize * 2, MenuRectY + MenuSettings->TextSize);
	TheSettingManager->FillMenuSections(&Sections, SelectedNode.Section); // get a list of sections for a given category
	ListSize = Sections.size();
	Pages[2] = ListSize / RowsPerPage;
	Item = Sections.begin();
	for (UInt32 i = 0; i < ListSize; i++) {
		if (i >= RowsPerPage * SelectedPage[2] && i < RowsPerPage * (SelectedPage[2] + 1)) {
			Text = Item->c_str();
			Rect.top += MenuSettings->TextSize + RowSpace;
			Rect.bottom += MenuSettings->TextSize + RowSpace;
			SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
			if (SelectedRow[2] == Rows[2]) {
				strcat(SelectedNode.Section, ".");
				strcat(SelectedNode.Section, Text);

				textColor = (SelectedColumn >= 2) ? TextColorSelected : TextColorNormal;
				shadowColor = (SelectedColumn >= 2) ? TextShadowColorSelected : TextShadowColorNormal;
			}
			else {
				textColor = TextColorNormal;
				shadowColor = TextShadowColorNormal;
			}
			FontNormal->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, shadowColor);
			FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, textColor);

			Rows[2]++;
		}
		Item++;
	}

	// render right column (settings name/value pairs)
	Rows[3] = 0;
	SetRect(&Rect, MenuRectX + ItemColumnSize * 2, MenuRectY, MenuRectX + ItemColumnSize * 3, MenuRectY + MenuSettings->TextSize);
	TheSettingManager->FillMenuSettings(&Settings, SelectedNode.Section); // build a setting list to display values
	ListSize = Settings.size();
	Pages[3] = ListSize / RowsPerPage;
	Setting = Settings.begin();
	for (UInt32 i = 0; i < ListSize; i++) {
		if (i >= RowsPerPage * SelectedPage[3] && i < RowsPerPage * (SelectedPage[3] + 1)) {
			strcpy(SettingText, Setting->Key);
			strcat(SettingText, " = ");
			strcat(SettingText, Setting->Value);
			Rect.top += MenuSettings->TextSize + RowSpace;
			Rect.bottom += MenuSettings->TextSize + RowSpace;
			SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);

			textColor = TextColorNormal;
			shadowColor = TextShadowColorNormal;
			if (SelectedRow[3] == Rows[3]) {
				memcpy((void*)&SelectedNode, Setting._Ptr, sizeof(SettingManager::Configuration::ConfigNode));
				if (SelectedColumn >= 3) {
					if (EditingMode) {
						strcpy(SelectedNode.Value, EditingValue);
						strcpy(SettingText, Setting->Key);
						strcat(SettingText, " = ");
						strcat(SettingText, EditingValue);
					}
					textColor = EditingMode ? TextColorEditing : TextColorSelected;
					shadowColor = EditingMode ? TextShadowColorEditing : TextShadowColorSelected;
				}
			}

			FontNormal->DrawTextA(NULL, SettingText, -1, &RectShadow, DT_LEFT, shadowColor);
			FontNormal->DrawTextA(NULL, SettingText, -1, &Rect, DT_LEFT, textColor);

			Rows[3]++;
		}
		Setting++;
	}

	const char* DescriptionText = "";

	if (SelectedColumn == 3) {
		DescriptionText = SelectedNode.Description.c_str();
	}
	else if (isShaderSection && SelectedColumn == 1) {
		// Get the general description of the effect from the Status.Enabled node of the Shader settings
		SettingManager::Configuration::ConfigNode StatusNode;
		char statusSection[256];
		strcpy(statusSection, "Shaders.");
		GetMidSection(MidSection);
		strcat(statusSection, MidSection);
		strcat(statusSection, ".Status");
		bool success = TheSettingManager->Config.FillNode(&StatusNode, statusSection, "Enabled");

		DescriptionText = success?StatusNode.Description.c_str():"";
	}

	int right = MenuRectX + ItemColumnSize * 3;
	int bottom = MenuRectY + (RowsPerPage * MenuSettings->TextSize);

	// render description
	SetRect(&Rect, PositionX, bottom - MenuSettings->TextSize, right, bottom + MenuSettings->TextSize);
	SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);

	FontNormal->DrawTextA(NULL, DescriptionText, -1, &RectShadow, DT_RIGHT, TextShadowColorNormal);
	FontNormal->DrawTextA(NULL, DescriptionText, -1, &Rect, DT_RIGHT, TextColorNormal);

}

// For a given Node, get the Shader/Effect name (Part of the section between the _Shader header and the subcategory)
void GameMenuManager::GetMidSection(char* MidSection) {
	
	char* PositionStart = NULL;
	char* PositionEnd = NULL;
	size_t Size = 0;

	PositionStart = strstr(SelectedNode.Section, ".");
	PositionStart++;
	PositionEnd = strstr(PositionStart, ".");
	Size = PositionEnd - PositionStart;
	strncpy(MidSection, PositionStart, Size);
	MidSection[Size] = NULL;

}
