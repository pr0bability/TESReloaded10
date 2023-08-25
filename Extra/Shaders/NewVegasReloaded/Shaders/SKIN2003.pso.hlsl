//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D AttenuationMap : register(s5);
sampler2D BaseMap : register(s0);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D GlowMap : register(s4);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10];
sampler2D ShadowMap : register(s6);
sampler2D ShadowMaskMap : register(s7);
float4 Toggles : register(c27);


// Registers:
//
//   Name           Reg   Size
//   -------------- ----- ----
//   AmbientColor   const_1       1
//   PSLightColor[0]   const_3       2
//   Toggles        const_27      1
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   FaceGenMap0    texture_2       1
//   FaceGenMap1    texture_3       1
//   GlowMap        texture_4       1
//   AttenuationMap texture_5       1
//   ShadowMap      texture_6       1
//   ShadowMaskMap  texture_7       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;			// partial precision
    float3 texcoord_6 : TEXCOORD6_centroid;			// partial precision
    float4 texcoord_7 : TEXCOORD7;			// partial precision
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float4 texcoord_4 : TEXCOORD4;			// partial precision
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

    const float4 const_0 = {-0.5, 2, -1, 1};

    float1 att2;
    float1 att3;
    float1 q10;
    float1 q12;
    float3 q14;
    float3 q15;
    float3 q16;
    float3 q33;
    float3 q34;
    float3 q35;
    float3 q4;
    float3 q5;
    float3 q6;
    float1 q7;
    float1 q8;
    float1 q9;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r5;
    float4 r8;
    float4 r9;
    float3 t0;
    float1 t1;

    t1.x = tex2D(ShadowMaskMap, IN.texcoord_7.zw);			// partial precision
    t0.xyz = tex2D(ShadowMap, IN.texcoord_7.xy);			// partial precision
    r5.xyzw = tex2D(GlowMap, IN.BaseUV.xy);			// partial precision
    r9.xyzw = tex2D(FaceGenMap1, IN.BaseUV.xy);			// partial precision
    r8.xyzw = tex2D(FaceGenMap0, IN.BaseUV.xy);			// partial precision
    r0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    q4.xyz = normalize(IN.texcoord_6.xyz);			// partial precision
    q6.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    r1.xz = const_0.xz;
    q15.xyz = 2 * ((expand(r8.xyz) + r0.xyz) * (2 * r9.xyz));			// partial precision
    q16.xyz = (Toggles.x <= 0.0 ? q15.xyz : (q15.xyz * IN.color_0.rgb));			// partial precision
    r2.xyzw = ((r1.z + AmbientColor.a) >= 0.0 ? 0 : 1) * (r0.w - Toggles.w);
    r2.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r1.yzw = r2.wzyx - 0.5;
    q5.xyz = normalize(2 * r1.wzy);			// partial precision
    q12.x = sqr(1 - shades(q5.xyz, q4.xyz));			// partial precision
    q7.x = dot(q5.xyz, q6.xyz);
    q8.x = saturate(q7.x);			// partial precision
    q9.x = saturate((q7.x + 0.3) * 0.769230783);
    q10.x = saturate(((3 - (q9.x * 2)) * sqr(q9.x)) - ((3 - (q8.x * 2)) * sqr(q8.x)));			// partial precision
    q14.xyz = (q8.x * const_4.xyz) + ((q12.x * shades(q4.xyz, -q6.xyz)) * lerp(const_4.xyz, r5.xyz, -r1.x));			// partial precision
    r1.yzw = ((q12.x * shades(q4.xyz, -IN.texcoord_1)) * const_3.wzyx) * 0.5;			// partial precision
    r1.yzw = (const_3.wzyx * shades(q5.xyz, IN.texcoord_1.xyz)) + r1.yzw;			// partial precision
    clip(r2.xyzw);
    att3.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    att2.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    q33.xyz = (r1.wzy * ((t1.x * (t0.xyz - 1)) + 1)) + (((q10.x * r5.xyz) + q14.xyz) * saturate((1 - att2.x) - att3.x));			// partial precision
    r1.w = r0.w * AmbientColor.a;			// partial precision
    q34.xyz = max(q33.xyz + AmbientColor.rgb, 0);			// partial precision
    q35.xyz = q34.xyz * q16.xyz;			// partial precision
    r1.xyz = (Toggles.y <= 0.0 ? q35.xyz : ((IN.color_1.a * (IN.color_1.rgb - (q16.xyz * q34.xyz))) + q35.xyz));			// partial precision
    OUT.color_0.rgba = r1.xyzw;			// partial precision

    return OUT;
};

// approximately 76 instruction slots used (9 texture, 67 arithmetic)
