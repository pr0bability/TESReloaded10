// Basic Lighting shader with one directional light and one point light
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);
float4 FogParam : register(c14);
float3 FogColor : register(c15);
float4 EyePosition : register(c16);
float4 LightData[10] : register(c25);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   FogParam      const_14      1
//   FogColor      const_15      1
//   EyePosition   const_16      1
//   LightData[0]     const_25      2
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 texcoord_0 : TEXCOORD0;
    float4 color_0 : COLOR0;

#define	TanSpaceProj	float3x3(IN.tangent.xyz, IN.binormal.xyz, IN.normal.xyz)
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
    float4 texcoord_2 : TEXCOORD2;
    float3 texcoord_3 : TEXCOORD3;
    float3 texcoord_4 : TEXCOORD4;
    float4 texcoord_5 : TEXCOORD5;
};

// Code:

#include "Includes/helpers.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

// #define	expand(v)		(((v) - 0.5) / 0.5)
// #define	compress(v)		(((v) * 0.5) + 0.5)

    float3 eye0;
    float3 mdl21;
    float3 q3;
    float1 q6;

    mdl21.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    OUT.color_0.rgba = IN.color_0.rgba;

    // fog
    q6.x = 1 - saturate((FogParam.x - length(mdl21.xyz)) / FogParam.y);
    OUT.color_1.a = exp2(q6.x * FogParam.z);
    OUT.color_1.rgb = FogColor.rgb;

    OUT.position.xyz = mdl21.xyz;
    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);

    OUT.texcoord_0.xy = IN.texcoord_0.xy;

    OUT.texcoord_1.xyz = normalize(mul(TanSpaceProj, LightData[0].xyz));
    OUT.texcoord_1.w = LightData[0].w;

    OUT.texcoord_2.xyz = mul(TanSpaceProj, LightData[1].xyz - IN.position.xyz);
    OUT.texcoord_2.w = LightData[1].w;

    OUT.texcoord_3.xyz = normalize(mul(TanSpaceProj, (EyePosition.xyz - IN.position.xyz)));

    // q3.xyz = normalize(LightData[1].xyz - IN.position.xyz); // light to point vector
    OUT.texcoord_4.xyz = IN.normal; // unused
    // OUT.texcoord_4.xyz = mul(TanSpaceProj, normalize(normalize(eye0.xyz) + q3.xyz));

    OUT.texcoord_5.xyz = IN.tangent;	// normalized light vector (based on light radius), unused
    // OUT.texcoord_5.xyz = compress(q3.xyz / LightData[1].w);	// normalized light vector (based on light radius), unused
    // OUT.texcoord_5.w = 0.5;

    return OUT;
};

// approximately 59 instruction slots used
