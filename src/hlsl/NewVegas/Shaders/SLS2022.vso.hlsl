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

    // const int4 const_4 = {4, 1, 2, 3};

    float3 mdl23;
    // float3 q0;
    // float1 q1;
    // float1 q2;
    // float3 q3;
    // float3 q4;
    float1 q5;
    // float4 r0;

    // q4.xyz = normalize(LightData[3].xyz - IN.position.xyz);
    // q3.xyz = normalize(LightData[2].xyz - IN.position.xyz);
    // q0.xyz = normalize(LightData[1].xyz - IN.position.xyz);
    mdl23.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    // q1.x = min(4, fvars0.z);
    // q2.x = frac(q1.x);
    // r0.x = dot(IN.binormal.xyz, q3.xyz);
    OUT.color_0.rgba = IN.color_0.rgba;
    OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    OUT.position.xyz = mdl23.xyz;
    
    // OUT.texcoord_2.w = 1;
    // OUT.texcoord_2.xyz = normalize(mul(TanSpaceProj, LightData[0].xyz));
    // r0.yzw = (const_4.yzw < ((q1.x < 0.0 ? (-q2.x < q2.x ? 1.0 : 0.0) : 0) + (q1.x - q2.x)) ? 1.0 : 0.0);
    
    // OUT.texcoord_3.w = r0.y * LightData[1].w;
    // OUT.texcoord_3.x = dot(IN.tangent.xyz, q0.xyz) * r0.y;
    // OUT.texcoord_3.y = r0.y * dot(IN.binormal.xyz, q0.xyz);
    // OUT.texcoord_3.z = r0.y * dot(IN.normal.xyz, q0.xyz);
    // r0.y = dot(IN.normal.xyz, q3.xyz);

    // OUT.texcoord_4.w = r0.z * LightData[2].w;
    // OUT.texcoord_4.x = r0.z * dot(IN.tangent.xyz, q3.xyz);
    // OUT.texcoord_4.yz = r0.z * r0.xy;
    // r0.y = dot(IN.normal.xyz, q4.xyz);
    // r0.x = dot(IN.binormal.xyz, q4.xyz);
    
    // OUT.texcoord_5.w = r0.w * LightData[3].w;
    // OUT.texcoord_5.x = r0.w * dot(IN.tangent.xyz, q4.xyz);
    // OUT.texcoord_5.yz = r0.w * r0.xy;
    // // log r0.x, r0.x
    
    // fog
    q5.x = 1 - saturate((FogParam.x - length(mdl23.xyz)) / FogParam.y);
    OUT.color_1.a = exp2(q5.x * FogParam.z);
    OUT.color_1.rgb = FogColor.rgb;

    OUT.texcoord_0.xy = IN.texcoord_0.xy;

    OUT.texcoord_1.xyz = mul(TanSpaceProj, LightData[0].xyz);
    // OUT.texcoord_1.xyz = IN.position.xyz;
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
