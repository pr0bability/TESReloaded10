#define MenuSettings TheSettingManager->SettingsMain.Menu
#define TextColorNormal D3DCOLOR_XRGB(MenuSettings.TextColorNormal[0], MenuSettings.TextColorNormal[1], MenuSettings.TextColorNormal[2])
#define TextShadowColorNormal D3DCOLOR_XRGB(MenuSettings.TextShadowColorNormal[0], MenuSettings.TextShadowColorNormal[1], MenuSettings.TextShadowColorNormal[2])
#define TextColorSelected D3DCOLOR_XRGB(MenuSettings.TextColorSelected[0], MenuSettings.TextColorSelected[1], MenuSettings.TextColorSelected[2])
#define TextShadowColorSelected D3DCOLOR_XRGB(MenuSettings.TextShadowColorSelected[0], MenuSettings.TextShadowColorSelected[1], MenuSettings.TextShadowColorSelected[2])
#define TextColorEditing D3DCOLOR_XRGB(MenuSettings.TextColorEditing[0], MenuSettings.TextColorEditing[1], MenuSettings.TextColorEditing[2])
#define TextShadowColorEditing D3DCOLOR_XRGB(MenuSettings.TextShadowColorEditing[0], MenuSettings.TextShadowColorEditing[1], MenuSettings.TextShadowColorEditing[2])
#define TextColorEnabled D3DCOLOR_XRGB(MenuSettings.TextColorEnabled[0], MenuSettings.TextColorEnabled[1], MenuSettings.TextColorEnabled[2])
#define TextShadowColorEnabled D3DCOLOR_XRGB(MenuSettings.TextShadowColorEnabled[0], MenuSettings.TextShadowColorEnabled[1], MenuSettings.TextShadowColorEnabled[2])
#define PositionX TheSettingManager->SettingsMain.Menu.PositionX
#define PositionY TheSettingManager->SettingsMain.Menu.PositionY
#define TitleColumnSize TheSettingManager->SettingsMain.Menu.TitleColumnSize
#define MainItemColumnSize TheSettingManager->SettingsMain.Menu.MainItemColumnSize
#define ItemColumnSize TheSettingManager->SettingsMain.Menu.ItemColumnSize
#define RowSpace TheSettingManager->SettingsMain.Menu.RowSpace
#define RowsPerPage TheSettingManager->SettingsMain.Menu.RowsPerPage

//#define debugMenuInput

