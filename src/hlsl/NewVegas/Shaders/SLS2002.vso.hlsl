// LOD terrain vertex shader
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);
row_major float4x4 ObjToCubeSpace : register(c8);
float4 HighDetailRange : register(c12);
float4 FogParam : register(c14);
float3 FogColor : register(c15);
float4 GeomorphParams : register(c19);
float4 LightData[10] : register(c25);
float4 TESR_CameraPosition : register(c36);
float4x4 TESR_InvViewProjectionTransform : register(c37);

// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   ModelViewProj[0]   const_0        1
//   ModelViewProj[1]   const_1        1
//   ModelViewProj[2]   const_2        1
//   ModelViewProj[3]   const_3        1
//   ObjToCubeSpace[0]  const_8        1
//   ObjToCubeSpace[1]  const_9        1
//   HighDetailRange const_12      1
//   FogParam        const_14      1
//   FogColor        const_15      1
//   GeomorphParams  const_19      1
//   LightData[0]       const_25      1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
};

struct VS_OUTPUT {
    float4 color_1 : COLOR1;
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float3 texcoord_1 : TEXCOORD1;
    float3 location : TEXCOORD2;
    float3 worldpos : TEXCOORD3;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 r0 = IN.position;
    float4 r1 = IN.position;
    r1.z = lerp(IN.texcoord_1.x, IN.position.z, GeomorphParams.x);

    float q0 = (abs(dot(ObjToCubeSpace[1].xyzw, r1.xyzw) - HighDetailRange.y) < HighDetailRange.w ? 1.0 : 0.0);
    float q1 = (abs(dot(ObjToCubeSpace[0].xyzw, r1.xyzw) - HighDetailRange.x) < HighDetailRange.z ? 1.0 : 0.0);

    r0.z = r1.z - ((q0.x * q1.x) * GeomorphParams.y);
    float3 mdl11 = mul(float3x4(ModelViewProj[0].xyzw, ModelViewProj[1].xyzw, ModelViewProj[2].xyzw), r0.xyzw);

    // fog
    float q2 = 1 - saturate((FogParam.x - length(mdl11)) / FogParam.y);
    // log r0.x, r0.x
    OUT.color_1.a = exp2(q2.x * FogParam.z);
    OUT.color_1.rgb = FogColor.rgb;

    OUT.position.w = dot(ModelViewProj[3].xyzw, r0.xyzw);
    OUT.position.xyz = mdl11.xyz;
    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    OUT.texcoord_1 = LightData[0].xyz; // sun direction

    OUT.location.xyz = mul(TESR_InvViewProjectionTransform, OUT.position).xyz;
    OUT.worldpos = OUT.location.xyz + TESR_CameraPosition.xyz;

    return OUT;
};

// approximately 34 instruction slots used
