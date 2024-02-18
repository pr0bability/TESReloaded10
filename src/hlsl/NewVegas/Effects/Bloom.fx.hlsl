// Bloom shader

float4 TESR_ReciprocalResolution;
float4 TESR_SunColor;
float4 TESR_SunAmount;
float4 TESR_LensData; // x: lens strength, y: luma threshold
float4 TESR_DebugVar; // used for the luma threshold used for bloom

sampler2D TESR_BloomBuffer : register(s0) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = ANISOTROPIC; MINFILTER = ANISOTROPIC; MIPFILTER = ANISOTROPIC; };
sampler2D TESR_SourceBuffer : register(s1) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = ANISOTROPIC; MINFILTER = ANISOTROPIC; MIPFILTER = ANISOTROPIC; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = ANISOTROPIC; MINFILTER = ANISOTROPIC; MIPFILTER = ANISOTROPIC; };

static const float scale = 0.5;

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

#include "Includes/Helpers.hlsl"
#include "Includes/Blur.hlsl"


// downsample/upsample a part of the screen given by the scaleFactor
float4 ScaleUp(VSOUT IN, uniform sampler2D buffer, uniform float scaleFactor) : COLOR0
{
	float2 uv = IN.UVCoord * scaleFactor;// scale the uv by wanted scale
	if ((uv.x > 1 || uv.y > 1)) return float4(0, 0, 0, 1); // discard uvs outside of [0-1]range

	float4 color = tex2D(TESR_BloomBuffer, uv + float2(-0.5, -0.5) * TESR_ReciprocalResolution.xy / 2);
	color += tex2D(TESR_BloomBuffer, uv + float2(-0.5, 0.5) * TESR_ReciprocalResolution.xy / 2);
	color += tex2D(TESR_BloomBuffer, uv + float2(0.5, -0.5) * TESR_ReciprocalResolution.xy / 2);
	color += tex2D(TESR_BloomBuffer, uv + float2(0.5, 0.5) * TESR_ReciprocalResolution.xy / 2);

	// return float4(uv, 0, 1);
	return float4(color.rgb/4, 1);
}

float4 Bloom(VSOUT IN ):COLOR0{

    float2 uv = IN.UVCoord;
	// clip((uv <= scale) - 1);
	// uv /= scale;

	// quick average lum with 4 samples at corner pixels
	// float4 color = linearize(tex2D(TESR_SourceBuffer, uv));
	float4 color = linearize(tex2D(TESR_SourceBuffer, uv + float2(-1, -1) * TESR_ReciprocalResolution.xy / 2));
	color += linearize(tex2D(TESR_SourceBuffer, uv + float2(-1, 1) * TESR_ReciprocalResolution.xy / 2));
	color += linearize(tex2D(TESR_SourceBuffer, uv + float2(1, -1) * TESR_ReciprocalResolution.xy / 2));
	color += linearize(tex2D(TESR_SourceBuffer, uv + float2(1, 1) * TESR_ReciprocalResolution.xy / 2));
	color /= 4;

	float threshold = TESR_DebugVar.y;
	float brightness = max(0.000001, luma(color));
	float bloomScale = TESR_DebugVar.x; 

	float bloom = bloomScale * sqr(max(0.0, brightness - threshold)) / brightness;

	return float4(saturate(bloom) * color.rgb, 1);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Bloom();
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer, 2);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer, 2);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Blur(TESR_BloomBuffer, OffsetMaskH, 2, 0.5);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Blur(TESR_BloomBuffer, OffsetMaskV, 2, 0.5);
	}

	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Blur(TESR_BloomBuffer, OffsetMaskH, 1, 0.5);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Blur(TESR_BloomBuffer, OffsetMaskV, 1, 0.5);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer, 0.5);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer, 0.5);
	}
}