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
//   PSLightColor[0]   const_3       3
//   NormalMap      texture_0       1
//   AttenuationMap texture_3       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float3 texcoord_3 : TEXCOORD3_centroid;			// partial precision
    float4 texcoord_4 : TEXCOORD4;			// partial precision
    float4 texcoord_5 : TEXCOORD5;			// partial precision
    float4 texcoord_6 : TEXCOORD6;			// partial precision
    float2 NormalUV : TEXCOORD0;			// partial precision
    float3 texcoord_7 : TEXCOORD7_centroid;			// partial precision
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

#define	shade(n, l)		max(dot(n, l), 0)
#define	shades(n, l)		saturate(dot(n, l))
#define	weight(v)		dot(v, 1)
#define	sqr(v)			((v) * (v))

    const float4 const_2 = {0.3, 0, 0, 0};

    float1 att1;
    float1 att2;
    float1 att3;
    float1 att4;
    float1 att5;
    float1 att51;
    float3 q0;
    float1 q10;
    float1 q11;
    float1 q12;
    float3 q14;
    float1 q15;
    float1 q16;
    float3 q17;
    float1 q18;
    float1 q21;
    float1 q22;
    float1 q23;
    float1 q24;
    float3 q32;
    float3 q6;
    float3 q7;
    float1 q8;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r4;
    float4 r5;

    r5.xyz = tex2D(NormalMap, IN.NormalUV.xy);			// partial precision
    att4.x = tex2D(AttenuationMap, IN.texcoord_5.zw);			// partial precision
    att2.x = tex2D(AttenuationMap, IN.texcoord_5.xy);			// partial precision
    att5.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    att1.x = tex2D(AttenuationMap, IN.texcoord_6.xy);			// partial precision
    att51.x = tex2D(AttenuationMap, IN.texcoord_6.zw);			// partial precision
    att3.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    r5.w = saturate((1 - att3.x) - att5.x);			// partial precision
    r0.yzw = r5.wzyx - 0.5;
    q6.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    q7.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    q0.xyz = normalize(IN.texcoord_1.xyz);			// partial precision
    q32.xyz = normalize(2 * r0.wzy);			// partial precision
    q10.x = sqr(1 - shades(q32.xyz, q6.xyz));			// partial precision
    q8.x = dot(q32.xyz, q7.xyz);
    q11.x = saturate(q8.x);			// partial precision
    q12.x = saturate((q8.x + 0.3) * 0.769230783);
    q15.x = dot(q32.xyz, q0.xyz);
    q16.x = saturate(q15.x);			// partial precision
    q18.x = saturate((q15.x + 0.3) * 0.769230783);
    q21.x = dot(q32.xyz, normalize(IN.texcoord_3.xyz));
    q22.x = saturate(q21.x);			// partial precision
    q23.x = saturate((q21.x + 0.3) * 0.769230783);
    q24.x = saturate(((3 - (q23.x * 2)) * sqr(q23.x)) - ((3 - (q22.x * 2)) * sqr(q22.x)));			// partial precision
    r4.xyz = const_2.xyz;			// partial precision
    r4.w = -0.5;
    q14.xyz = (const_4.wzyx * q11.x) + lerp(const_4.wzyx, r4.wzyx, -r4.w) * (shades(q6.xyz, -q7.xyz) * q10.x);			// partial precision
    r0.yzw = (saturate(((3 - (q12.x * 2)) * sqr(q12.x)) - ((3 - (q11.x * 2)) * sqr(q11.x))) * const_2.wzyx) + q14.xyz;			// partial precision
    q17.xyz = (const_3.wzyx * q16.x) + ((q10.x * shades(q6.xyz, -q0.xyz)) * lerp(const_3.wzyx, r4.wzyx, -r4.w));			// partial precision
    r2.yzw = (saturate(((3 - (q18.x * 2)) * sqr(q18.x)) - ((3 - (q16.x * 2)) * sqr(q16.x))) * const_2.wzyx) + q17.xyz;			// partial precision
    r0.xyz = (r2.wzy * r5.w) + (saturate((1 - att2.x) - att4.x) * r0.wzy);			// partial precision
    r2.w = shades(q6.xyz, -normalize(IN.texcoord_3.xyz));			// partial precision
    r2.xyz = lerp(const_5.xyz, r4.xyz, -r4.w);			// partial precision
    r1.yzw = (const_5.wzyx * q22.x) + ((q10.x * r2.w) * r2.wzyx);			// partial precision
    OUT.color_0.a = 1;			// partial precision
    OUT.color_0.rgb = (saturate((1 - att1.x) - att51.x) * ((q24.x * const_2.xyz) + r1.wzy)) + r0.xyz;			// partial precision

    return OUT;
};

// approximately 94 instruction slots used (7 texture, 87 arithmetic)
