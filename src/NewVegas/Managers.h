#pragma once

class CommandManager;	extern CommandManager*		TheCommandManager;
class SettingManager;	extern SettingManager*		TheSettingManager;
class ShaderManager;	extern ShaderManager*		TheShaderManager;
class TextureManager;	extern TextureManager*		TheTextureManager;
class RenderManager;	extern RenderManager*		TheRenderManager;
class FrameRateManager; extern FrameRateManager*	TheFrameRateManager;
class GameMenuManager;	extern GameMenuManager*		TheGameMenuManager;
class GameEventManager; extern GameEventManager*	TheGameEventManager;
class ShadowManager;	extern ShadowManager*		TheShadowManager;
class CameraManager;	extern CameraManager*		TheCameraManager;
class BinkManager;		extern BinkManager*			TheBinkManager;

class Main;					extern Main*					Global;
class TES;					extern TES*						Tes;
class PlayerCharacter;		extern PlayerCharacter*			Player;
class SceneGraph;			extern SceneGraph*				WorldSceneGraph;
class MainDataHandler;		extern MainDataHandler*			DataHandler;
class MenuInterfaceManager; extern MenuInterfaceManager*	InterfaceManager;
class QueuedModelLoader;	extern QueuedModelLoader*		ModelLoader;
class ShadowSceneNode;		extern ShadowSceneNode*			SceneNode;

#include "../Core/RenderManager.h"
#include "../Core/SettingManager.h"
#include "../Core/CommandManager.h"
#include "../Core/TextureRecord.h"
#include "../Core/ShaderManager.h"
#include "../Core/TextureManager.h"
#include "../Core/FrameRateManager.h"
#include "../Core/GameEventManager.h"
#include "../Core/GameMenuManager.h"
#include "../Core/RenderPass.h"
#include "../Core/ShadowManager.h"
#include "../Core/CameraManager.h"
#include "../Core/BinkManager.h"

void InitializeManagers();