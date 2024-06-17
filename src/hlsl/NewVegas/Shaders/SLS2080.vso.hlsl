// Vertex Shader for LOD terrain during transition with near terrain
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);
float4 FogParam : register(c14);
float3 FogColor : register(c15);
float4 LandBlendParams : register(c19);
float4 LightData[10] : register(c25);
float4 TESR_CameraPosition : register(c35);
float4x4 TESR_InvViewProjectionTransform : register(c36);


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   ModelViewProj[0]   const_0        1
//   ModelViewProj[1]   const_1        1
//   ModelViewProj[2]   const_2        1
//   ModelViewProj[3]   const_3        1
//   FogParam        const_14      1
//   FogColor        const_15      1
//   LandBlendParams const_19      1
//   LightData[0]       const_25      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float3 texcoord_3 : TEXCOORD3;
    float texcoord_4 : TEXCOORD4;
    float4 texcoord_5 : TEXCOORD5;
    float3 location : TEXCOORD6;
    float3 worldpos : TEXCOORD7;
};


#include "includes/Helpers.hlsl"
// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r0;

    float3 mdl4 = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);

    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.position.xyz = mdl4.xyz;

    r0.xy = (IN.texcoord_0.xy * 0.015625) + LandBlendParams.xy;
    r0.z = 1 - r0.x;
    
    OUT.texcoord_0.xy = (r0.zy * 0.9921875) + (1.0 / 256);

    r0.xy = sqr(LandBlendParams.zw - IN.position.xy);
    r0.zw = FogParam.z;

    OUT.texcoord_3.xyz = LightData[0].xyz;
    OUT.texcoord_4.x = 1 - saturate((9625.59961 - sqrt(r0.y + r0.x)) * 0.000375600968);
    r0.xy = 1 - saturate((FogParam.x - length(mdl4.xyz)) / FogParam.y);

    // lit r0, r0
    OUT.texcoord_5.w = r0.z;
    OUT.texcoord_5.xyz = FogColor.rgb;

    OUT.location = mul(TESR_InvViewProjectionTransform, OUT.position);
    OUT.worldpos = OUT.location + TESR_CameraPosition;

    return OUT;
};

// approximately 33 instruction slots used
