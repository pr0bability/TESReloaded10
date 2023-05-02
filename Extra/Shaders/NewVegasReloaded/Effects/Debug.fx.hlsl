float4 TESR_SunColor;
float4 TESR_FogColor;
float4 TESR_WaterShallowColor; // Shallow color used by the game for water
float4 TESR_WaterDeepColor; // Deep color used by the game for water
float4 TESR_HorizonColor;
float4 TESR_SkyColor;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_SourceBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };
sampler2D TESR_AvgLumaBuffer : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };


struct VSOUT {
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN {
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN) {
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"
#include "Includes/Normals.hlsl"

float4 displayBuffer(float4 color, float2 uv, float2 bufferPosition, float2 bufferSize, sampler2D buffer){
	float2 lowerCorner = bufferPosition + bufferSize;
	if ((uv.x < bufferPosition.x || uv.y < bufferPosition.y) || (uv.x > lowerCorner.x || uv.y > lowerCorner.y )) return color;
	return tex2D(buffer, float2(invlerp(bufferPosition, lowerCorner, uv)));
}

float4 DebugShader( VSOUT IN) : COLOR0 {
    
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);

	color = displayBuffer(color, IN.UVCoord, float2(0.1, 0.05), float2(0.15, 0.15), TESR_NormalsBuffer);
	color = displayBuffer(color, IN.UVCoord, float2(0.3, 0.05), float2(0.15, 0.15), TESR_DepthBuffer);

    if (IN.UVCoord.x > 0.9 && IN.UVCoord.x < 0.95){
        if (IN.UVCoord.y > 0.2 && IN.UVCoord.y < 0.25) return TESR_SunColor;
        if (IN.UVCoord.y > 0.25 && IN.UVCoord.y < 0.3) return TESR_FogColor;
        if (IN.UVCoord.y > 0.3 && IN.UVCoord.y < 0.35) return TESR_HorizonColor;
        if (IN.UVCoord.y > 0.35 && IN.UVCoord.y < 0.4) return TESR_SkyColor;
        if (IN.UVCoord.y > 0.4 && IN.UVCoord.y < 0.45) return TESR_WaterShallowColor;
        if (IN.UVCoord.y > 0.45 && IN.UVCoord.y < 0.5) return TESR_WaterDeepColor;
        if (IN.UVCoord.y > 0.55 && IN.UVCoord.y < 0.6) return tex2D(TESR_AvgLumaBuffer, float2(0.5, 0.5));
    }

    return color;
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 DebugShader();
	}
}