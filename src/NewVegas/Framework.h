#pragma once
#pragma warning (disable: 4244) //disable warning for possible loss of data in implicit cast between int, float and double

#define DETOURS_INTERNAL
#define assert(a) static_assert(a, "Assertion failed")
#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <CommCtrl.h>
#include <string>
#include <regex>
#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <d3d9.h>
#include <d3dx9math.h>
#include <dinput.h>
#include <dsound.h>
#include "../lib/Detours/detours.h"
#include "../lib/Nvidia/nvapi.h"
#include "../lib/Bink/bink.h"
#include "../Base/Logger.h"
#include "../Base/Types.h"
#include "../Base/SafeWrite.h"
#include "../Base/PluginVersion.h"
#include "..//Base/Utils.h"
#include "Plugin.h"
#include "../NewVegas/nvse/GameNi.h"
#include "../NewVegas/nvse/GameHavok.h"
#include "../NewVegas/nvse/Game.h"
#include "Defines.h"
#include "Base.h"
#include "Managers.h"
#include "../Core/Hooks/GameCommon.h"
#include "../Core/Hooks/FormsCommon.h"
#include "../NewVegas/Hooks/Settings.h"
#include "../NewVegas/Hooks/Game.h"
#include "../NewVegas/Hooks/ShaderIO.h"
#include "../NewVegas/Hooks/Render.h"
#include "../NewVegas/Hooks/Forms.h"
#include "../NewVegas/Hooks/Shadows.h"
#include "../NewVegas/Hooks/FlyCam.h"
