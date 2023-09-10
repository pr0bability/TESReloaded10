// Shader to blend cloud layers from weathers transitions with the sky
//
// Parameters:

float2 Params : register(c4);
sampler2D TexMap : register(s0);
sampler2D TexMapBlend : register(s1);

float4 TESR_DebugVar;
float4 TESR_ReciprocalResolution;
float4 TESR_SunColor;
float4 TESR_SunDirection;


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Params       const_4       1
//   TexMap       texture_0       1
//   TexMapBlend  texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 TexUV : TEXCOORD0;
    float2 position: VPOS;
    float2 TexBlendUV : TEXCOORD1;
    float4 color_0 : COLOfinalColor;
};

struct VS_OUTPUT {
    float4 color_0 : COLOfinalColor; // sunglare active region/suncolor
};

// Code:
#include "Includes/Helpers.hlsl"
#include "Includes/Position.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;


    float4 cloudsWeather1 = tex2D(TexMap, IN.TexUV.xy);
    float4 cloudsWeather2 = tex2D(TexMapBlend, IN.TexBlendUV.xy);

    float4 cloudsWeatherBlend = lerp(cloudsWeather1, cloudsWeather2, Params.x); // weather transition
    cloudsWeather2.w = cloudsWeather2.w * Params.x;

    float4 finalColor = (weight(cloudsWeather1.xyz) == 0.0 ? cloudsWeather2 : (weight(cloudsWeather2.xyz) == 0.0 ? cloudsWeather1 : cloudsWeatherBlend)); // select either weather or blend
    finalColor.w = (1 - Params.x) * cloudsWeather1.w; // first cloud layer

    float4 color = float4((finalColor.xyz * IN.color_0.rgb) * Params.y, finalColor.w * IN.color_0.a);

    // finalColor: sunglare
    // finalColor alpha: clouds first layer

    OUT.color_0.rgba = selectColor(TESR_DebugVar.x, color, cloudsWeather2.aaaa, cloudsWeather1, cloudsWeatherBlend, finalColor, float4(finalColor.rgb, 1), float4(cloudsWeather2.rgb, 1), float4((finalColor.xyz * IN.color_0.rgb) * Params.y, 1), IN.color_0, Params.yyyy);

    // OUT.color_0.a = 0;

    return OUT;
};

// approximately 15 instruction slots used (2 texture, 13 arithmetic)
