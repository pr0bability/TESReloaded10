#pragma once
#pragma warning (disable: 4244) //disable warning for possible loss of data in implicit cast between int, float and double

#define DETOURS_INTERNAL
#define assert(a) static_assert(a, "Assertion failed")
#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <d3d9.h>
#include <d3dx9math.h>
#include <dinput.h>
#include <dsound.h>
#include "../../lib/Detours/detours.h"
#include "../../lib/Nvidia/nvapi.h"
#include "../../src/base/Logger.h"
#include "../../src/base/Types.h"
#include "../../src/base/SafeWrite.h"
#include "../../src/base/PluginVersion.h"
#include "Plugin.h"
#include "obse/GameNi.h"
#include "obse/GameHavok.h"
#include "obse/Game.h"
#include "Defines.h"
#include "Base.h"
#include "Managers.h"
#include "../../src/core/Hooks/GameCommon.h"
#include "../../src/core/Hooks/FormsCommon.h"
#include "../../src/core/Hooks/SleepingCommon.h"
#include "../../src/core/Hooks/Script.h"
#include "../../src/Oblivion/Hooks/Settings.h"
#include "../../src/Oblivion/Hooks/Game.h"
#include "../../src/Oblivion/Hooks/ShaderIO.h"
#include "../../src/Oblivion/Hooks/Render.h"
#include "../../src/Oblivion/Hooks/Forms.h"
#include "../../src/Oblivion/Hooks/Events.h"
#include "../../src/Oblivion/Hooks/Animation.h"
#include "../../src/Oblivion/Hooks/Shadows.h"
#include "../../src/Oblivion/Hooks/FrameRate.h"
#include "../../src/Oblivion/Hooks/Texture.h"
#include "../../src/Oblivion/Hooks/Occlusion.h"
#include "../../src/Oblivion/Hooks/Memory.h"
#include "../../src/Oblivion/Hooks/Grass.h"
#include "../../src/Oblivion/Hooks/Camera.h"
#include "../../src/Oblivion/Hooks/Equipment.h"
#include "../../src/Oblivion/Hooks/Mounted.h"
#include "../../src/Oblivion/Hooks/Sleeping.h"
#include "../../src/Oblivion/Hooks/Dodge.h"
#include "../../src/Oblivion/Hooks/FlyCam.h"