void GameMenuManager::Initialize() {

	Logger::Log("Starting the menu manager...");
	TheGameMenuManager = new GameMenuManager();

	TheGameMenuManager->SelectedColumn = 0;
	TheGameMenuManager->SelectedRow[4] = { 0 };
	TheGameMenuManager->SelectedPage[4] = { 0 };
	TheGameMenuManager->Enabled = false;
	TheGameMenuManager->EditingMode = false;
	TheGameMenuManager->MainMenuOn = false;

	D3DXCreateFontA(TheRenderManager->device, MenuSettings.TextSize, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, MenuSettings.TextFont, &TheGameMenuManager->FontNormal);
	D3DXCreateFontA(TheRenderManager->device, MenuSettings.TextSize, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, MenuSettings.TextFont, &TheGameMenuManager->FontSelected);
	D3DXCreateFontA(TheRenderManager->device, MenuSettings.TextSizeStatus, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, MenuSettings.TextFontStatus, &TheGameMenuManager->FontStatus);

	TheGameMenuManager->Keys[1] = "Esc";
	TheGameMenuManager->Keys[2] = "1";
	TheGameMenuManager->Keys[3] = "2";
	TheGameMenuManager->Keys[4] = "3";
	TheGameMenuManager->Keys[5] = "4";
	TheGameMenuManager->Keys[6] = "5";
	TheGameMenuManager->Keys[7] = "6";
	TheGameMenuManager->Keys[8] = "7";
	TheGameMenuManager->Keys[9] = "8";
	TheGameMenuManager->Keys[10] = "9";
	TheGameMenuManager->Keys[11] = "0";
	TheGameMenuManager->Keys[12] = "-";
	TheGameMenuManager->Keys[13] = "=";
	TheGameMenuManager->Keys[14] = "Backspace";
	TheGameMenuManager->Keys[15] = "Tab";
	TheGameMenuManager->Keys[16] = "Q";
	TheGameMenuManager->Keys[17] = "W";
	TheGameMenuManager->Keys[18] = "E";
	TheGameMenuManager->Keys[19] = "R";
	TheGameMenuManager->Keys[20] = "T";
	TheGameMenuManager->Keys[21] = "Y";
	TheGameMenuManager->Keys[22] = "U";
	TheGameMenuManager->Keys[23] = "I";
	TheGameMenuManager->Keys[24] = "O";
	TheGameMenuManager->Keys[25] = "P";
	TheGameMenuManager->Keys[26] = "[";
	TheGameMenuManager->Keys[27] = "]";
	TheGameMenuManager->Keys[28] = "Enter";
	TheGameMenuManager->Keys[29] = "Left Ctrl";
	TheGameMenuManager->Keys[30] = "A";
	TheGameMenuManager->Keys[31] = "S";
	TheGameMenuManager->Keys[32] = "D";
	TheGameMenuManager->Keys[33] = "F";
	TheGameMenuManager->Keys[34] = "G";
	TheGameMenuManager->Keys[35] = "H";
	TheGameMenuManager->Keys[36] = "J";
	TheGameMenuManager->Keys[37] = "K";
	TheGameMenuManager->Keys[38] = "L";
	TheGameMenuManager->Keys[39] = ";";
	TheGameMenuManager->Keys[40] = "'";
	TheGameMenuManager->Keys[41] = "`";
	TheGameMenuManager->Keys[42] = "Left Shift";
	TheGameMenuManager->Keys[43] = "\\";
	TheGameMenuManager->Keys[44] = "Z";
	TheGameMenuManager->Keys[45] = "X";
	TheGameMenuManager->Keys[46] = "C";
	TheGameMenuManager->Keys[47] = "V";
	TheGameMenuManager->Keys[48] = "B";
	TheGameMenuManager->Keys[49] = "N";
	TheGameMenuManager->Keys[50] = "M";
	TheGameMenuManager->Keys[51] = ",";
	TheGameMenuManager->Keys[52] = ".";
	TheGameMenuManager->Keys[53] = "/";
	TheGameMenuManager->Keys[54] = "Right Shift";
	TheGameMenuManager->Keys[55] = "Keypad *";
	TheGameMenuManager->Keys[56] = "Left Alt";
	TheGameMenuManager->Keys[57] = "Spacebar";
	TheGameMenuManager->Keys[58] = "Caps Lock";
	TheGameMenuManager->Keys[59] = "F1";
	TheGameMenuManager->Keys[60] = "F2";
	TheGameMenuManager->Keys[61] = "F3";
	TheGameMenuManager->Keys[62] = "F4";
	TheGameMenuManager->Keys[63] = "F5";
	TheGameMenuManager->Keys[64] = "F6";
	TheGameMenuManager->Keys[65] = "F7";
	TheGameMenuManager->Keys[66] = "F8";
	TheGameMenuManager->Keys[67] = "F9";
	TheGameMenuManager->Keys[68] = "F10";
	TheGameMenuManager->Keys[69] = "Num Lock";
	TheGameMenuManager->Keys[70] = "Scroll Lock";
	TheGameMenuManager->Keys[71] = "Keypad 7";
	TheGameMenuManager->Keys[72] = "Keypad 8";
	TheGameMenuManager->Keys[73] = "Keypad 9";
	TheGameMenuManager->Keys[74] = "Keypad -";
	TheGameMenuManager->Keys[75] = "Keypad 4";
	TheGameMenuManager->Keys[76] = "Keypad 5";
	TheGameMenuManager->Keys[77] = "Keypad 6";
	TheGameMenuManager->Keys[78] = "Keypad +";
	TheGameMenuManager->Keys[79] = "Keypad 1";
	TheGameMenuManager->Keys[80] = "Keypad 2";
	TheGameMenuManager->Keys[81] = "Keypad 3";
	TheGameMenuManager->Keys[82] = "Keypad 0";
	TheGameMenuManager->Keys[83] = "Keypad .";
	TheGameMenuManager->Keys[87] = "F11";
	TheGameMenuManager->Keys[88] = "F12";
	TheGameMenuManager->Keys[156] = "Keypad Enter";
	TheGameMenuManager->Keys[157] = "Right Control";
	TheGameMenuManager->Keys[181] = "Keypad /";
	TheGameMenuManager->Keys[184] = "Right Alt";
	TheGameMenuManager->Keys[199] = "Home";
	TheGameMenuManager->Keys[200] = "Up Arrow";
	TheGameMenuManager->Keys[201] = "PgUp";
	TheGameMenuManager->Keys[203] = "Left Arrow";
	TheGameMenuManager->Keys[205] = "Right Arrow";
	TheGameMenuManager->Keys[207] = "End";
	TheGameMenuManager->Keys[208] = "Down Arrow";
	TheGameMenuManager->Keys[209] = "PgDown";
	TheGameMenuManager->Keys[210] = "Insert";
	TheGameMenuManager->Keys[211] = "Delete";
}

