#pragma once

extern bool(__cdecl* LoadForm)(TESForm*, UInt32);
bool __cdecl LoadFormHook(TESForm* Form, UInt32 ModEntry);