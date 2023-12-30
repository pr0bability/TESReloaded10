float4 TESR_LotteData;
float4 TESR_ToneMapping;
float4 TESR_HDRData;

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
#define CMAX 1.6e+6f
#define EPS 1e-6f
// Code:
float4 AMDLottes(VSOUT IN) : COLOR0
{
    float contrast = TESR_LotteData.x;
    float b = TESR_LotteData.z;
    float c = TESR_LotteData.y;
    float shoulder = TESR_LotteData.w;

	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
    color.rgb = pows(color.rgb, max(1.0, TESR_ToneMapping.w)); // linearize color

    color.rgb *= TESR_HDRData.y;
    // Tonemap
    float3 peak = max(color.r, max(color.g, color.b));
    peak = min(CMAX, max(EPS, peak));

    float3 ratio = min(CMAX, color.rgb / peak);

    float lum = dot(color.rgb, float3(0.5, 0.4, 0.33));
    float gray = min(color.r, min(color.g, color.b));
	gray = max(0.0, gray);
    peak += min(peak, gray);
    peak *= 0.5;
    peak *= 1.0 + 1.666 * max(0, (peak - lum) / peak);

    float3 z = pows(peak, contrast);
    peak = z / (pows(z, b) * shoulder + c);

    float saturation = contrast; // crosstalk saturation
    float crossSaturation = contrast * 64.0; // crosstalk saturation
    // wrap crosstalk in transform
    ratio = pows(abs(ratio + 0.11) * 0.90909, saturation / crossSaturation);
    ratio = lerp(ratio, 1.0, pows(peak, float3(4.0, 1.5, 1.5) * 1.0/peak));
    ratio = pows(min(1.0, ratio), crossSaturation);

    color.rgb = peak * ratio;

    return float4(pows(color.rgb, 1.0/max(1.0, TESR_HDRData.w)),color.a);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 AMDLottes();
	}
}
