#pragma once

void SafeWrite8(UInt32 addr, UInt32 data);
void SafeWrite16(UInt32 addr, UInt32 data);
void SafeWrite32(UInt32 addr, UInt32 data);
void SafeWriteJump(UInt32 jumpSrc, UInt32 jumpTgt);
void SafeWriteCall(UInt32 jumpSrc, UInt32 jumpTgt);
void SafeWriteNop(UInt32 jumpSrc, UInt8 num);

template <typename C, typename Ret, typename... Args>
void __fastcall ReplaceVTableEntry(void** apVTable, uint32_t auiPosition, Ret(C::* const target)(Args...) const) {
	union {
		Ret(C::* tgt)(Args...) const;
		SIZE_T funcPtr;
	} conversion;
	conversion.tgt = target;

	apVTable[auiPosition] = (void*)conversion.funcPtr;
}

template <typename C, typename Ret, typename... Args>
void __fastcall ReplaceVTableEntry(void** apVTable, uint32_t auiPosition, Ret(C::* const target)(Args...)) {
	union {
		Ret(C::* tgt)(Args...);
		SIZE_T funcPtr;
	} conversion;
	conversion.tgt = target;

	apVTable[auiPosition] = (void*)conversion.funcPtr;
}
