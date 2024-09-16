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
    float4 uv : TEXCOORD0;
    float4 vertex_color : COLOR0;
    float4 blend_0 : TEXCOORD1;
    float4 blend_1 : TEXCOORD2;
};

struct VS_OUTPUT {
    float4 blend_0 : COLOR0;
    float4 blend_1 : COLOR1;
    float4 sPosition : POSITION;
    float2 uv : TEXCOORD0;
    float3 vertex_color : TEXCOORD1;
    float3 lPosition : TEXCOORD2;
    float3 tangent : TEXCOORD3;
    float3 binormal : TEXCOORD4;
    float3 normal : TEXCOORD5;
    float4 viewPosition : TEXCOORD7;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 mdl0;
    float4 r0;

    mdl0.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);

    OUT.blend_0 = IN.blend_0;
    OUT.blend_1 = IN.blend_1;

    OUT.sPosition.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.sPosition.xyz = mdl0.xyz;
    OUT.uv.xy = IN.uv.xy;
    OUT.vertex_color.xyz = IN.vertex_color.rgb;
    OUT.lPosition.xyz = IN.position.xyz;
    OUT.tangent.xyz = IN.tangent.xyz;
    OUT.binormal.xyz = IN.binormal.xyz;
    OUT.normal.xyz = IN.normal.xyz;
    r0.zw = FogParam.z;
    r0.xy = 1 - saturate((FogParam.x - length(mdl0.xyz)) / FogParam.y);
    // lit r0, r0
    OUT.viewPosition.w = r0.z;
    OUT.viewPosition.xyz = mul(TESR_InvViewProjectionTransform, OUT.sPosition).xyz;

    return OUT;
};

// approximately 27 instruction slots used