void GameMenuManager::MainMenuMessage() {
	std::chrono::time_point now = std::chrono::system_clock::now();

	// on main menu, only draw the bottom text as signal the mod has loaded
	if (!MainMenuOn) {
		MainMenuOn = true;
		MainMenuStartTime = now;
	}

	std::chrono::duration<double> elapsed_seconds = now - MainMenuStartTime;
	if (elapsed_seconds.count() > 5.0) return; // only show message at the bottom of the screen for 5 seconds

	std::string menuMessage = std::string(PluginVersion::VersionString) + " - Open the Config Menu by pressing the key " + Keys[MenuSettings.KeyEnable];

	SetRect(&Rect, 0, TheRenderManager->height - MenuSettings.TextSize - 10, TheRenderManager->width, TheRenderManager->height + MenuSettings.TextSize);
	SetRect(&RectShadow, Rect.left + 1, Rect.top + 1, Rect.right + 1, Rect.bottom + 1);
	FontNormal->DrawTextA(NULL, menuMessage.c_str(), -1, &RectShadow, DT_CENTER, TextShadowColorNormal);
	FontNormal->DrawTextA(NULL, menuMessage.c_str(), -1, &Rect, DT_CENTER, TextColorNormal);
}

void GameMenuManager::HandleInput() {
	bool menuKeyDown = Global->OnKeyDown(MenuSettings.KeyEnable);

#ifdef debugMenuInput
	if (TheShaderManager->GameState.isExterior) Logger::Log("====Exterior====");
	else Logger::Log("====Interior====");
	Logger::Log("Menu key %i is down? %i", MenuSettings.KeyEnable, menuKeyDown);
#endif

	// check for menu key to toggle display of the menu
	if (menuKeyDown) {
		Enabled = !Enabled;
#ifdef debugMenuInput
		Logger::Log("toggling menu : %i", Enabled);
#endif

		lastKeyPressed = std::chrono::system_clock::now();
		keyDown = 999;
		EditingMode = false;
	}
	if (!InterfaceManager->IsActive(Menu::MenuType::kMenuType_None)) {
		// cancel out the menu if a game menu is active
		Enabled = false;
		EditingMode = false;
		return;
	}

	if (!Enabled) return; // menu is disabled, no need to check further inputs

	bool isShaderSection = !memcmp(SelectedNode.Section, "Shaders", 7);
	bool isStatusSection = !memcmp(SelectedNode.Section + strlen(SelectedNode.Section) - 6, "Status", 6);
	bool isWeatherSection = !memcmp(SelectedNode.Section, "Weathers", 8);

	bool useNumpad = TheSettingManager->GetSettingI("Main.Menu.Keys", "EntryUseNumpad");

	//handle entry using numpad keys or number row keys depending on setting
	int toggleEntry = useNumpad ? MenuSettings.KeyEditing : 13;
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
	if (Global->OnKeyDown(toggleEntry) && SelectedColumn == 3 && !isStatusSection) {
		EditingMode = !EditingMode;
		
		if (EditingMode)
			strcpy(EditingValue, SelectedNode.Value); 								// start editing, copy the current value to the edited string
		else
			TheSettingManager->SetSettingF(SelectedNode.Section, SelectedNode.Key, atof(EditingValue));			// finished editing, sets the value. 
	}

	if (EditingMode) {

		if (IsKeyPressed(key0))
			strcat(EditingValue, "0");
		else if (IsKeyPressed(key1))
			strcat(EditingValue, "1");
		else if (IsKeyPressed(key2))
			strcat(EditingValue, "2");
		else if (IsKeyPressed(key3))
			strcat(EditingValue, "3");
		else if (IsKeyPressed(key4))
			strcat(EditingValue, "4");
		else if (IsKeyPressed(key5))
			strcat(EditingValue, "5");
		else if (IsKeyPressed(key6))
			strcat(EditingValue, "6");
		else if (IsKeyPressed(key7))
			strcat(EditingValue, "7");
		else if (IsKeyPressed(key8))
			strcat(EditingValue, "8");
		else if (IsKeyPressed(key9))
			strcat(EditingValue, "9");
		else if (IsKeyPressed(keyDot))
			strcat(EditingValue, ".");
		else if (IsKeyPressed(keyMinus))
			strcat(EditingValue, "-");
		if (strlen(EditingValue) > 0 && IsKeyPressed(14)) EditingValue[strlen(EditingValue) - 1] = NULL;
	}
	else if (!SelectedNode.Section) {
		return; //SelectedNode.Section is empty the first time the menu renders
	}

#ifdef debugMenuInput
	Logger::Log("Selected column: %i, selected row: %i, Selected page: %i", SelectedColumn, SelectedRow[SelectedColumn], SelectedPage[SelectedColumn]);
	Logger::Log("Selected setting: %s.%s ", SelectedNode.Section, SelectedNode.Key);
#endif

	// handle other types of user input
	if (Global->OnKeyDown(MenuSettings.KeySave)) {
		TheSettingManager->SaveSettings();
		InterfaceManager->ShowMessage("Settings saved.");
	}
	else if (SelectedColumn == COLUMNS::HEADER) {
		// header is a column tilted to the side (column 0). Left and Right change rows and Down moves to column 1 (or the actual menu)
		SelectedPage[COLUMNS::CATEGORY] = SelectedPage[COLUMNS::SECTION] = SelectedPage[COLUMNS::SETTINGS] = 0;

		if (IsKeyPressed(MenuSettings.KeyDown)) {
			SelectedColumn = COLUMNS::CATEGORY;
		}
		else if (IsKeyPressed(MenuSettings.KeyLeft)) {
			SelectedRow[SelectedColumn] = max(0, SelectedRow[SelectedColumn] - 1);
		}
		else if (IsKeyPressed(MenuSettings.KeyRight)) {
			SelectedRow[SelectedColumn] = min(SelectedRow[SelectedColumn] + 1, Rows[SelectedColumn]);
		}
		else if (IsKeyPressed(MenuSettings.KeyUp)) {
			SelectedColumn = COLUMNS::CATEGORY;
			SelectedRow[SelectedColumn] = Rows[SelectedColumn];
			SelectedPage[SelectedColumn] = Pages[SelectedColumn];
		}
	}
	else {
		// handle navigation
		if (IsKeyPressed(MenuSettings.KeyUp)) {
			if (SelectedRow[SelectedColumn] == 0)
				if (SelectedColumn == COLUMNS::CATEGORY && SelectedRow[COLUMNS::CATEGORY] == 0)
					SelectedColumn = COLUMNS::HEADER;  // go to header
				else {
					SelectedRow[SelectedColumn] = Rows[SelectedColumn]; // go to the end
					SelectedPage[SelectedColumn] = Pages[SelectedColumn];
				}
			else {
				SelectedRow[SelectedColumn] = SelectedRow[SelectedColumn] - 1;
				if (SelectedRow[SelectedColumn] < SelectedPage[SelectedColumn] * RowsPerPage) {
					SelectedPage[SelectedColumn] = SelectedPage[SelectedColumn] - 1;
				}
			}

			if (SelectedColumn < COLUMNS::SETTINGS) {
				// reset selected rows for columns further than current one
				for (int i = SelectedColumn + 1; i <= COLUMNS::SETTINGS; i++) {
					SelectedRow[i] = 0; 
				}
			}
		}
		else if (IsKeyPressed(MenuSettings.KeyDown)) {

			if (SelectedRow[SelectedColumn] >= Rows[SelectedColumn]) {
				// if we're on the last row, we cycle to the top
				SelectedRow[SelectedColumn] = 0;
				SelectedPage[SelectedColumn] = 0; // Go to first page
			}
			else {
				SelectedRow[SelectedColumn] = SelectedRow[SelectedColumn] + 1;
				if (SelectedRow[SelectedColumn] >= (SelectedPage[SelectedColumn] + 1) * RowsPerPage) {
					SelectedPage[SelectedColumn] = SelectedPage[SelectedColumn] + 1; // Go to next page
				}
			}

			if (SelectedColumn < COLUMNS::SETTINGS) {
				// reset selected rows for columns further than current one
				for (int i = SelectedColumn + 1; i <= COLUMNS::SETTINGS; i++) {
					SelectedRow[i] = 0; 
				}
			}
		}
		else if (IsKeyPressed(MenuSettings.KeyLeft)) {
			if (SelectedColumn == COLUMNS::CATEGORY) {
				SelectedRow[SelectedColumn] = 0;
				SelectedColumn = COLUMNS::HEADER;  // go to header
			}
			else 
				SelectedColumn = max(SelectedColumn - 1, 0);
		}
		else if (IsKeyPressed(MenuSettings.KeyRight)) {
			SelectedColumn = min(SelectedColumn + 1, COLUMNS::SETTINGS);
		}
		else if (IsKeyPressed(MenuSettings.KeyPageUp)) {
			SelectedPage[SelectedColumn] = max(SelectedPage[SelectedColumn] - 1, 0);
			SelectedRow[SelectedColumn] = SelectedPage[SelectedColumn] * RowsPerPage;
		}
		else if (IsKeyPressed(MenuSettings.KeyPageDown)) {
			SelectedPage[SelectedColumn] = min(SelectedPage[SelectedColumn] + 1, Pages[SelectedColumn]);
			SelectedRow[SelectedColumn] = SelectedPage[SelectedColumn] * RowsPerPage;
		}
		else if (IsKeyPressed(MenuSettings.KeyAdd)) {
			// handle value add/subtract keys
			//Logger::Log("Add for %s.%s, isShader Section? %i, isStatusSection? %i, Column %i",SelectedNode.Section, SelectedNode.Key, isShaderSection, isStatusSection, SelectedColumn);

			// react to user key input to reduce the value of the setting
			if (isShaderSection && (SelectedColumn == COLUMNS::CATEGORY || (SelectedColumn == COLUMNS::SETTINGS && isStatusSection))) {
				// enable shaders and effects
				bool ShaderEnabled = TheSettingManager->GetMenuShaderEnabled(SelectedNode.MidSection);
				if (!ShaderEnabled) TheShaderManager->SwitchShaderStatus(SelectedNode.MidSection);
			}
			else if (SelectedColumn == COLUMNS::CATEGORY && isWeatherSection) {
				TESWeather* Weather = (TESWeather*)DataHandler->GetFormByName(SelectedNode.MidSection, TESForm::FormType::kFormType_Weather);
				Tes->sky->ForceWeather(Weather);
			}
			else if (SelectedColumn == COLUMNS::SETTINGS) {
				TheSettingManager->Increment(SelectedNode.Section, SelectedNode.Key);
			}
			TheSettingManager->LoadSettings(); //update constants stored in Settings structs
		}
		else if (IsKeyPressed(MenuSettings.KeySubtract)) {
			//Logger::Log("Subtract for %s.%s, isShader Section? %i, isStatusSection? %i, Column %i", SelectedNode.Section, SelectedNode.Key, isShaderSection, isStatusSection, SelectedColumn);

			// react to user key input to reduce the value of the setting
			if (isShaderSection && (SelectedColumn == COLUMNS::CATEGORY || (SelectedColumn == COLUMNS::SETTINGS && isStatusSection))) {
				// disable shaders and effects
				bool ShaderEnabled = TheSettingManager->GetMenuShaderEnabled(SelectedNode.MidSection);
				if (ShaderEnabled) TheShaderManager->SwitchShaderStatus(SelectedNode.MidSection);
			}
			else if (SelectedColumn == COLUMNS::SETTINGS) {
				TheSettingManager->Decrement(SelectedNode.Section, SelectedNode.Key);
			}
			TheSettingManager->LoadSettings(); //update constants stored in Settings structs
		}
	}
}


