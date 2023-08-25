//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D AttenuationMap : register(s4);
sampler2D BaseMap : register(s0);
sampler2D GlowMap : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10];


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   AmbientColor   const_1       1
//   PSLightColor[0]   const_3       3
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   GlowMap        texture_3       1
//   AttenuationMap texture_4       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float3 texcoord_3 : TEXCOORD3_centroid;			// partial precision
    float4 texcoord_4 : TEXCOORD4;			// partial precision
    float4 texcoord_5 : TEXCOORD5;			// partial precision
    float2 BaseUV : TEXCOORD0;			// partial precision
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

    float1 att1;
    float1 att3;
    float1 att39;
    float1 att4;
    float3 noxel2;
    float1 q10;
    float1 q11;
    float1 q13;
    float1 q16;
    float1 q17;
    float1 q18;
    float1 q19;
    float3 q40;
    float3 q5;
    float3 q6;
    float1 q8;
    float1 q9;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r4;
    float4 r5;
    float3 r6;
    float4 texel0;

    noxel2.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r4.xyzw = tex2D(GlowMap, IN.BaseUV.xy);			// partial precision
    texel0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    OUT.color_0.rgba = texel0.xyzw;			// partial precision
    att4.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    att3.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    att1.x = tex2D(AttenuationMap, IN.texcoord_5.xy);			// partial precision
    att39.x = tex2D(AttenuationMap, IN.texcoord_5.zw);			// partial precision
    r2.w = saturate((1 - att1.x) - att39.x);			// partial precision
    q6.xyz = normalize(expand(noxel2.xyz));			// partial precision
    q5.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    q13.x = sqr(1 - shades(q6.xyz, q5.xyz));			// partial precision
    r4.w = shades(q5.xyz, -normalize(IN.texcoord_2.xyz)) * q13.x;			// partial precision
    q16.x = dot(q6.xyz, normalize(IN.texcoord_3.xyz));
    q18.x = saturate(q16.x);			// partial precision
    q17.x = saturate((q16.x + 0.3) * 0.769230783);
    q19.x = saturate(((3 - (q17.x * 2)) * sqr(q17.x)) - ((3 - (q18.x * 2)) * sqr(q18.x)));			// partial precision
    q8.x = dot(q6.xyz, normalize(IN.texcoord_2.xyz));
    q9.x = saturate(q8.x);			// partial precision
    q10.x = saturate((q8.x + 0.3) * 0.769230783);
    q11.x = saturate(((3 - (q10.x * 2)) * sqr(q10.x)) - ((3 - (q9.x * 2)) * sqr(q9.x)));			// partial precision
    r2.xyz = (q13.x * shades(q5.xyz, -normalize(IN.texcoord_3.xyz))) * lerp(const_5.xyz, r4.xyz, 0.5);			// partial precision
    q40.xyz = (q11.x * r4.wzyx) + ((const_4.wzyx * q9.x) + (r4.w * lerp(const_4.wzyx, r4.wzyx, 0.5)));			// partial precision
    r0.yzw = (const_5.wzyx * q18.x) + r2.wzyx;			// partial precision
    r5.yzw = ((q13.x * shades(q5.xyz, -IN.texcoord_1)) * const_3.wzyx) * 0.5;			// partial precision
    r1.yzw = (saturate((1 - att3.x) - att4.x) * q40.xyz) + ((const_3.wzyx * shades(q6.xyz, IN.texcoord_1.xyz)) + r5.yzw);			// partial precision
    r6.xyz = ((r2.w * ((q19.x * r4.xyz) + r0.wzy)) + r1.wzy) + AmbientColor.rgb;			// partial precision

    return OUT;
};

// approximately 75 instruction slots used (7 texture, 68 arithmetic)
