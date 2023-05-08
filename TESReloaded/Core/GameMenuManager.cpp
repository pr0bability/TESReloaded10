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

	if (InterfaceManager->IsActive(Menu::MenuType::kMenuType_Main)) {
		SetRect(&Rect, 0, TheRenderManager->height - MenuSettings->TextSize - 10, TheRenderManager->width, TheRenderManager->height + MenuSettings->TextSize);
		SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
		FontNormal->DrawTextA(NULL, PluginVersion::VersionString, -1, &RectShadow, DT_CENTER, TextShadowColorNormal);
		FontNormal->DrawTextA(NULL, PluginVersion::VersionString, -1, &Rect, DT_CENTER, TextColorNormal);
	}
	else {
		if (Global->OnKeyDown(MenuSettings->KeyEnable)) {
			Enabled = !Enabled;
			EditingMode = false;
		}
		if (!InterfaceManager->IsActive(Menu::MenuType::kMenuType_None)) {
			Enabled = false;
			EditingMode = false;
		}
		if (Enabled) {
			if (Global->OnKeyDown(MenuSettings->KeyEditing) && SelectedColumn == 3) {
				if (!EditingMode)
					strcpy(EditingValue, SelectedNode.Value);
				else
					TheSettingManager->SetSetting(SelectedNode.Section, SelectedNode.Key, (float)atof(SelectedNode.Value));
				EditingMode = !EditingMode;
			}
			if (!EditingMode) {
				if (SelectedColumn == 0) {
					// header is a column tilted to the side (column 0). Left and Right change rows and Down moves to column 1 (or the actual menu)
					if (Global->OnKeyDown(MenuSettings->KeyDown)) {
						SelectedColumn = 1;
					}else if (Global->OnKeyDown(MenuSettings->KeyLeft) && SelectedRow[SelectedColumn] > 0) {					
						SelectedRow[SelectedColumn] -= 1;
					}else if (Global->OnKeyDown(MenuSettings->KeyRight) && SelectedRow[SelectedColumn] < Rows[SelectedColumn] - 1){	
						SelectedRow[SelectedColumn] += 1;
					}
					SelectedPage[1] = SelectedPage[2] = SelectedPage[3] = 0;
				}
				else {
					// handle user input normally
					if (Global->OnKeyDown(MenuSettings->KeyUp)) {
						if (SelectedRow[SelectedColumn] > 0) SelectedRow[SelectedColumn] -= 1; else SelectedColumn = 0; // move up in the column or go to header
					}else if (Global->OnKeyDown(MenuSettings->KeyDown) && SelectedRow[SelectedColumn] < Rows[SelectedColumn] - 1) {
						SelectedRow[SelectedColumn] += 1;
					}else if (Global->OnKeyDown(MenuSettings->KeyLeft)) {
						SelectedRow[SelectedColumn] = 0;
						SelectedColumn -= 1;
					}else if (Global->OnKeyDown(MenuSettings->KeyRight) && SelectedColumn < 3) {
						SelectedColumn += 1;
					}else if (Global->OnKeyDown(MenuSettings->KeyPageUp) && SelectedPage[SelectedColumn] > 0) {
						SelectedPage[SelectedColumn] -= 1;
						SelectedRow[SelectedColumn] = 0;
					}else if (Global->OnKeyDown(MenuSettings->KeyPageDown) && SelectedPage[SelectedColumn] < Pages[SelectedColumn]) {
						SelectedPage[SelectedColumn] += 1;
						SelectedRow[SelectedColumn] = 0;
					}else if (Global->OnKeyDown(MenuSettings->KeyAdd)) {
						// react to user key input to reduce the value of the setting
						if ((SelectedColumn == 1 && !memcmp(SelectedNode.Section, "Shaders", 7)) || ((SelectedColumn == 3 && !memcmp(SelectedNode.Section, "Shaders", 7) && !memcmp(SelectedNode.Section + strlen(SelectedNode.Section) - 6, "Status", 6)))) {
							// enable shaders and effects
							GetMidSection(MidSection);
							bool ShaderEnabled = TheSettingManager->GetMenuShaderEnabled(MidSection);
							if (!ShaderEnabled) TheShaderManager->SwitchShaderStatus(MidSection);
						}
						else if (SelectedColumn == 1 && !memcmp(SelectedNode.Section, "Weathers", 8)) {
							GetMidSection(MidSection);
							TESWeather* Weather = (TESWeather*)DataHandler->GetFormByName(MidSection, TESForm::FormType::kFormType_Weather);
							Tes->sky->ForceWeather(Weather);
						}
						else if (SelectedColumn == 3) {
							// edit settings values
							switch (SelectedNode.Type) {
								int intvalue;
								float floatvalue;
								case SettingManager::Configuration::NodeType::Boolean:
									intvalue = TheSettingManager->GetSettingI(SelectedNode.Section, SelectedNode.Key);
									TheSettingManager->SetSetting(SelectedNode.Section, SelectedNode.Key, (bool)!intvalue);
									break;
								case SettingManager::Configuration::NodeType::Integer:
									intvalue = TheSettingManager->GetSettingI(SelectedNode.Section, SelectedNode.Key);
									TheSettingManager->SetSetting(SelectedNode.Section, SelectedNode.Key, (UINT8)(intvalue + 1));
									break;
								case SettingManager::Configuration::NodeType::Float:
									floatvalue = TheSettingManager->GetSettingF(SelectedNode.Section, SelectedNode.Key);
									TheSettingManager->SetSetting(SelectedNode.Section, SelectedNode.Key, floatvalue + 0.1f);
									break;
							}
						}
					}else if (Global->OnKeyDown(MenuSettings->KeySubtract)) {
						// react to user key input to reduce the value of the setting
						if ((SelectedColumn == 1 && !memcmp(SelectedNode.Section, "Shaders", 7)) || ((SelectedColumn == 3 && !memcmp(SelectedNode.Section, "Shaders", 7) && !memcmp(SelectedNode.Section + strlen(SelectedNode.Section) - 6, "Status", 6)))) {
							// disable shaders and effects
							GetMidSection(MidSection);
							bool ShaderEnabled = TheSettingManager->GetMenuShaderEnabled(MidSection);
							if (ShaderEnabled) TheShaderManager->SwitchShaderStatus(MidSection);
						}else if (SelectedColumn == 3) {
							// edit settings values
							int intvalue;
							float floatvalue;
							switch (SelectedNode.Type) {
								case SettingManager::Configuration::NodeType::Boolean:
									intvalue = TheSettingManager->GetSettingI(SelectedNode.Section, SelectedNode.Key);
									TheSettingManager->SetSetting(SelectedNode.Section, SelectedNode.Key, !intvalue);
									break;
								case SettingManager::Configuration::NodeType::Integer:
									intvalue = TheSettingManager->GetSettingI(SelectedNode.Section, SelectedNode.Key);
									TheSettingManager->SetSetting(SelectedNode.Section, SelectedNode.Key, UINT8(intvalue - 1));
									break;
								case SettingManager::Configuration::NodeType::Float:
									floatvalue = TheSettingManager->GetSettingF(SelectedNode.Section, SelectedNode.Key);
									TheSettingManager->SetSetting(SelectedNode.Section, SelectedNode.Key, floatvalue - 0.1f);
									break;
							}
						}
					} else if (Global->OnKeyDown(MenuSettings->KeySave)) {
						TheSettingManager->SaveSettings();
						InterfaceManager->ShowMessage("Settings saved.");
					}
				}
			}
			else {

				//handle entry using numpad keys
				if (Global->OnKeyDown(82))
					strcat(EditingValue, "0");
				else if (Global->OnKeyDown(79))
					strcat(EditingValue, "1");
				else if (Global->OnKeyDown(80))
					strcat(EditingValue, "2");
				else if (Global->OnKeyDown(81))
					strcat(EditingValue, "3");
				else if (Global->OnKeyDown(75))
					strcat(EditingValue, "4");
				else if (Global->OnKeyDown(76))
					strcat(EditingValue, "5");
				else if (Global->OnKeyDown(77))
					strcat(EditingValue, "6");
				else if (Global->OnKeyDown(71))
					strcat(EditingValue, "7");
				else if (Global->OnKeyDown(72))
					strcat(EditingValue, "8");
				else if (Global->OnKeyDown(73))
					strcat(EditingValue, "9");
				else if (Global->OnKeyDown(83))
					strcat(EditingValue, ".");
				else if (Global->OnKeyDown(74))
					strcat(EditingValue, "-");
				if (strlen(EditingValue) > 0 && Global->OnKeyDown(14)) EditingValue[strlen(EditingValue) - 1] = NULL;
			}

			Text = TitleMenu;
			SetRect(&Rect, PositionX, PositionY, PositionX + TitleColumnSize, PositionY + MenuSettings->TextSize);
			SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
			FontNormal->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
			FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorNormal);

			SetRect(&Rect, Rect.left, Rect.bottom + RowSpace, Rect.right, Rect.bottom + RowSpace + 2);
			TheRenderManager->device->Clear(1L, (D3DRECT*)&Rect, D3DCLEAR_TARGET, TextColorNormal, 0.0f, 0L);

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
						if (SelectedColumn >= 1) {
							FontSelected->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorSelected);
							FontSelected->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorSelected);
						}
						else {
							FontNormal->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
							FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorNormal);
						}
					}
					else {
						FontNormal->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
						FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorNormal);
					}

					// if in shader mode, add indication wether each shader is activated
					if (!memcmp(SelectedNode.Section, "Shaders", 7)) {
						if (SelectedRow[1] == Rows[1] && SelectedColumn >= 1)
							FontSelected->DrawTextA(NULL, Text, -1, &Rect, DT_CALCRECT, TextColorSelected);
						else
							FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_CALCRECT, TextColorNormal);
						Rect.left = Rect.right + 1;
						Rect.right += 100;
						SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
						bool enabled = TheSettingManager->GetMenuShaderEnabled(Text);
						strcpy(TextShaderStatus, enabled?"ENABLED":"DISABLED");
						if (SelectedRow[1] == Rows[1] && SelectedColumn >= 1) {
							if (!memcmp(TextShaderStatus, "ENABLED", 7)) {
								FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &RectShadow, DT_LEFT, TextShadowColorEnabled);
								FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &Rect, DT_LEFT, TextColorEnabled);
							}
							else {
								FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &RectShadow, DT_LEFT, TextShadowColorSelected);
								FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &Rect, DT_LEFT, TextColorSelected);
							}
						}
						else {
							if (!memcmp(TextShaderStatus, "ENABLED", 7)) {
								FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &RectShadow, DT_LEFT, TextShadowColorEnabled);
								FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &Rect, DT_LEFT, TextColorEnabled);
							}
							else {
								FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
								FontStatus->DrawTextA(NULL, TextShaderStatus, -1, &Rect, DT_LEFT, TextColorNormal);
							}
						}
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
						if (SelectedColumn >= 2) {
							FontSelected->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorSelected);
							FontSelected->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorSelected);
						}
						else {
							FontNormal->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
							FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorNormal);
						}
					}
					else {
						FontNormal->DrawTextA(NULL, Text, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
						FontNormal->DrawTextA(NULL, Text, -1, &Rect, DT_LEFT, TextColorNormal);
					}
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
					if (SelectedRow[3] == Rows[3]) {
						memcpy((void*)&SelectedNode, Setting._Ptr, sizeof(SettingManager::Configuration::ConfigNode));
						if (SelectedColumn >= 3) {
							if (EditingMode) {
								strcpy(SelectedNode.Value, EditingValue);
								strcpy(SettingText, Setting->Key);
								strcat(SettingText, " = ");
								strcat(SettingText, EditingValue);
								FontSelected->DrawTextA(NULL, SettingText, -1, &RectShadow, DT_LEFT, TextShadowColorEditing);
								FontSelected->DrawTextA(NULL, SettingText, -1, &Rect, DT_LEFT, TextColorEditing);
							}
							else {
								FontSelected->DrawTextA(NULL, SettingText, -1, &RectShadow, DT_LEFT, TextShadowColorSelected);
								FontSelected->DrawTextA(NULL, SettingText, -1, &Rect, DT_LEFT, TextColorSelected);
							}
						}
						else {
							FontNormal->DrawTextA(NULL, SettingText, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
							FontNormal->DrawTextA(NULL, SettingText, -1, &Rect, DT_LEFT, TextColorNormal);
						}
					}
					else {
						FontNormal->DrawTextA(NULL, SettingText, -1, &RectShadow, DT_LEFT, TextShadowColorNormal);
						FontNormal->DrawTextA(NULL, SettingText, -1, &Rect, DT_LEFT, TextColorNormal);
					}
					Rows[3]++;
				}
				Setting++;
			}
		}
	}

}

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
