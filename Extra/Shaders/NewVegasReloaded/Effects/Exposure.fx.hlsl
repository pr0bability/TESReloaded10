// Exposure shader for Oblivion/Skyrim Reloaded
#define debug 0

float4 TESR_ExposureData; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed
float4 TESR_FrameTime; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_AvgLumaBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#include "Includes/Helpers.hlsl"

static const float minBrightness = TESR_ExposureData.x;
static const float maxBrightness = TESR_ExposureData.y;

struct VSOUT
{
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}


float4 Exposure(VSOUT IN) : COLOR0
{
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	float averageLuma = tex2D(TESR_AvgLumaBuffer, float2(0.5, 0.5)).g;

	float negativeLumaDiff = invlerps(minBrightness, 0, averageLuma) * minBrightness;
	float additiveLumaDiff = invlerps(maxBrightness, 1, averageLuma) * (1 - maxBrightness) * 2;
	float lumaDiff = additiveLumaDiff - negativeLumaDiff;

#if debug
	if (IN.UVCoord.x > 0.7 && IN.UVCoord.x < 0.8 && IN.UVCoord.y>0.7 && IN.UVCoord.y<0.8) return averageLuma;
	if (IN.UVCoord.x > 0.7 && IN.UVCoord.x < 0.8 && IN.UVCoord.y>0.8 && IN.UVCoord.y<0.9) return float4(lumaDiff, -lumaDiff, 0, 1);
#endif

	color = pows(color, (1 + lumaDiff));
	return float4(color.rgb, 1);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 Exposure();
	}
}