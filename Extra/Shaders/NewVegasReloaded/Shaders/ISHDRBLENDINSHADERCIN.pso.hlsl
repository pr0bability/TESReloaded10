//
//
#define	ScreenSpace	Src0
// Parameters:

sampler2D ScreenSpace : register(s0);  // bloom buffer
sampler2D DestBlend : register(s1);    // base non tonemapped image
float4 HDRParam : register(c1);
float3 BlurScale : register(c2);
float4 Cinematic : register(c19); // x:saturation, y:brightness, z:contrast
float4 Tint : register(c20);  // weather Cinematic IS tint?
float4 Fade : register(c22);  // Night eye color
float4 TESR_DebugVar : register(c24);


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

// Structures:

struct VS_INPUT {
    float2 ScreenOffset : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:


// ACES tonemapping https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    // blends bloom

    // const float4 const_3 = {0.299, 0.587000012, 0.114, 0};

    float4 bloom = tex2D(ScreenSpace, IN.ScreenOffset.xy);
    float4 hdrImage = tex2D(DestBlend, IN.texcoord_1.xy);
    
    float4 final = hdrImage;
    final.w = BlurScale.z;

    float HDR = HDRParam.x * TESR_DebugVar.z; // brights cutoff?

    float q0 = 1.0 / max(bloom.w, HDR) * TESR_DebugVar.y;
    float3 q1 = ((q0 * HDR) * final.rgb) + max(bloom.xyz * (q0 * 0.5), 0);

    float screenluma = luma(q1);
    
    float3 q3 = lerp(screenluma.xxx, q1.rgb, Cinematic.x * TESR_DebugVar.x); // saturation
    float3 q2 = q3 + (Tint.a * ((screenluma * Tint.rgb) - q3)); // apply tint
    float3 q4 = Cinematic.w * q2 - Cinematic.y;
    final.xyz = lerp((Cinematic.z * q4) + Cinematic.y, Fade.xyz, Fade.w); // apply night fade

    OUT.color_0.a = 1;
    OUT.color_0.rgb = final;
    // OUT.color_0.rgb = ACESFilm(hdrImage + bloom);
    OUT.color_0.rgb = selectColor(TESR_DebugVar.w, final, (Cinematic.z * q4) + Cinematic.y, hdrImage, q1, q3, q4, bloom, q0, Tint, Fade);
    // OUT.color_0.rgb = Tint;

    return OUT;
};

// approximately 18 instruction slots used (2 texture, 16 arithmetic)
