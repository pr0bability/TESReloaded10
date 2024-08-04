// basic object shader for 1 directional light and 3 point lights but no specular. Pass: BSSM_ADT4 PSO: SLS2031
//
// Parameters:

float3 FogColor : register(c15);
float4 FogParam : register(c14);
float4 LightData[10] : register(c25);
row_major float4x4 ModelViewProj : register(c0);
float4 fvars0 : register(c17);

float4 TESR_CameraPosition;

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
//   fvars0        const_17      1
//   LightData[0]     const_25      4
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
    float4 texcoord_4 : TEXCOORD4;
    float4 texcoord_5 : TEXCOORD5;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 mdl23;
    float1 q5;
    mdl23.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    OUT.color_0.rgba = IN.color_0.rgba;
    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.position.xyz = mdl23.xyz;
    
    
    // fog
    q5.x = 1 - saturate((FogParam.x - length(mdl23.xyz)) / FogParam.y);
    OUT.color_1.a = exp2(q5.x * FogParam.z);
    OUT.color_1.rgb = FogColor.rgb;

    OUT.texcoord_0.xy = IN.texcoord_0.xy;

    OUT.texcoord_1.xyz = mul(TanSpaceProj, LightData[0].xyz);
    OUT.texcoord_1.w = LightData[0].w;

    OUT.texcoord_2.xyz = mul(TanSpaceProj, LightData[1].xyz - IN.position.xyz);
    OUT.texcoord_2.w = LightData[1].w;

    OUT.texcoord_3 = mul(TanSpaceProj, TESR_CameraPosition.xyz - IN.position.xyz);

    OUT.texcoord_4.xyz = mul(TanSpaceProj, LightData[2].xyz - IN.position.xyz);
    OUT.texcoord_4.w = LightData[2].w;

    OUT.texcoord_5.xyz = mul(TanSpaceProj, LightData[3].xyz - IN.position.xyz);
    OUT.texcoord_5.w = LightData[3].w;

    return OUT;
};

// approximately 68 instruction slots used
