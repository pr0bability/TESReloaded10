// Image space shadows shader for Oblivion Reloaded

float4x4 TESR_WorldTransform;
float4 TESR_ShadowData;
float4 TESR_ShadowFade;
float4 TESR_ReciprocalResolution;
//sampler_state removed to avoid a artifact. TODO investigate
sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_SourceBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_PointShadowBuffer : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"
static const float MAXDISTANCE = TESR_ShadowFade.w;

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

#include "Includes/BlurDepth.hlsl"

float4 Shadow( VSOUT IN ) : COLOR0 {
	float4 color = tex2D(TESR_PointShadowBuffer, IN.UVCoord);
	return color;
}

float4 CombineShadow( VSOUT IN ) : COLOR0 {

	// combine Shadow pass and source using an overlay mode + alpha blending
	float4 color = tex2D(TESR_SourceBuffer, IN.UVCoord);
	float depth = readDepth(IN.UVCoord);
	float3 eyeDir = toWorld(IN.UVCoord);
	float uniformDepth = length(depth * eyeDir);

	//multiply by 2 to only use the lower half of values to impact darkness
	float Shadow = saturate(tex2D(TESR_RenderedBuffer, IN.UVCoord).r);

	Shadow = lerp(Shadow, 1.0, invlerps(300, MAXDISTANCE, uniformDepth)); // fade shadows with distance
    float shadowPower = TESR_ShadowData.y * TESR_ShadowFade.y;
	
	Shadow = saturate(lerp(1, Shadow, shadowPower)); // shadow darkness (shadowFade.y is set to 0 when shadows are disabled)

	float4 finalColor = saturate(Shadow * pow(color, 1 - shadowPower * 0.4)); // boost the gamma of the base image
	finalColor.rgb = lerp(luma(finalColor).xxx, finalColor.rgb, 1 + Shadow * shadowPower * 0.5); // boost saturation in darker areas

	float lumaDiff = invlerps(luma(finalColor.rgb), 1.0f, luma(color.rgb));
	return lerp(finalColor, color, lumaDiff); // bring back some of the original color based on luma (brightest lights will come through)
}

technique {
	
	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Shadow();
	}

	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 DepthBlur(TESR_RenderedBuffer, OffsetMaskH, 1, 3500, MAXDISTANCE);
	}
	
	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 DepthBlur(TESR_RenderedBuffer, OffsetMaskV, 1, 3500, MAXDISTANCE);
	}
	
	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 CombineShadow();
	}
	
}
