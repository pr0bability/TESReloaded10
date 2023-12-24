//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D ScreenSpace : register(s0);  // bloom buffer
sampler2D DestBlend : register(s1);    // base non tonemapped image

float4 HDRParam : register(c1);
float3 BlurScale : register(c2);
float4 Cinematic : register(c19); // x:saturation, y:avgluma, z:brightness, w: contrast
float4 Tint : register(c20);  // weather Cinematic IS tint?
float4 Fade : register(c22);  // Night eye color
float4 TESR_DebugVar : register(c24);
float4 TESR_HDRBloomData : register(c25); //
float4 TESR_SunAmount : register(c26);
float4 TESR_HDRData : register(c27); //
float4 TESR_LotteData : register(c28); //
float4 TESR_ToneMapping : register(c29); //

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   HDRParam     const_1       1
//   BlurScale    const_2       1
//   Cinematic    const_19      1
//   Tint         const_20      1
//   Fade         const_22      1
//   ScreenSpace         texture_0       1
//   DestBlend    texture_1       1
//

#include "Includes/Helpers.hlsl"
#include "Includes/Tonemapping.hlsl"

// Structures:

struct VS_INPUT {
    float2 ScreenOffset : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    
    // scale bloom while maintaining color
    float4 bloom = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    //float maxBloom = max(bloom.r, max(bloom.g, bloom.b));
    // float4 ratio = bloom / maxBloom;
    // bloom.rgb = TESR_HDRBloomData.x * pows(max(maxBloom, 0), TESR_HDRBloomData.y) * ratio;
    bloom.rgb = TESR_HDRBloomData.x * pows(max(bloom.rgb, 0.0), TESR_HDRBloomData.y);

    float gammaCorrection = max(1.0, TESR_ToneMapping.w);
    float4 hdrImage = tex2D(DestBlend, IN.texcoord_1.xy);
    
    float3 final = pows(hdrImage.rgb, gammaCorrection); // linearize

    float screenluma = luma(final);
    final = lerp(screenluma.xxx, final, Cinematic.x * TESR_HDRData.z); // saturation
    final = lerp(final.rgb, Tint.rgb * luma(final.rgb), saturate(Tint.a * TESR_ToneMapping.z)); // apply tint
    
    float q0 = 1.0 / max(bloom.w, HDRParam.x); // HDRParam.x, brights cutoff?
    final = ((q0 * HDRParam.x) * final) + max(bloom.rgb * (q0 * 0.5), 0.0); // blend image and bloom
    
    final = lerp(final.rgb, final.rgb * Cinematic.z, TESR_HDRData.z); // apply brightness from Cinematic
    final = tonemap(final.rgb * TESR_HDRData.y); // exposure & tonemap using provided tonemapper
    
    final = pows(final.rgb, 1.0/max(1.0, TESR_HDRData.w)); // delinearise
    
    final = lerp(final.rgb, Cinematic.z * (final.rgb - Cinematic.y) + Cinematic.y, TESR_HDRData.z * TESR_ToneMapping.y); // apply contrast from Cinematic, scaled by modifier
    final = lerp(final.rgb, Fade.rgb, Fade.a); // apply night eye and fade, gamma-space but vanilla-accurate

    OUT.color_0.rgb = final;
    OUT.color_0.a = BlurScale.z;

    return OUT;
};

// approximately 18 instruction slots used (2 texture, 16 arithmetic)
