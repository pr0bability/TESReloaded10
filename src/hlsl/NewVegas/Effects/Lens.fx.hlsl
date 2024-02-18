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
sampler2D TESR_BloomBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_LensSampler : register(s3) < string ResourceName = "Effects\dirtlens.png"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

//static const float scale = 0.5;

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

float4 Lens(VSOUT IN) : COLOR0 
{
	float2 uv = IN.UVCoord;
    float4 color = linearize(tex2D(TESR_SourceBuffer, uv));
    float4 dirtColor = tex2D(TESR_LensSampler, uv);

    // Get the bloom mask to calculate areas where dirt lens will appear
	float4 bloom = tex2D(TESR_BloomBuffer, IN.UVCoord);
    color = color + color * saturate(dirtColor.r * bloom) * TESR_LensData.x;

    return delinearize(float4(color.rgb, 1));
}

technique
{
    pass
    {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Lens();
    }
}