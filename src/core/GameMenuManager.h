#pragma once

typedef std::map<int, std::string> KeyCodes;

class GameMenuManager { // Never disposed
public:
	static void Initialize();

	enum COLUMNS {
		HEADER = 0,
		CATEGORY = 1,
		SECTION = 2,
		SETTINGS = 3,
	};

	void					ValidateSelection();
	void					Render();
	void					HandleInput();
	void					MainMenuMessage();
	void					UpdateSettings();
	void					DrawLine(int x, int y, int length);
	int 					DrawShadowedText(const char* text, int x, int y, int width, D3DCOLOR color, ID3DXFont* Font, int Alignment);
	bool					IsKeyPressed(UInt16 KeyCode);
	std::string				GetKeyName(int keyCode);

	bool										Enabled;
	bool										EditingMode;
	int 										Pages[4];
	int 										SelectedPage[4];
	int 										Rows[4];
	int 										SelectedRow[4];
	int 										SelectedColumn;
	char										EditingValue[20];
	int											textSize;
	KeyCodes									Keys;

	SettingManager::Configuration::ConfigNode	SelectedNode;
	ID3DXFont*									FontSelected;
	ID3DXFont*									FontNormal;
	ID3DXFont*									FontStatus;
	RECT										Rect;
	RECT										RectShadow;
	std::chrono::system_clock::time_point		MainMenuStartTime;
	bool										MainMenuOn;
	std::chrono::system_clock::time_point		lastKeyPressed;
	UInt16										keyDown;

	enum MenuPauseState : uint32_t {
		MENU_PAUSED	= 0, // Pauses the game, runs MenuMode only
		MENU_LIVE	= 1, // Does not pause the game, runs both GameMode and MenuMode
		MENU_MIXED	= 2, // Does not pause the game, runs only MenuMode
	};

	MenuPauseState(__cdecl* IsLiveMenu)(uint32_t aeMenu, bool abCheckInstances, bool abGameModeCheck);

	int HeaderYPos;
	int MenuHeight;
	int rowHeight;
	int margins;
	int pageSize;
	int TitleColumnWidth;
	int MainColumnWidth;
	int ItemColumnWidth;
};