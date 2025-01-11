#pragma once

typedef unsigned char		UInt8;
typedef unsigned short		UInt16;
typedef unsigned long		UInt32;
typedef unsigned long long	UInt64;
typedef signed char			SInt8;
typedef signed short		SInt16;
typedef signed long			SInt32;
typedef signed long long	SInt64;
typedef float				Float32;
typedef double				Float64;

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8, T9 Arg9) { class T {}; union { UInt32 x; UInt32(T::* m)(T1, T2, T3, T4, T5, T6, T7, T8, T9); } u = { Method }; return ((T*)Instance->*u.m)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9); }
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8) { class T {}; union { UInt32 x; UInt32(T::* m)(T1, T2, T3, T4, T5, T6, T7, T8); } u = { Method }; return ((T*)Instance->*u.m)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8); }
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7) { class T {}; union { UInt32 x; UInt32(T::* m)(T1, T2, T3, T4, T5, T6, T7); } u = { Method }; return ((T*)Instance->*u.m)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7); }
template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6) { class T {}; union { UInt32 x; UInt32(T::* m)(T1, T2, T3, T4, T5, T6); } u = { Method }; return ((T*)Instance->*u.m)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6); }
template <typename T1, typename T2, typename T3, typename T4, typename T5> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5) { class T {}; union { UInt32 x; UInt32(T::* m)(T1, T2, T3, T4, T5); } u = { Method }; return ((T*)Instance->*u.m)(Arg1, Arg2, Arg3, Arg4, Arg5); }
template <typename T1, typename T2, typename T3, typename T4> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4) { class T {}; union { UInt32 x; UInt32(T::* m)(T1, T2, T3, T4); } u = { Method }; return ((T*)Instance->*u.m)(Arg1, Arg2, Arg3, Arg4); }
template <typename T1, typename T2, typename T3> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1, T2 Arg2, T3 Arg3) { class T {}; union { UInt32 x; UInt32(T::* m)(T1, T2, T3); } u = { Method }; return ((T*)Instance->*u.m)(Arg1, Arg2, Arg3); }
template <typename T1, typename T2> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1, T2 Arg2) { class T {}; union { UInt32 x; UInt32(T::* m)(T1, T2); } u = { Method }; return ((T*)Instance->*u.m)(Arg1, Arg2); }
template <typename T1> inline UInt32 ThisCall(UInt32 Method, void* Instance, T1 Arg1) { class T {}; union { UInt32 x; UInt32(T::* m)(T1); } u = { Method }; return ((T*)Instance->*u.m)(Arg1); }
inline UInt32 ThisCall(UInt32 Method, void* Instance) { class T {}; union { UInt32 x; UInt32(T::* m)(); } u = { Method }; return ((T*)Instance->*u.m)(); }

inline float ThisCallF(UInt32 Method, void* Instance) { class T {}; union { UInt32 x; float(T::* m)(); } u = { Method }; return ((T*)Instance->*u.m)(); }

inline double ThisCallD(UInt32 Method, void* Instance) { class T {}; union { UInt32 x; float(T::* m)(); } u = { Method }; return ((T*)Instance->*u.m)(); }

template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret StdCall(UInt32 _addr, Args ...args)
{
	return ((T_Ret(__stdcall*)(Args...))_addr)(std::forward<Args>(args)...);
}

template <typename T_Ret = UInt32, typename ...Args>
__forceinline T_Ret ThisStdCall(UInt32 _addr, const void* _this, Args ...args)
{
	return ((T_Ret(__thiscall*)(const void*, Args...))_addr)(_this, std::forward<Args>(args)...);
}

template <typename T_Ret = void, typename ...Args>
__forceinline T_Ret CdeclCall(UInt32 _addr, Args ...args)
{
	return ((T_Ret(__cdecl*)(Args...))_addr)(std::forward<Args>(args)...);
}

/**
 *	A bitfield.
 */
template <typename T>
class Bitfield
{
public:
	Bitfield() { field = 0; }
	~Bitfield() { }

	void	Clear(void) { field = 0; }						//!< Clears all bits
	void	RawSet(UInt32 data) { field = data; }					//!< Modifies all bits

	void	Set(UInt32 data) { field |= data; }					//!< Sets individual bits
	void	Clear(UInt32 data) { field &= ~data; }					//!< Clears individual bits
	void	Unset(UInt32 data) { Clear(data); }					//!< Clears individual bits
	void	Mask(UInt32 data) { field &= data; }					//!< Masks individual bits
	void	Toggle(UInt32 data) { field ^= data; }					//!< Toggles individual bits
	void	SetBit(UInt32 data, bool state)
	{
		if (state) Set(data); else Clear(data);
	}

	void	SetField(T data, T mask, T pos) {
		field = (field & ~mask) | (data << pos);
	}

	T		GetField(T mask, T pos) const {
		return (field & mask) >> pos;
	}

	T		Get(void) const { return field; }					//!< Gets all bits
	T		GetBit(UInt32 data) const { return field & data; }			//!< Gets individual bits
	T		Extract(UInt32 bit) const { return (field >> bit) & 1; }		//!< Extracts a bit
	T		ExtractField(UInt32 shift, UInt32 length)					//!< Extracts a series of bits
	{
		return (field >> shift) & (0xFFFFFFFF >> (32 - length));
	}

	bool	IsSet(UInt32 data) const { return ((field & data) == data) ? true : false; }	//!< Are all these bits set?
	bool	IsUnSet(UInt32 data) const { return (field & data) ? false : true; }			//!< Are all these bits clear?
	bool	IsClear(UInt32 data) const { return IsUnSet(data); }							//!< Are all these bits clear?

	T		field;	//!< bitfield data
};

typedef Bitfield <UInt8>	Bitfield8;		//!< An 8-bit bitfield
typedef Bitfield <UInt16>	Bitfield16;		//!< A 16-bit bitfield
typedef Bitfield <UInt32>	Bitfield32;		//!< A 32-bit bitfield
