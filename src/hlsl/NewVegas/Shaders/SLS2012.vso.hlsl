// Generic lighting vertex shader for one directional light
//
// Parameters:

float4 EyePosition : register(c16);
float3 FogColor : register(c15);
float4 FogParam : register(c14);
float4 LightData[10] : register(c25);
row_major float4x4 ModelViewProj : register(c0);


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
//   LightData[0]     const_25      1
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
    float3 texcoord_3 : TEXCOORD3;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 l7;
    float3 mdl8;
    float1 q2;

    mdl8.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    l7.xyz = mul(TanSpaceProj, normalize(normalize(EyePosition.xyz - IN.position.xyz) + LightData[0].xyz));
    OUT.color_0.rgba = IN.color_0.rgba;

    // fog
    q2.x = log(1 - saturate((FogParam.x - length(mdl8.xyz)) / FogParam.y));
    //log r0.x, r0.x
    OUT.color_1.rgb = FogColor.rgb;
    OUT.color_1.a = exp2(q2.x * FogParam.z);
    
    OUT.position.xyz = mdl8.xyz;
    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    
    OUT.texcoord_1.xyz = normalize(mul(TanSpaceProj, LightData[0].xyz));
    OUT.texcoord_1.w = LightData[0].w;
    
    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    OUT.texcoord_3.xyz = normalize(l7.xyz);

    return OUT;
};

// approximately 40 instruction slots used
