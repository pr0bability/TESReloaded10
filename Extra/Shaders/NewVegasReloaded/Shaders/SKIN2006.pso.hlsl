// Skin shader for 2 extra point lights (light 2 and 3) 
//
// Parameters:

sampler2D BaseMap : register(s0);
sampler2D NormalMap : register(s1);
sampler2D GlowMap : register(s3);
sampler2D AttenuationMap : register(s4);

float4 AmbientColor : register(c1);
float4 PSLightColor[10];

float4 TESR_ReciprocalResolution;
float4 TESR_SkinData;
float4 TESR_SkinColor;
float4 TESR_DebugVar;


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   AmbientColor   const_1       1
//   PSLightColor[0]   SunLightColor       3
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   GlowMap        texture_3       1
//   AttenuationMap texture_4       1
//

#include "Includes/helpers.hlsl"

// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;
    float3 texcoord_1 : TEXCOORD1_centroid;			// eyeDirection for pointlight1
    float3 texcoord_2 : TEXCOORD2_centroid;			// light direction from pointlight
    float3 texcoord_3 : TEXCOORD3_centroid;			// light direction from pointlight2
    float4 texcoord_4 : TEXCOORD4;			        // attenuation map UV
    float4 texcoord_5 : TEXCOORD5;			        // attenuation map2 UV
    float3 texcoord_7 : TEXCOORD7_centroid;			// eyeDirection for pointlight2
};


struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:
#include "Includes/Skin.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float1 att1;
    float1 att3;
    float1 att39;
    float1 att4;
    float3 noxel2;
    float1 q10;
    float1 q11;
    float1 q13;
    float1 q16;
    float1 q17;
    float1 q18;
    float1 q19;
    float3 q40;
    float3 q5;
    float3 q6;
    float1 q8;
    float1 q9;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r4;
    float4 r5;
    float3 r6;
    float4 texel0;

    const float4 SunLightColor = PSLightColor[1];
    const float4 PointLight1Color = PSLightColor[2];
    const float4 PointLight2Color = PSLightColor[3];

    noxel2.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r4.xyzw = tex2D(GlowMap, IN.BaseUV.xy);			// partial precision
    texel0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    OUT.color_0.rgba = texel0.xyzw;			// partial precision
    att4.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    att3.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    att1.x = tex2D(AttenuationMap, IN.texcoord_5.xy);			// partial precision
    att39.x = tex2D(AttenuationMap, IN.texcoord_5.zw);			// partial precision
    r2.w = saturate((1 - att1.x) - att39.x);			// partial precision
    q6.xyz = normalize(expand(noxel2.xyz));			// partial precision
    q5.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    q13.x = sqr(1 - shades(q6.xyz, q5.xyz));			// partial precision
    r4.w = shades(q5.xyz, -normalize(IN.texcoord_2.xyz)) * q13.x;			// partial precision
    q16.x = dot(q6.xyz, normalize(IN.texcoord_3.xyz));
    q18.x = saturate(q16.x);			// partial precision
    q17.x = saturate((q16.x + 0.3) * 0.769230783);
    q19.x = saturate(((3 - (q17.x * 2)) * sqr(q17.x)) - ((3 - (q18.x * 2)) * sqr(q18.x)));			// partial precision
    q8.x = dot(q6.xyz, normalize(IN.texcoord_2.xyz));
    q9.x = saturate(q8.x);			// partial precision
    q10.x = saturate((q8.x + 0.3) * 0.769230783);
    q11.x = saturate(((3 - (q10.x * 2)) * sqr(q10.x)) - ((3 - (q9.x * 2)) * sqr(q9.x)));			// partial precision
    r2.xyz = (q13.x * shades(q5.xyz, -normalize(IN.texcoord_3.xyz))) * lerp(PointLight2Color.xyz, r4.xyz, 0.5);			// partial precision
    q40.xyz = (q11.x * r4.xyzw) + ((PointLight1Color.xyzw * q9.x) + (r4.w * lerp(PointLight1Color.xyzw, r4.wzyx, 0.5)));			// partial precision
    r0.yzw = (PointLight2Color.xyzw * q18.x) + r2.xyzw;			// partial precision
    r5.yzw = ((q13.x * shades(q5.xyz, -IN.texcoord_1)) * SunLightColor.xyzw) * 0.5;			// partial precision
    r1.yzw = (saturate((1 - att3.x) - att4.x) * q40.xyz) + ((SunLightColor.xyzw * shades(q6.xyz, IN.texcoord_1.xyz)) + r5.yzw);			// partial precision
    r6.xyz = ((r2.w * ((q19.x * r4.yzw) + r0.yzw)) + r1.yzw) + AmbientColor.rgb;			// partial precision

    OUT.color_0.rgba = selectColor(TESR_DebugVar.x, float4(r6, 1), PSLightColor[0], PSLightColor[1], PSLightColor[2], PSLightColor[3], PSLightColor[4], PSLightColor[5], PSLightColor[6], PSLightColor[7], PSLightColor[8]);
    return OUT;
};

// approximately 75 instruction slots used (7 texture, 68 arithmetic)