void GameMenuManager::DrawLine(int x, int y, int length) {
	float posX = PositionX + x;
	float posY = PositionY + y;
	SetRect(&Rect, posX, posY, posX + length, posY + 2);
	TheRenderManager->device->Clear(1L, (D3DRECT*)&Rect, D3DCLEAR_TARGET, TextColorNormal, 0.0f, 0L);
}


int GameMenuManager::DrawShadowedText(const char* text, int x, int y, int width, D3DCOLOR color, ID3DXFont* Font, int Alignment) {
	float posX = PositionX + x;
	float posY = PositionY + y;
	RECT rectangle;
	RECT rectangleShadow;

	SetRect(&rectangle, posX, posY, posX + width, posY + MenuSettings.TextSize);

	if (Alignment == DT_LEFT)
		Font->DrawTextA(NULL, text, -1, &rectangle, DT_CALCRECT, TextShadowColorNormal); // calculate rectangle

	SetRect(&rectangleShadow, rectangle.left + 1, rectangle.top + 1, rectangle.right + 1, rectangle.bottom + 1);

	Font->DrawTextA(NULL, text, -1, &rectangleShadow, Alignment, TextShadowColorNormal);
	Font->DrawTextA(NULL, text, -1, &rectangle, Alignment, color);

	return (int)rectangle.right - PositionX;
}


