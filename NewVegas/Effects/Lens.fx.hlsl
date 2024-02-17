// Lens Shader For TESReloaded
//--------------------------------------------------

float4 TESR_ReciprocalResolution;
float4 TESR_CinemaSettings; //x: dirtlens opacity, y:grainAmount, z:chromatic aberration strength 
float4 TESR_SunColor;
float4 TESR_SunAmbient;
float4 TESR_SunAmount;
float4 TESR_LensData; // x: lens strength, y: luma threshold
float4 TESR_DebugVar; // used for the luma threshold used for bloom

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_SourceBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_LensSampler : register(s2) < string ResourceName = "Effects\dirtlens.png"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

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

float4 Lens(VSOUT IN) : COLOR0 
{
	float2 uv = IN.UVCoord;
    float4 color = tex2D(TESR_SourceBuffer, uv);
    float4 dirtColor = tex2D(TESR_LensSampler, uv);

    // Get the bloom mask to calculate areas where dirt lens will appear
	float4 bloom = tex2D(TESR_RenderedBuffer, IN.UVCoord);
    color = color + saturate(dirtColor.r * bloom) * TESR_LensData.x;

    return float4(color.rgb, 1);
}


float4 Bloom(VSOUT IN ):COLOR0{

    float2 uv = IN.UVCoord;
	clip((uv <= scale) - 1);
	uv /= scale;

	// quick average lum with 4 samples at corner pixels
	float4 color = tex2D(TESR_RenderedBuffer, uv);
	color = tex2D(TESR_RenderedBuffer, uv + float2(-1, -1) * TESR_ReciprocalResolution.xy);
	color += tex2D(TESR_RenderedBuffer, uv + float2(-1, 1) * TESR_ReciprocalResolution.xy);
	color += tex2D(TESR_RenderedBuffer, uv + float2(1, -1) * TESR_ReciprocalResolution.xy);
	color += tex2D(TESR_RenderedBuffer, uv + float2(1, 1) * TESR_ReciprocalResolution.xy);
	color /= 4;

	float threshold = TESR_LensData.y;
	float brightness = luma(color);
	float bloomScale = 0.1; 

	float bloom = bloomScale * sqr(max(0.0, brightness - threshold)) / brightness;

	return float4(bloom * color.rgb * 100, 1);
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
		PixelShader = compile ps_3_0 Scale(TESR_RenderedBuffer, 0.5);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Scale(TESR_RenderedBuffer, 0.5);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Blur(TESR_RenderedBuffer, OffsetMaskH, 2, 0.125);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Blur(TESR_RenderedBuffer, OffsetMaskV, 2, 0.125);
	}

	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Blur(TESR_RenderedBuffer, OffsetMaskH, 1, 0.125);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Blur(TESR_RenderedBuffer, OffsetMaskV, 1, 0.125);
	}

	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Scale(TESR_RenderedBuffer, 2);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Scale(TESR_RenderedBuffer, 2);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Scale(TESR_RenderedBuffer, 2);
	}

    pass
    {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Lens();
    }
}