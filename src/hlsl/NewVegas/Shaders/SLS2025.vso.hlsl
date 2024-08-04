// basic object shader with one direct light and two point lights. Pass BSSM_ADTS10 PSO: SLS2034
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);
float4 FogParam : register(c14);
float3 FogColor : register(c15);
float4 EyePosition : register(c16);
float4 fvars0 : register(c17);
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
//   fvars0        const_17      1
//   LightData[0]     const_25      3
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
    float3 texcoord_5 : TEXCOORD5;
    float3 texcoord_6 : TEXCOORD6;
    float3 texcoord_7 : TEXCOORD7;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    // const int4 const_4 = {3, 1, 2, 0};

    // float3 eye0;
    // float3 mdl52;
    // float3 q12;
    // float1 q13;
    // float3 q4;
    // float3 q6;
    // float1 q8;
    // float1 q9;
    // float2 r0;
    // float4 r1;

    // mdl52.xyz = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), IN.position.xyzw);
    // q8.x = min(3, fvars0.z);
    // q9.x = frac(q8.x);
    // q4.xyz = LightData[1].xyz - IN.position.xyz;
    // q6.xyz = normalize(q4.xyz);
    // OUT.color_0.rgba = IN.color_0.rgba;
    // OUT.position.w = dot(ModelViewProj[3].xyzw, IN.position.xyzw);
    // OUT.position.xyz = mdl52.xyz;
    // OUT.texcoord_2.w = LightData[0].w;
    // OUT.texcoord_2.xyz = normalize(mul(TanSpaceProj, LightData[0].xyz));
    // eye0.xyz = EyePosition.xyz - IN.position.xyz;
    // q13.x = 1 - saturate((FogParam.x - length(mdl52.xyz)) / FogParam.y);
    // r1.xy = (const_4.yz < ((q8.x < 0.0 ? (-q9.x < q9.x ? 1.0 : 0.0) : 0) + (q8.x - q9.x)) ? 1.0 : 0.0);
    // OUT.texcoord_3.w = r1.x * LightData[1].w;
    // OUT.texcoord_3.x = dot(IN.tangent.xyz, q6.xyz) * r1.x;
    // OUT.texcoord_3.y = r1.x * dot(IN.binormal.xyz, q6.xyz);
    // OUT.texcoord_3.z = r1.x * dot(IN.normal.xyz, q6.xyz);


    // OUT.texcoord_4.w = r1.y * LightData[2].w;
    // OUT.texcoord_5.xyz = normalize(mul(TanSpaceProj, normalize(normalize(eye0.xyz) + LightData[0].xyz)));
    // OUT.texcoord_6.xyz = r1.x * mul(TanSpaceProj, normalize(normalize(q4.xyz) + normalize(eye0.xyz)));
    // r1.xzw = LightData[2].xyyz - IN.position.xyyz;
    // q12.xyz = r1.xzww / length(r1.xzw);
    // r0.y = dot(IN.normal.xyz, q12.xyz);
    // r0.x = dot(IN.binormal.xyz, q12.xyz);
    // OUT.texcoord_4.x = r1.y * dot(IN.tangent.xyz, q12.xyz);
    // OUT.texcoord_4.yz = r1.y * r0.xy;
    // OUT.texcoord_7.xyz = r1.y * mul(TanSpaceProj, normalize(normalize(r1.xzw) + normalize(eye0.xyz)));
    // //log r0.x, r0.x
    // OUT.color_1.a = exp2(q13.x * FogParam.z);
    // OUT.color_1.rgb = FogColor.rgb;
    // OUT.texcoord_0.xy = IN.texcoord_0.xy;
    // OUT.texcoord_1.w = LightData[2].w;
    // OUT.texcoord_1.xyz = IN.position.xyz;

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

    OUT.texcoord_3 = mul(TanSpaceProj, EyePosition.xyz - IN.position.xyz);

    OUT.texcoord_4.xyz = mul(TanSpaceProj, LightData[2].xyz - IN.position.xyz);
    OUT.texcoord_4.w = LightData[2].w;


    // q3.xyz = normalize(LightData[1].xyz - IN.position.xyz); // light to point vector
    // OUT.texcoord_4.xyz = IN.normal; // unused
    // OUT.texcoord_4.xyz = mul(TanSpaceProj, normalize(normalize(eye0.xyz) + q3.xyz));

    OUT.texcoord_5 = IN.normal;	// normalized light vector (based on light radius), unused
    OUT.texcoord_6 = IN.tangent;	// normalized light vector (based on light radius), unused
    OUT.texcoord_7 = IN.binormal;	// normalized light vector (based on light radius), unused


    return OUT;
};

// approximately 88 instruction slots used
