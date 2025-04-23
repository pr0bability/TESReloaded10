#include "Forms.h"

__declspec(naked) void SetRegionEditorNameHook() {
	
	__asm {
		push	ecx
		call	SetRegionEditorName
		add		esp, 8
		xor		esi, esi
		jmp		Jumpers::SetRegionEditorName::Return
	}

}



__declspec(naked) void SetWeatherEditorNameHook() {

	__asm {
		push	ecx
		call	SetWeatherEditorName
		add		esp, 8
		jmp		Jumpers::SetWeatherEditorName::Return
	}

}