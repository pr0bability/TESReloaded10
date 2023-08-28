#pragma once

class GameMenuManager { // Never disposed
public:
	static void Initialize();

	void					Render();
	bool					IsKeyPressed(UInt16 KeyCode);

	bool										Enabled;
	bool										EditingMode;
	UInt32										Pages[4];
	UInt32										SelectedPage[4];
	UInt32										Rows[4];
	UInt32										SelectedRow[4];
	UInt32										SelectedColumn;
	char										EditingValue[20];
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

};