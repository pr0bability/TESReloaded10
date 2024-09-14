// basic object shader for 1 directional light and 3 point lights but no specular. Pass: BSSM_ADT4 VSO: SLS2022
//
// Parameters:

float4 AmbientColor : register(c1);
float4 EmittanceColor : register(c2);
float4 PSLightColor[10] : register(c3);
float4 PSLightPosition[8] : register(c19);
float4 Toggles : register(c27);

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   AmbientColor    const_1       1
//   EmittanceColor  const_2       1
//   PSLightColor[0]    const_3       4
//   PSLightPosition[0] const_19      3
//   Toggles         const_27      1
//   BaseMap         texture_0       1
//   NormalMap       texture_1       1
//


// Structures:

struct VS_INPUT {
    float4 texcoord_2 : TEXCOORD2;
    float3 texcoord_3 : TEXCOORD3;
    float4 texcoord_4 : TEXCOORD4;
    float4 texcoord_5 : TEXCOORD5;
    float2 BaseUV : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
    float4 color_0 : COLOR0;
    float4 color_1 : COLOR1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:
#define useVertexColor Toggles.x
#define useFog Toggles.y
#define glossPower Toggles.z
#define alphaTestRef Toggles.w
#define tint red 

#include "Includes/Helpers.hlsl"
#include "Includes/Object.hlsl"


VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

// #define	expand(v)		(((v) - 0.5) / 0.5)
// #define	compress(v)		(((v) * 0.5) + 0.5)
// #define	shade(n, l)		max(dot(n, l), 0)
// #define	shades(n, l)		saturate(dot(n, l))

//     const float4 const_7 = {-0.5, 1, 2, 3};

//     float3 noxel3;
//     float3 q0;
//     float3 q1;
//     float3 q10;
//     float3 q2;
//     float3 q21;
//     float3 q22;
//     float3 q23;
//     float3 q4;
//     float3 q5;
//     float3 q7;
//     float3 q9;
//     float4 r0;
//     float4 r1;
//     float4 r2;
//     float3 r3;
//     float3 r4;

//     noxel3.xyz = tex2D(NormalMap, IN.BaseUV.xy);
//     r0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);
//     q4.xyz = normalize(expand(noxel3.xyz));
    
//     q5.xyz = shades(q4.xyz, IN.texcoord_2.xyz) * PSLightColor[0].rgb;
//     r1.yzw = const_7.yzw;
    
//     q2.xyz = (PSLightPosition[2].xyz - IN.texcoord_1.xyz) / PSLightPosition[2].w;
//     r3.z = shades(q2.xyz, q2.xyz);
    
//     q1.xyz = (PSLightPosition[1].xyz - IN.texcoord_1.xyz) / PSLightPosition[1].w;
//     r3.y = shades(q1.xyz, q1.xyz);

//     q0.xyz = (PSLightPosition[0].xyz - IN.texcoord_1.xyz) / PSLightPosition[0].w;
//     r3.x = shades(q0.xyz, q0.xyz);
//     r3.xyz = 1 - r3.xyz;
//     q10.xyz = (Toggles.x <= 0.0 ? r0.xyz : (r0.xyz * IN.color_0.rgb));

//     r2.xyzw = (AmbientColor.a >= r1.y ? 0 : (r0.w - Toggles.w));
//     r1.w = (    clip(r2.xyzw);

// // ) - EmittanceColor.a;

//     r2.xyz = (r3.x * (shades(q4.xyz, normalize(IN.texcoord_3.xyz)) * PSLightColor[1].xyz)) + q5.xyz;
//     q7.xyz = (r1.y >= EmittanceColor.a ? q5.xyz : r2.xyz);

//     r4.xyz = (r3.y * (shades(q4.xyz, normalize(IN.texcoord_4.xyz)) * PSLightColor[2].xyz)) + q7.xyz;
//     q9.xyz = (r1.z >= EmittanceColor.a ? q7.xyz : r4.xyz);

//     q21.xyz = (r1.w >= 0.0 ? q9.xyz : ((r3.z * (shades(q4.xyz, normalize(IN.texcoord_5.xyz)) * PSLightColor[3].xyz)) + q9.xyz));

//     r1.w = r0.w * AmbientColor.a;
//     q22.xyz = max(q21.xyz + AmbientColor.rgb, 0);
//     q23.xyz = q22.xyz * q10.xyz;

//     r1.xyz = (Toggles.y <= 0.0 ? q23.xyz : ((IN.color_1.a * (IN.color_1.rgb - (q10.xyz * q22.xyz))) + q23.xyz));
//     OUT.color_0.rgba = r1.xyzw;


    float4 texture0 = linearize(tex2D(BaseMap, IN.BaseUV.xy));
    int uiLightCount = int(EmittanceColor.a);

    float4 r2 = (AmbientColor.a < 1 ? (texture0.a - alphaTestRef) : 0); // ??
    clip(r2.xyzw);

    float4 normal = tex2D(NormalMap, IN.BaseUV.xy);
    normal.xyz = normalize(expand(normal.xyz));

    // float roughness = (1 - pow(normal.a, glossPower));
    float roughness = getRoughness(normal.a, glossPower);

    float3 color = useVertexColor > 0 ? texture0.rgb * linearize(IN.color_0.rgb) : texture0.rgb;
    float3 eyeDir = normalize(IN.texcoord_3.xyz);

    // float3 final = PBR(0, roughness, color, normal.xyz, eyeDir, normalize(IN.texcoord_1.xyz), linearize(PSLightColor[0].rgb)) + color * linearize(AmbientColor.rgb);
    float3 final = getSunLighting(color, roughness, normal.xyz, eyeDir, IN.texcoord_1.xyz, PSLightColor[0].rgb, AmbientColor.rgb);
    final += getPointLightLighting(color, roughness, normal.xyz, eyeDir, IN.texcoord_2.xyz, IN.texcoord_2.w, PSLightColor[1].rgb);
    final += getPointLightLighting(color, roughness, normal.xyz, eyeDir, IN.texcoord_4.xyz, IN.texcoord_4.w, PSLightColor[2].rgb);
    final += getPointLightLighting(color, roughness, normal.xyz, eyeDir, IN.texcoord_5.xyz, IN.texcoord_5.w, PSLightColor[3].rgb);

    OUT.color_0.rgb = getFinalColor(final);
    // OUT.color_0.rgb = lerp(delinearize(final), yellow, 0.1);
    OUT.color_0.a = texture0.a * AmbientColor.a;

    return OUT;

};

// approximately 62 instruction slots used (2 texture, 60 arithmetic)
