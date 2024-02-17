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
	
	// saturation
	color = max(0.0,lerp(luma(color).rrrr, color, TESR_ImageAdjustData.z));
	
	color.rgb = pows(color.rgb, 2.2); // linearise
	float2 io = float2(1, 0);

	float3 darks = smoothstep(1, 0, color.rgb) * color.rgb * pows(TESR_DarkAdjustColor.rgb,2.2); // linearise
	float3 lights = smoothstep(0, 1, color.rgb) * color.rgb * pows(TESR_LightAdjustColor.rgb,2.2); // linearise

	float3 newColor = darks.rgb + lights.rgb;

	// brightness
	newColor *= max(0.0,TESR_ImageAdjustData.x);

	color.rgb = lerp(color.rgb, newColor, TESR_ImageAdjustData.w); // strength of the effect

	color.rgb = pows(color.rgb, 1.0/2.2); // delinearise

	// contrast
	//0.5^2.2=0.21764
	newColor = max(0.0,(color.rgb - float(0.5).rrr) * TESR_ImageAdjustData.y + float(0.5).rrr);

	color.rgb = lerp(color.rgb, newColor, TESR_ImageAdjustData.w); // strength of the effect
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
