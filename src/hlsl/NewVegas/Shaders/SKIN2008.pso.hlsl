//
//
// Parameters:

sampler2D AttenuationMap : register(s3);
sampler2D NormalMap : register(s0);
float4 PSLightColor[10];


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   PSLightColor[0]   const_3       2
//   NormalMap      texture_0       1
//   AttenuationMap texture_3       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float4 texcoord_4 : TEXCOORD4;			// partial precision
    float4 texcoord_5 : TEXCOORD5;			// partial precision
    float2 NormalUV : TEXCOORD0;			// partial precision
    float3 texcoord_7 : TEXCOORD7_centroid;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

#define	expand(v)		(((v) - 0.5) / 0.5)
#define	compress(v)		(((v) * 0.5) + 0.5)
#define	shade(n, l)		max(dot(n, l), 0)
#define	shades(n, l)		saturate(dot(n, l))
#define	weight(v)		dot(v, 1)
#define	sqr(v)			((v) * (v))

    const float4 const_2 = {0.3, 0, 0, 0};

    float1 att0;
    float1 att1;
    float1 att13;
    float1 att2;
    float3 noxel3;
    float3 q10;
    float1 q12;
    float3 q14;
    float3 q15;
    float1 q16;
    float1 q17;
    float1 q18;
    float3 q258;
    float3 q4;
    float3 q44;
    float3 q5;
    float1 q6;
    float1 q7;
    float1 q8;
    float1 q9;
    float4 r0;
    float4 r1;
    float4 r3;

    noxel3.xyz = tex2D(NormalMap, IN.NormalUV.xy);			// partial precision
    att1.x = tex2D(AttenuationMap, IN.texcoord_5.xy);			// partial precision
    att13.x = tex2D(AttenuationMap, IN.texcoord_5.zw);			// partial precision
    att2.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    att0.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    r0.w = saturate((1 - att0.x) - att2.x);			// partial precision
    q4.xyz = normalize(expand(noxel3.xyz));			// partial precision
    q10.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    q12.x = sqr(1 - shades(q4.xyz, q10.xyz));			// partial precision
    q5.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    q15.xyz = normalize(IN.texcoord_1.xyz);			// partial precision
    q6.x = dot(q4.xyz, q5.xyz);
    q7.x = saturate(q6.x);			// partial precision
    q8.x = saturate((q6.x + 0.3) * 0.769230783);
    q9.x = saturate(((3 - (q8.x * 2)) * sqr(q8.x)) - ((3 - (q7.x * 2)) * sqr(q7.x)));			// partial precision
    q16.x = dot(q4.xyz, q15.xyz);
    q17.x = saturate(q16.x);			// partial precision
    q18.x = saturate((q16.x + 0.3) * 0.769230783);
    r3.xyz = const_2.xyz;			// partial precision
    r3.w = -0.5;
    q14.xyz = (q7.x * const_4.xyz) + ((q12.x * shades(q10.xyz, -q5.xyz)) * lerp(const_4.xyz, r3.xyz, -r3.w));			// partial precision
    r1.yzw = (const_3.wzyx * q17.x) + ((q12.x * shades(q10.xyz, -q15.xyz)) * lerp(const_3.wzyx, r3.wzyx, -r3.w));			// partial precision
    q258.xyz = (saturate(((3 - (q18.x * 2)) * sqr(q18.x)) - ((3 - (q17.x * 2)) * sqr(q17.x))) * const_2.xyz) + r1.wzy;			// partial precision
    q44.xyz = (r0.w * q258.xyz) + (((q9.x * const_2.xyz) + q14.xyz) * saturate((1 - att1.x) - att13.x));			// partial precision
    OUT.color_0.a = 1;			// partial precision
    OUT.color_0.rgb = q44.xyz;			// partial precision

    return OUT;
};

// approximately 68 instruction slots used (5 texture, 63 arithmetic)
