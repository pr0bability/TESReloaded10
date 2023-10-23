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
float4 TESR_HDRBloomData : register(c25);
float4 TESR_SunAmount : register(c26);
float4 TESR_HDRData : register(c27);
float4 TESR_LotteData : register(c28);
float4 TESR_ToneMapping : register(c29);

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

    float4 bloom = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    bloom.rgb = TESR_HDRBloomData.x * pow(max(bloom.rgb, 0), TESR_HDRBloomData.y);
    float4 hdrImage = tex2D(DestBlend, IN.texcoord_1.xy) * TESR_HDRData.y; // exposure

    float4 final = pow(hdrImage, 1/TESR_ToneMapping.w); // linearize
    final.w = BlurScale.z;

    float HDR = HDRParam.x; // brights cutoff?

    float q0 = 1.0 / max(bloom.w, HDR);
    final.rgb = ((q0 * HDR) * final.rgb) + max(bloom.rgb * (q0 * 0.5), 0); // blend image and bloom

    float screenluma = luma(final.rgb);
    final.rgb = lerp(screenluma.xxx, final.rgb, Cinematic.x * TESR_HDRData.z); // saturation
    float3 q2 = lerp(final.rgb, Tint.rgb * screenluma, Tint.a); // apply tint

    float3 oldTonemap = Cinematic.z * (Cinematic.w * q2.rgb - Cinematic.y) + Cinematic.y; // apply cinematic brightness & contrast modifiers
    final.rgb = selectColor(TESR_HDRData.x * 0.1, oldTonemap, ACESFilm(q2), Reinhard(q2, TESR_HDRBloomData.w), Lottes(q2, TESR_LotteData.x, TESR_LotteData.y,  TESR_LotteData.z, TESR_HDRBloomData.w, TESR_LotteData.w), Uncharted2Tonemap(q2), q2, q2, q2, q2, q2 ); // tonemap

    final.rgb *= lerp(1, Fade.rgb, lerp(Fade.a, 0, saturate(luma(final.rgb)))); // apply night eye only to darker parts of the scene to avoid dulling bloom

    OUT.color_0.a = 1;
    OUT.color_0.rgb = pow(final.rgb, TESR_HDRData.w);

    return OUT;
};

// approximately 18 instruction slots used (2 texture, 16 arithmetic)
