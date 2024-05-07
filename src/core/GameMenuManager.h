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

	void					Render();
	void					HandleInput();
	void					MainMenuMessage();
	void					UpdateSettings();
	void					DrawLine(int x, int y, int length);
	int 					DrawShadowedText(const char* text, int x, int y, int width, D3DCOLOR color, ID3DXFont* Font, int Alignment);
	bool					IsKeyPressed(UInt16 KeyCode);

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

	int HeaderYPos;
	int MenuHeight;
	int rowHeight;
	int margins;
	int pageSize;
	int TitleColumnWidth;
	int MainColumnWidth;
	int ItemColumnWidth;
};