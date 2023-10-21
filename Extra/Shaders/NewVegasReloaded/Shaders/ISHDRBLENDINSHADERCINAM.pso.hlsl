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

// Structures:

struct VS_INPUT {
    float2 ScreenOffset : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

// VS_OUTPUT main(VS_INPUT IN) {
//     VS_OUTPUT OUT;

//     const float4 const_0 = {0.299, 0.587000012, 0.114, 0}; // luma coeffs

//     float4 r0 = tex2D(ScreenSpace, IN.ScreenOffset.xy);
//     float4 r1 = tex2D(DestBlend, IN.texcoord_1.xy); // hdr image to tonemap
//     float4 r2 = tex2D(DestBlend, IN.ScreenOffset.xy); // sdr image (already tonemapped) displayed within the mask
    
//     float q0 = 1.0 / max(r0.w, HDRParam.x);
//     float3 q1 = ((q0 * HDRParam.x) * r1.rgb) + max(r0.rgb * (q0 * 0.5), 0);
    
//     float sceneluma = dot(q1.xyz, const_0.rgb);

//     float3 q3 = lerp(sceneluma, q1, Cinematic.x); // saturation

//     float3 q4 = (Cinematic.w * ((Tint.a * ((sceneluma * Tint.rgb) - q3)) + q3)) - Cinematic.y;
//     float3 q5 = lerp(r2.rgb, lerp((Cinematic.z * q4) + Cinematic.y, Fade.xyz, Fade.w), UseAlphaMask.w);

//     OUT.color_0.a = BlurScale.z;
//     OUT.color_0.rgb = (r2.w == 0 ? r2.rgb : q5.rgb); //only tonemap the area within the mask

//     return OUT;
// };


// ACES tonemapping https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x) {
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

float3 Reinhard(float3 x, float whitepoint) {
    return x * (1 + x / whitepoint.xxx)/(1 + x);
}

float3 Uncharted2Tonemap(float3 x) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 bloom = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    bloom.rgb = TESR_HDRBloomData.x * pow(max(bloom.rgb, 0), TESR_HDRBloomData.y);
    float4 hdrImage = tex2D(DestBlend, IN.texcoord_1.xy) * TESR_HDRData.y; // exposure
    float4 background = tex2D(DestBlend, IN.ScreenOffset.xy); // sdr image (already tonemapped) displayed within the mask

    float4 final = hdrImage;
    final.w = BlurScale.z;

    float HDR = HDRParam.x; // brights cutoff?

    float q0 = 1.0 / max(bloom.w, HDR);
    final.rgb = ((q0 * HDR) * final.rgb) + max(bloom.rgb * (q0 * 0.5), 0); // blend image and bloom

    float screenluma = luma(final.rgb);
    final.rgb = lerp(screenluma.xxx, final.rgb, Cinematic.x * TESR_HDRData.z); // saturation
    float3 q2 = lerp(final.rgb, Tint.rgb * screenluma, Tint.a); // apply tint

    float3 oldTonemap = Cinematic.z * (Cinematic.w * q2.rgb - Cinematic.y) + Cinematic.y; // apply cinematic brightness & contrast modifiers
    final.rgb = selectColor(TESR_HDRData.x * 0.1, oldTonemap, ACESFilm(q2), Reinhard(q2, TESR_HDRBloomData.w), Uncharted2Tonemap(q2), q2, q2, q2, q2, q2, q2 ); // tonemap

    final.rgb = lerp(final.rgb, Fade.rgb, Fade.a * (1 - luma(q2))); // apply night eye only to darker parts of the scene to avoid dulling bloom

    OUT.color_0.a = BlurScale.z;;

    OUT.color_0.rgb = (background.w == 0 ? background.rgb : pow(final.rgb, TESR_HDRData.w)); //only tonemap the area within the mask

    return OUT;
};

// approximately 22 instruction slots used (3 texture, 19 arithmetic)
