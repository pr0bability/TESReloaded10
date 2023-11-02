// Tonemap & apply bloom/cinematic filter from weather for scene with alpha mask
//
#define	ScreenSpace	Src0
// Parameters:
sampler2D ScreenSpace : register(s0);
sampler2D DestBlend : register(s1);

float4 HDRParam : register(c1);
float3 BlurScale : register(c2);
float4 Cinematic : register(c19);
float4 Tint : register(c20);
float4 Fade : register(c22);
float4 UseAlphaMask : register(c23);
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
//   UseAlphaMask const_23      1
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
    float maxBloom = max(bloom.r, max(bloom.g, bloom.b));
    float4 ratio = bloom / maxBloom;
    bloom.rgb = TESR_HDRBloomData.x * pows(max(maxBloom, 0), TESR_HDRBloomData.y) * ratio;

    float4 hdrImage = tex2D(DestBlend, IN.texcoord_1.xy); 
    float3 final = pows(hdrImage.rgb, 1/TESR_ToneMapping.w) * TESR_HDRData.y; // linearize & exposure

    float HDR = HDRParam.x; // brights cutoff?
    float q0 = 1.0 / max(bloom.w, HDR);
    final = ((q0 * HDR) * final) + max(bloom.rgb * (q0 * 0.5), 0); // blend image and bloom

    float screenluma = luma(final);
    final = lerp(screenluma.xxx, final, Cinematic.x * TESR_HDRData.z); // saturation

    final = tonemap(final);
    //float3 tint = tonemap(screenluma * Tint.rgb); // tonemap tint to simulate tinting before tonemap
 
    screenluma = saturate(luma(final));
    final = lerp(final, Tint.rgb, Tint.a * TESR_ToneMapping.z); // apply tint
    final *= lerp(1, Fade.rgb, lerp(Fade.a, 0, screenluma)); // apply night eye only to darker parts of the scene to avoid dulling bloom

    float4 background = tex2D(DestBlend, IN.ScreenOffset.xy); // sdr image (already tonemapped) displayed within the mask
    OUT.color_0.rgb = (background.w == 0 ? background.rgb : pows(final.rgb, TESR_HDRData.w)); //only tonemap the area within the mask
    OUT.color_0.a = BlurScale.z;
    //OUT.color_0.a = 1;

    return OUT;
};

// approximately 22 instruction slots used (3 texture, 19 arithmetic)
