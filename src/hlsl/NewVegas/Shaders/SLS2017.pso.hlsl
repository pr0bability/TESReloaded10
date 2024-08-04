// Basic lighting shader with one light Pass: BSSM_ADTS VSO: SLS2012
//
// Parameters:

float4 AmbientColor : register(c1);
float4 PSLightColor[10] : register(c3);
float4 Toggles : register(c27);
float4 TESR_TerrainData : register(c28);
float4 TESR_TerrainExtraData : register(c29);
float4 TESR_DebugVar : register(c31);

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   AmbientColor const_1       1
//   PSLightColor[0] const_3       1
//   Toggles      const_27      1
//   BaseMap      texture_0       1
//   NormalMap    texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float4 texcoord_1 : TEXCOORD1_centroid;
    float3 texcoord_3 : TEXCOORD3_centroid;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

#define useVertexColor Toggles.x
#define useFog Toggles.y
#define glossPower Toggles.z
#define alphaTestRef Toggles.w
#define tint green

#include "Includes/Helpers.hlsl"
#include "Includes/Object.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 texture0 = linearize(tex2D(BaseMap, IN.BaseUV.xy));
    
    // clip faded objects and no alpha pixels
    float4 r2 = (AmbientColor.a >= 1 ? 0 : (texture0.a - alphaTestRef));
    clip(r2.xyzw);

    float4 normal = tex2D(NormalMap, IN.BaseUV.xy);
    normal.xyz = normalize(expand(normal.xyz));

    float roughness = getRoughness(normal.a, glossPower);

    // final color
    float3 color = useVertexColor > 0 ? (texture0.rgb * linearize(IN.color_0.rgb)) : texture0.rgb;
    float3 eyeDir = normalize(IN.texcoord_3.xyz);
    float3 final = getSunLighting(color, roughness, normal.xyz, eyeDir, IN.texcoord_1.xyz, PSLightColor[0].rgb, AmbientColor.rgb);

    // apply fog
    // final = useFog ? lerp(color, IN.color_1.rgb, IN.color_1.a) : color;

    OUT.color_0.rgb = getFinalColor(final);
    OUT.color_0.a = texture0.a * AmbientColor.a;

    return OUT;
};

// approximately 38 instruction slots used (2 texture, 36 arithmetic)