void GameMenuManager::Render() {

	StringList Sections;
	SettingManager::Configuration::SettingList Settings;

	int CurrentColumn = 0;
	char TextShaderStatus[20];
	size_t ListSize = 0;

	if (InterfaceManager->IsActive(Menu::MenuType::kMenuType_Main)) {
		MainMenuMessage();
		return;
	}

	HandleInput();
	if (!Enabled) return; // skip render if menu is disabled

	DrawShadowedText(TitleMenu, 0, 0, TitleColumnSize, TextColorNormal, FontNormal, DT_LEFT);

	if (TheSettingManager->hasUnsavedChanges)
		DrawShadowedText("/!\\ You have unsaved changes. Save them using the Enter key.", MainItemColumnSize * 3, 0, MainItemColumnSize * 2, TextColorNormal, FontNormal, DT_LEFT);

	DrawLine(0, MenuSettings.TextSize + RowSpace, TitleColumnSize); 	// draw line under Title

	bool useNumpad = TheSettingManager->GetSettingI("Main.Menu.Keys", "EntryUseNumpad");
	int toggleEntry = useNumpad ? MenuSettings.KeyEditing : 13;

	 std::string KeysInfo = "Keybinds: Enable/Increment: " + Keys[MenuSettings.KeyAdd] + ", Disable/Decrement: " + Keys[MenuSettings.KeySubtract] + ", Start Editing value: " + Keys[toggleEntry] + ", Save Settings: " + Keys[MenuSettings.KeySave];
	DrawShadowedText(KeysInfo.c_str(), 0, MenuSettings.TextSize + RowSpace + 2, MainItemColumnSize * 2, TextColorNormal, FontNormal, DT_LEFT);

	DrawLine(0, MenuSettings.TextSize * 2 + RowSpace * 2 + 2, TitleColumnSize); 	// draw line under Keymap advice

	// render header as horizontal column
	int HeaderYPos = (MenuSettings.TextSize * 2 + RowSpace * 2 + 4);
	TheSettingManager->FillMenuSections(&Sections, NULL);
	ListSize = Sections.size();
	Rows[COLUMNS::HEADER] = ListSize - 1;
	Pages[COLUMNS::HEADER] = 0;
	for (UInt32 i = 0; i < ListSize; i++) {
		ID3DXFont* Font = FontNormal;
		D3DXCOLOR textColor = TextColorNormal;
		
		if (SelectedRow[COLUMNS::HEADER] == i) {
			strcpy(SelectedNode.Section, Sections[i].c_str());

			Font = FontSelected;
			textColor = TextColorSelected;
		}
		
		DrawShadowedText(Sections[i].c_str(), MainItemColumnSize * i, HeaderYPos, MainItemColumnSize, textColor, Font, DT_LEFT);
	}

	// draw line under header
	DrawLine(0, HeaderYPos + MenuSettings.TextSize + RowSpace, 3 * ItemColumnSize);

	bool isShaderSection = !memcmp(SelectedNode.Section, "Shaders", 7);

	// render left column (shaders/menu category names)
	int MenuHeight = HeaderYPos + MenuSettings.TextSize + RowSpace * 3 + 2;
	int rowHeight = MenuSettings.TextSize + RowSpace;

	TheSettingManager->FillMenuSections(&Sections, SelectedNode.Section);
	ListSize = Sections.size();
	CurrentColumn = COLUMNS::CATEGORY;
	Rows[CurrentColumn] = ListSize - 1;
	Pages[CurrentColumn] = ListSize / RowsPerPage;
	for (UInt32 i = RowsPerPage * SelectedPage[CurrentColumn]; i < min(ListSize, RowsPerPage * (SelectedPage[CurrentColumn] + 1)); i++) {
		ID3DXFont* Font = FontNormal;
		D3DXCOLOR textColor = TextColorNormal;
		int lineYPos = MenuHeight + rowHeight * (i % RowsPerPage);

		if (SelectedRow[CurrentColumn] == i ) {
			strcat(SelectedNode.Section, ".");
			strcat(SelectedNode.Section, Sections[i].c_str());

			if (SelectedColumn >= CurrentColumn){
				Font = FontSelected;
				textColor = TextColorSelected;
			}
		}

		int pos = DrawShadowedText(Sections[i].c_str(), 0, lineYPos, ItemColumnSize, textColor, Font, DT_LEFT);

		// if in shader mode, add indication wether each shader is activated
		if (isShaderSection) {
			bool enabled = TheSettingManager->GetMenuShaderEnabled(Sections[i].c_str());
			if (enabled) textColor = TextColorEnabled;

			DrawShadowedText(enabled ? "ENABLED" : "DISABLED", pos + 1, lineYPos, 100, textColor, FontStatus, DT_LEFT);

			// show render time for effect if debug mode enabled
			EffectRecord* effect = TheShaderManager->GetEffectByName(Sections[i].c_str());
			if (effect && TheSettingManager->SettingsMain.Develop.DebugMode) { 
				std::stringstream ss;

				float total = max(effect->renderTime + effect->constantUpdateTime, 0);

				// in the case of Shadows, we add the time spent rendering the shadows buffer and shadow maps
				if ((effect == TheShaderManager->Effects.ShadowsExteriors && TheShaderManager->GameState.isExterior)|| 
					(effect == TheShaderManager->Effects.ShadowsInteriors && !TheShaderManager->GameState.isExterior)) {

					total += TheShaderManager->Effects.PointShadows->renderTime;
					total += TheShaderManager->Effects.PointShadows2->renderTime;
					total += TheShaderManager->Effects.SunShadows->renderTime;
					total += TheShadowManager->shadowMapsRenderTime;
				}

				if (!TheSettingManager->SettingsMain.Main.RenderEffects) total = 0;

				ss << std::fixed << std::setprecision(4) << total;
				std::string duration = ss.str();
				duration += " ms";

				//Logger::Log("%s render time: %s", Sections[i].c_str(), duration.c_str());

				DrawShadowedText(duration.c_str(), 0, lineYPos, ItemColumnSize - MenuSettings.TextSize, TextColorNormal, FontNormal, DT_RIGHT);
			}
		}
	}

	// render middle column (shader/menu subsection names)
	TheSettingManager->FillMenuSections(&Sections, SelectedNode.Section); // get a list of sections for a given category
	ListSize = Sections.size();
	CurrentColumn = COLUMNS::SECTION;
	Rows[CurrentColumn] = ListSize - 1;
	Pages[CurrentColumn] = ListSize / RowsPerPage;
	for (UInt32 i = RowsPerPage * SelectedPage[CurrentColumn]; i < min(ListSize, RowsPerPage * (SelectedPage[CurrentColumn] + 1)); i++) {
		// Selected is true as long as we are deeper in the menu than current column
		ID3DXFont* Font = FontNormal;
		D3DXCOLOR textColor = TextColorNormal;

		if (SelectedRow[CurrentColumn] == i) { // row is selected
			strcat(SelectedNode.Section, ".");
			strcat(SelectedNode.Section, Sections[i].c_str());

			if (SelectedColumn >= CurrentColumn) {
				Font = FontSelected;
				textColor = TextColorSelected;
			}
		}

		DrawShadowedText(Sections[i].c_str(), ItemColumnSize * (CurrentColumn - 1), MenuHeight + rowHeight * (i % RowsPerPage), ItemColumnSize, textColor, Font, DT_LEFT);
	}

	// render right column (settings name/value pairs)
	TheSettingManager->FillMenuSettings(&Settings, SelectedNode.Section); // build a setting list to display values
	ListSize = Settings.size();
	CurrentColumn = COLUMNS::SETTINGS;
	Rows[CurrentColumn] = ListSize - 1;
	Pages[CurrentColumn] = ListSize / RowsPerPage;
	SettingManager::Configuration::SettingList::iterator Setting = Settings.begin();

	for (UInt32 i = RowsPerPage * SelectedPage[CurrentColumn]; i < min(ListSize, RowsPerPage * (SelectedPage[CurrentColumn] + 1)); i++) {
		//SettingManager::Configuration::ConfigNode Setting = Settings[i];

		D3DXCOLOR textColor = TextColorNormal;

		char SettingText[80];
		strcpy(SettingText, Setting->Key);
		strcat(SettingText, " = ");


		if (SelectedRow[CurrentColumn] == i) { // row is selected
			memcpy((void*)&SelectedNode, Setting._Ptr, sizeof(SettingManager::Configuration::ConfigNode));

			if (SelectedColumn == CurrentColumn) {
				if (EditingMode) {
					strcat(SettingText, EditingValue);
					textColor = TextColorEditing;
				}
				else {
					strcat(SettingText, Setting->Value);
					textColor = TextColorSelected;
				}
			}
			else {
				strcat(SettingText, Setting->Value);
			}
		}
		else {
			strcat(SettingText, Setting->Value);
		}
	
		DrawShadowedText(SettingText, ItemColumnSize * (CurrentColumn - 1), MenuHeight + rowHeight * (i % RowsPerPage), ItemColumnSize, textColor, FontNormal, DT_LEFT);
		Setting++;
	}

	//Draw Description/Help line
	const char* DescriptionText = "";
	if (SelectedColumn == COLUMNS::SETTINGS) {
		DescriptionText = SelectedNode.Description.c_str();
	}
	else if (isShaderSection && SelectedColumn == COLUMNS::CATEGORY) {
		// Get the general description of the effect from the Status.Enabled node of the Shader settings
		SettingManager::Configuration::ConfigNode StatusNode;
		char statusSection[256];
		strcpy(statusSection, "Shaders.");
		strcat(statusSection, SelectedNode.MidSection);
		strcat(statusSection, ".Status");
		bool success = TheSettingManager->Config.FillNode(&StatusNode, statusSection, "Enabled");

		DescriptionText = success?StatusNode.Description.c_str():"";
	}

	// render description
	DrawShadowedText(DescriptionText, ItemColumnSize, HeaderYPos, ItemColumnSize * 2, TextColorNormal, FontNormal, DT_LEFT);
}


// Returns a key pressed value at regular intervals if key is held down
bool GameMenuManager::IsKeyPressed(UInt16 KeyCode){
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = now - lastKeyPressed;

	bool key = Global->OnKeyPressed(KeyCode); 
	bool down = Global->OnKeyDown(KeyCode);

	if (key || down){
		if (down || (KeyCode != keyDown || elapsed_seconds.count() > 0.2)) {
			// reset counter
			lastKeyPressed = now;
			keyDown = KeyCode;
			return true;
		}
	}
	else if (KeyCode == keyDown) {
		// Key is up after being down
		keyDown = 999;
	}

	return false;
}
