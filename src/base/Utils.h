#pragma once

static inline std::string ltrim(const std::string s)
{
	return std::regex_replace(s, std::regex("^\\s+"), "");
}


static inline std::string rtrim(const std::string s)
{
	return std::regex_replace(s, std::regex("\\s+$"), "");
}


static inline std::string trim(const std::string s) {
	return ltrim(rtrim(s));
}

static inline D3DXVECTOR4 lerp(D3DXVECTOR4 a, D3DXVECTOR4 b, float t) {
	D3DXVECTOR4 result;
	result.x = std::lerp(a.x, b.x, t);
	result.y = std::lerp(a.y, b.y, t);
	result.z = std::lerp(a.z, b.z, t);
	result.w = std::lerp(a.w, b.w, t);

	return result;
}

inline static float clamp(float a, float b, float t) {
	return min(max(a, t), b);
}

inline static float invLerp(float a, float b, float t) {
	return(t - a) / (b - a);
}

inline static float step(float a, float b, float t) {
	return clamp(0, 1, invLerp(a, b, t));
}

inline static float smoothStep(float a, float b, float t) {
	float t2 = step(a, b, t);
	return t2 * t2 * (3.0 - 2.0 * t2);
}

// returns the sign of the int
template <typename T> inline static int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

// does a power operation which retains the sign
inline static  float pows(float a, float b) {
	return (pow(abs(a), b) * sgn(a));
}