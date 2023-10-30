// Basic Image adjutsments for Oblivion Reloaded

float4 TESR_ImageAdjustData;
float4 TESR_DarkAdjustColor;
float4 TESR_LightAdjustColor;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
 
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

 
float4 ImageAdjust(VSOUT IN) : COLOR0
{
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	float2 io = float2(1, 0);

	float4 darks = smoothstep(1, 0, color) * color * TESR_DarkAdjustColor;
	float4 lights = smoothstep(0, 1, color) * color * TESR_LightAdjustColor;

	float4 newColor = darks + lights;
	
	// saturation
	newColor = lerp(luma(newColor).rrrr, newColor, TESR_ImageAdjustData.z);

	// contrast
	newColor = (newColor - float(0.5).rrrr) * TESR_ImageAdjustData.y + float(0.5).rrrr;

	// brightness
	newColor += (TESR_ImageAdjustData.x - 1);

	color = lerp(color, newColor, TESR_ImageAdjustData.w); // strength of the effect

    // if (IN.UVCoord.x > 0.8 && IN.UVCoord.x < 0.95){
    //     if (IN.UVCoord.y > 0.15 && IN.UVCoord.y < 0.2) return TESR_ImageAdjustData;
    //     if (IN.UVCoord.y > 0.2 && IN.UVCoord.y < 0.25) return TESR_DarkAdjustColor;
    //     if (IN.UVCoord.y > 0.25 && IN.UVCoord.y < 0.3) return TESR_LightAdjustColor;
    //     if (IN.UVCoord.y > 0.3 && IN.UVCoord.y < 0.4) return darks;
    //     if (IN.UVCoord.y > 0.4 && IN.UVCoord.y < 0.5) return lights;
    //     if (IN.UVCoord.y > 0.5 && IN.UVCoord.y < 0.6) return darks + lights;
	// }

	return float4(color.rgb, 1);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ImageAdjust();
	}
}
