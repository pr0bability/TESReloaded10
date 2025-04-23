#pragma once

typedef UInt32	PluginHandle;	// treat this as an opaque type

class CommandInfo;

class PluginInterface {
public:
	enum Interface {
		kInterface_Serialization = 0,
		kInterface_Console,

		// Added v0002
		kInterface_Messaging,
		kInterface_CommandTable,

		// Added v0004
		kInterface_StringVar,
		kInterface_ArrayVar,
		kInterface_Script,

		// Added v0005 - version bumped to 3
		kInterface_Data,
		// Added v0006
		kInterface_EventManager,

		kInterface_Max
	};

	UInt32	Version;
	UInt32	GameVersion;
	UInt32	EditorVersion;
	UInt32	IsEditor;

	bool	(*RegisterCommand)(CommandInfo* Info);
	void	(*SetOpcodeBase)(UInt32 Opcode);
	void*	(*QueryInterface)(UInt32 ID);

	PluginHandle(*GetPluginHandle)(void);
};

class PluginInfo {
public:
	enum {
		kInfoVersion = 2
	};

	UInt32		InfoVersion;
	const char* Name;
	UInt32		Version;
};

class CommandTableInterface {
public:
	CommandInfo* (*Start)();
	CommandInfo* (*End)();
	CommandInfo* (*GetByOpcode)(UInt32 Opcode);
	CommandInfo* (*GetByName)(const char* Name);
	UInt32		 (*GetReturnType)(CommandInfo* Info);
	UInt32		 (*GetRequiredOBSEVersion)(CommandInfo* Info);
	PluginInfo*  (*GetParentPlugin)(CommandInfo* Info);
};

enum
{
	kInterface_Serialization = 0,
	kInterface_Console,

	// Added v0002
	kInterface_Messaging,
	kInterface_CommandTable,

	// Added v0004
	kInterface_StringVar,
	kInterface_ArrayVar,
	kInterface_Script,

	// Added v0005 - version bumped to 3
	kInterface_Data,
	// Added v0006
	kInterface_EventManager,
	kInterface_Logging,

	kInterface_Max
};

struct NVSEMessagingInterface
{
	struct Message {
		const char	* sender;
		UInt32		type;
		UInt32		dataLen;
		void		* data;
	};

	typedef void (* EventCallback)(Message* msg);

	enum {
		kVersion = 4
	};

	// NVSE messages
	enum {
		kMessage_PostLoad,				// sent to registered plugins once all plugins have been loaded (no data)

		kMessage_ExitGame,				// exit to windows from main menu or in-game menu

		kMessage_ExitToMainMenu,		// exit to main menu from in-game menu

		kMessage_LoadGame,				// Dispatched immediately before plugin serialization callbacks invoked, after savegame has been read by Fallout
										// dataLen: length of file path, data: char* file path of .fos savegame file
										// Receipt of this message does not *guarantee* the serialization callback will be invoked
										// as there may be no .nvse file associated with the savegame

		kMessage_SaveGame,				// as above
	
		kMessage_ScriptEditorPrecompile,// EDITOR: Dispatched when the user attempts to save a script in the script editor.
										// NVSE first does its pre-compile checks; if these pass the message is dispatched before
										// the vanilla compiler does its own checks. 
										// data: ScriptBuffer* to the buffer representing the script under compilation
		
		kMessage_PreLoadGame,			// dispatched immediately before savegame is read by Fallout
										// dataLen: length of file path, data: char* file path of .fos savegame file

		kMessage_ExitGame_Console,		// exit game using 'qqq' console command

		kMessage_PostLoadGame,			//dispatched after an attempt to load a saved game has finished (the game's LoadGame() routine
										//has returned). You will probably want to handle this event if your plugin uses a Preload callback
										//as there is a chance that after that callback is invoked the game will encounter an error
										//while loading the saved game (eg. corrupted save) which may require you to reset some of your
										//plugin state.
										//data: bool, true if game successfully loaded, false otherwise */

		kMessage_PostPostLoad,			// sent right after kMessage_PostLoad to facilitate the correct dispatching/registering of messages/listeners
										// plugins may register as listeners during the first callback while deferring dispatches until the next
		kMessage_RuntimeScriptError,	// dispatched when an NVSE script error is encountered during runtime/
										// data: char* errorMessageText
// added for kVersion = 2
		kMessage_DeleteGame,			// sent right before deleting the .nvse cosave and the .fos save.
										// dataLen: length of file path, data: char* file path of .fos savegame file
		kMessage_RenameGame,			// sent right before renaming the .nvse cosave and the .fos save.
										// dataLen: length of old file path, data: char* old file path of .fos savegame file
										// you are expected to save the data and wait for kMessage_RenameNewGame
		kMessage_RenameNewGame,			// sent right after kMessage_RenameGame.
										// dataLen: length of new file path, data: char* new file path of .fos savegame file
		kMessage_NewGame,				// sent right before iterating through plugins newGame.
										// dataLen: 0, data: NULL
// added for kVersion == 3
		kMessage_DeleteGameName,		// version of the messages sent with a save file name instead of a save file path.
		kMessage_RenameGameName,
		kMessage_RenameNewGameName,

// added for kVersion == 4 (xNVSE)
		kMessage_DeferredInit,
		kMessage_ClearScriptDataCache,
		kMessage_MainGameLoop,			// called each game loop
		kMessage_ScriptCompile,   // EDITOR: called after successful script compilation in GECK. data: pointer to Script
								// RUNTIME: also gets called after successful script compilation at runtime via functions.
		kMessage_EventListDestroyed, // called before a script event list is destroyed, dataLen: 4, data: ScriptEventList* ptr
		kMessage_PostQueryPlugins // called after all plugins have been queried
	};

	UInt32	version;
	bool	(* RegisterListener)(PluginHandle listener, const char* sender, EventCallback handler);
	bool	(* Dispatch)(PluginHandle sender, UInt32 messageType, void * data, UInt32 dataLen, const char* receiver);
};