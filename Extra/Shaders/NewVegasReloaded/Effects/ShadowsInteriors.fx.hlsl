// Image space shadows shader for Oblivion Reloaded

float4x4 TESR_WorldTransform;
float4 TESR_ShadowData;
float4 TESR_ShadowFade;
float4 TESR_ReciprocalResolution;

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


float4 CombineShadow( VSOUT IN ) : COLOR0 {
	// combine Shadow pass and source using an overlay mode + alpha blending
	float4 color = tex2D(TESR_SourceBuffer, IN.UVCoord);
    color.rgb = pows(color.rgb, 2.2); // linearise
	float depth = readDepth(IN.UVCoord);
	float3 eyeDir = toWorld(IN.UVCoord);
	float uniformDepth = length(depth * eyeDir);

	float Shadow = saturate(tex2D(TESR_RenderedBuffer, IN.UVCoord).r);
	Shadow = lerp(Shadow, 1.0, invlerps(300, MAXDISTANCE, uniformDepth)); // fade shadows with distance
    float darkness = saturate(1 - TESR_ShadowData.y * TESR_ShadowFade.y); // shadowFade.y is set to 0 when shadows are disabled
	
	Shadow = saturate(lerp(darkness, 1, Shadow)); // shadow darkness to apply in the scene - 0 is full shadow 1 is full light
	float shadowPower = 1 - Shadow; // 1 is full shadow, useful for scaling effect

	// apply shadows to darkest values (< 1)
	float4 finalColor = saturate(Shadow * (pow(saturate(color), 1 - shadowPower * 0.2))); // boost the gamma of the base image
	finalColor = lerp(luma(finalColor), finalColor, 1 + shadowPower * 0.3); // add some saturation back to the darker parts of the image

	// readd values above 1
	finalColor += max(color - 1, 0.0);
    finalColor.rgb = pows(finalColor.rgb, 1.0/2.2); // delinearise

	return float4(finalColor.rgb, 1);
}

technique {

	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 DepthBlur(TESR_PointShadowBuffer, OffsetMaskH, 1, 3500, MAXDISTANCE);
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
