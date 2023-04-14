// Exposure shader for Oblivion/Skyrim Reloaded

float4 TESR_DebugVar;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_AvgLumaBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#include "Includes/Helpers.hlsl"

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

	if (IN.UVCoord.x > 0.7 && IN.UVCoord.x < 0.8 && IN.UVCoord.y>0.7 && IN.UVCoord.y<0.8) return averageLuma;

	color = pow(color, TESR_DebugVar.w);
	return color;
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 Exposure();
	}
}