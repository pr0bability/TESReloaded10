// Terrain vertex shader
//
// Parameters:

float3 FogColor : register(c15);
float4 FogParam : register(c14);
row_major float4x4 ModelViewProj : register(c0);
float4x4 TESR_InvViewProjectionTransform : register(c36);


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
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 normal : NORMAL;
    float4 texcoord_0 : TEXCOORD0;
    float4 color_0 : COLOR0;
    float4 texcoord_1 : TEXCOORD1;
    float4 texcoord_2 : TEXCOORD2;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float3 texcoord_1 : TEXCOORD1;
    float3 texcoord_2 : TEXCOORD2;
    float3 texcoord_3 : TEXCOORD3;
    float3 texcoord_4 : TEXCOORD4;
    float3 texcoord_5 : TEXCOORD5;
    float4 texcoord_7 : TEXCOORD7;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 mdl0;
    float4 r0;

    mdl0.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);

    OUT.color_0 = IN.texcoord_1;
    OUT.color_1 = IN.texcoord_2;

    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.position.xyz = mdl0.xyz;
    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    OUT.texcoord_1.xyz = IN.color_0.rgb;
    OUT.texcoord_2.xyz = IN.position.xyz;
    OUT.texcoord_3.xyz = IN.tangent.xyz;
    OUT.texcoord_4.xyz = IN.binormal.xyz;
    OUT.texcoord_5.xyz = IN.normal.xyz;
    r0.zw = FogParam.z;
    r0.xy = 1 - saturate((FogParam.x - length(mdl0.xyz)) / FogParam.y);
    // lit r0, r0
    OUT.texcoord_7.w = r0.z;
    OUT.texcoord_7.xyz = mul(TESR_InvViewProjectionTransform, OUT.position);

    return OUT;
};

// approximately 27 instruction slots used
