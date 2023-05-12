//Contrast Adaptive Sharpening Enhanced Shader for TESReloaded
//----------------------------------------------------
// Features:
// - Exponential Depth falloff
// 		Simple depth falloff using a max depth var to calculate a 0-1 fallOffFactor that's then lerped with the final sharpened color
//		Multipied by 1000 so you have to use lower input params making 0 apply no effect and 1 equal 1000
// - Intensity
//		It does what it says, intensity param that's multiplied by the contrast to increase overall sharpening effect
//		Multipied by two so the effect is more noticable.
// - Adjustable Kernel size
//		You just change the kernel size to increase sampling radius of the custom Manhattan sampling thingy
// - Debug modes
//		Used for dev mode so you can see each process of the sharpening
//----------------------------------------------------

#define debugMode 0 // 1 - FalloffFactor 2 - Contrast 3 - Average(blur)
#define falloffExponent 4 // Set the falloff exponent to control the curve steepness for depth falloff
#define kernelSize 3 // Set kernel size for averaging, to find total samples multiply the number by it's self

float4 TESR_SharpeningData; // X - Intensity Y - Clamp  Z - Offset(depth falloff) 
float4 TESR_ReciprocalResolution;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"

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
    VSOUT OUT = (VSOUT) 0.0f;
    OUT.vertPos = IN.vertPos;
    OUT.UVCoord = IN.UVCoord;
    return (OUT);
}

float4 ContrastAdaptiveSharpening(VSOUT IN) : COLOR
{
    // CAS Params
    float intensity = TESR_SharpeningData.x * 2; // Multiplied by two so it's more noticeable change
    float maxDepth = TESR_SharpeningData.z * 1000; // Multiplied by 1000 so the default values start at zero to one

    // Sample the rendered and reconstruct position from depth for depth falloff
    float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
    float depth = readDepth(IN.UVCoord);

    // Calculate the falloff factor using pow
    float falloffFactor = pow(saturate((maxDepth - depth) / maxDepth), falloffExponent);

    // Calculate the average color of the surrounding pixels
    float3 average = 0;
    float sum = 0;
    for (int x = -kernelSize / 2; x <= kernelSize / 2; x++)
    {
        for (int y = -kernelSize / 2; y <= kernelSize / 2; y++)
        {
            float weight = 1.0 - (abs(x) + abs(y)) / kernelSize; // simulate weights with Manhattan distance
            average += tex2D(TESR_RenderedBuffer, IN.UVCoord + float2(x, y) * TESR_ReciprocalResolution.xy) * weight;
            sum += weight;
        }
    }
    average /= sum;
    
    // Calculate the contrast between the current pixel and the average color
    float contrast = dot(abs(color.rgb - average), luma(color.rgb) / (luma(color.rgb) + 0.05));
    
    // Calculate the sharpening and lerp with fallofffactor
    color.rgb = lerp(color.rgb, color.rgb + (color.rgb - average) * saturate(contrast * intensity), falloffFactor);
    
    // Debug modes
    #if debugMode == 1
        return falloffFactor;
    #elif debugMode == 2
        return contrast * intensity;
    #elif debugMode == 3
        return float4(average, 1.0);   
    #endif
	
    return color;
}

technique
{
    pass
    {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 ContrastAdaptiveSharpening();
    }
}
