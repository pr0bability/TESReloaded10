//
//
//#define	ScreenSpace	Src0
// Parameters:

float4 HDRParam : register(c1);
float3 BlurScale : register(c2);
float4 Cinematic : register(c19); // x:saturation, y:avgluma, z:brightness, w: contrast
float4 Tint : register(c20);  // weather Cinematic IS tint ?
float4 Fade : register(c22);  // Night eye color
float4 TESR_DebugVar : register(c24);
float4 TESR_HDRBloomData : register(c25); //
//float4 TESR_SunAmount : register(c26);
float4 TESR_HDRData : register(c27); //
float4 TESR_LotteData : register(c28); //
float4 TESR_ToneMapping : register(c29); //
float4 TESR_ReciprocalResolution : register(c30); //
float4 TESR_BloomData : register(c31); //

sampler2D Src0 : register(s0);
sampler2D DestBlend : register(s1);    // base non tonemapped image
sampler2D TESR_BloomBuffer : register(s8) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = POINT; };

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
    float2 pos : VPOS;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    float gammaCorrection = max(1.0, TESR_ToneMapping.w);
    
    float4 NVRbloom = tex2D(TESR_BloomBuffer, IN.texcoord_1.xy);
    float4 bloom = tex2D(Src0, IN.ScreenOffset.xy);
    float4 final = tex2D(DestBlend, IN.texcoord_1.xy);
    NVRbloom.rgb = pows(NVRbloom.rgb, gammaCorrection) * TESR_BloomData.z; // linearize bloom
    bloom.rgb = pows(bloom.rgb, gammaCorrection); // linearize bloom
    final.rgb = pows(final.rgb, gammaCorrection); // linearize color
    
    bloom = lerp(bloom, NVRbloom, TESR_BloomData.w) + bloom * TESR_DebugVar.x;
    // scale bloom while maintaining color
    bloom.rgb = TESR_HDRBloomData.x * pows(max(bloom.rgb, 0.0), TESR_HDRBloomData.y);
    
    float cinematicScalar = TESR_HDRData.z;
    final.rgb = lerp(luma(final.rgb).xxx, final.rgb, Cinematic.x * cinematicScalar); // saturation
    final.rgb = lerp(final.rgb, Tint.rgb * luma(final.rgb), saturate(Tint.a * TESR_ToneMapping.z)); // apply tint
    
    float q0 = 1.0 / max(bloom.w, HDRParam.x); // HDRParam.x, brights cutoff
    final.rgb = ((q0 * HDRParam.x) * final.rgb) + max(bloom.rgb * (q0 * 0.5), 0.0); // blend image and bloom
    
    final.rgb = lerp(final.rgb, final.rgb * Cinematic.w, cinematicScalar); // apply brightness from Cinematic
    final.rgb = tonemap(final.rgb * TESR_HDRData.y); // exposure & tonemap using provided tonemapper
   
    final.rgb = pows(final.rgb, 1.0/max(1.0, TESR_HDRData.w)); // delinearise
    
    final.rgb = lerp(final.rgb, (Cinematic.z * (final.rgb - Cinematic.y)) + Cinematic.y, cinematicScalar * TESR_ToneMapping.y); // apply contrast from Cinematic, scaled by modifier
    final.rgb = lerp(final.rgb, Fade.rgb, Fade.a); // apply night eye and fade, gamma-space but vanilla accurate

    OUT.color_0 = float4(final.rgb,BlurScale.z); 

    return OUT;
};

// approximately 18 instruction slots used (2 texture, 16 arithmetic)
