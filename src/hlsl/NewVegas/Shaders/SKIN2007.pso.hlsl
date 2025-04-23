//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D AttenuationMap : register(s4);
sampler2D BaseMap : register(s0);
sampler2D GlowMap : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10];
sampler2D ShadowMap : register(s5);
sampler2D ShadowMaskMap : register(s6);


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
//   ShadowMap      texture_5       1
//   ShadowMaskMap  texture_6       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float3 texcoord_3 : TEXCOORD3_centroid;			// partial precision
    float2 BaseUV : TEXCOORD0;			// partial precision
    float3 texcoord_7 : TEXCOORD7_centroid;			// partial precision
    float4 texcoord_6 : TEXCOORD6;			// partial precision
    float4 texcoord_4 : TEXCOORD4;			// partial precision
    float4 texcoord_5 : TEXCOORD5;			// partial precision
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
    float1 att4;
    float1 att46;
    float3 noxel2;
    float1 q10;
    float1 q11;
    float1 q13;
    float3 q14;
    float3 q16;
    float1 q18;
    float1 q19;
    float1 q20;
    float1 q21;
    float3 q39;
    float3 q5;
    float3 q6;
    float3 q7;
    float1 q8;
    float1 q9;
    float4 r0;
    float4 r1;
    float3 r3;
    float4 r6;
    float3 r8;
    float1 t37;
    float4 texel0;

    t37.x = tex2D(ShadowMaskMap, IN.texcoord_6.zw);			// partial precision
    r3.xyz = tex2D(ShadowMap, IN.texcoord_6.xy);			// partial precision
    noxel2.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r6.xyzw = tex2D(GlowMap, IN.BaseUV.xy);			// partial precision
    texel0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    OUT.color_0.rgba = texel0.xyzw;			// partial precision
    att4.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    att1.x = tex2D(AttenuationMap, IN.texcoord_5.xy);			// partial precision
    att46.x = tex2D(AttenuationMap, IN.texcoord_5.zw);			// partial precision
    att3.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    r0.yzw = r3.wzyx - 1;			// partial precision
    q6.xyz = normalize(expand(noxel2.xyz));			// partial precision
    q5.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    q13.x = sqr(1 - shades(q6.xyz, q5.xyz));			// partial precision
    r6.w = q13.x * shades(q5.xyz, -normalize(IN.texcoord_3.xyz));			// partial precision
    q7.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    q8.x = dot(q6.xyz, q7.xyz);
    q9.x = saturate(q8.x);			// partial precision
    q10.x = saturate((q8.x + 0.3) * 0.769230783);
    q11.x = saturate(((3 - (q10.x * 2)) * sqr(q10.x)) - ((3 - (q9.x * 2)) * sqr(q9.x)));			// partial precision
    q18.x = dot(q6.xyz, normalize(IN.texcoord_3.xyz));
    q19.x = saturate(q18.x);			// partial precision
    q20.x = saturate((q18.x + 0.3) * 0.769230783);
    q21.x = saturate(((3 - (q20.x * 2)) * sqr(q20.x)) - ((3 - (q19.x * 2)) * sqr(q19.x)));			// partial precision
    r0.xyz = (t37.x * r0.wzy) + 1;			// partial precision
    r0.w = shades(q6.xyz, IN.texcoord_1.xyz);			// partial precision
    q39.xyz = (r0.w * const_3.xyz) + (((q13.x * shades(q5.xyz, -IN.texcoord_1)) * const_3.xyz) * 0.5);			// partial precision
    q14.xyz = (q9.x * const_4.xyz) + ((shades(q5.xyz, -q7.xyz) * q13.x) * lerp(const_4.xyz, r6.xyz, 0.5));			// partial precision
    q16.xyz = (r0.xyz * q39.xyz) + saturate((1 - att3.x) - att4.x) * ((q11.x * r6.xyz) + q14.xyz);			// partial precision
    r1.yzw = (const_5.wzyx * q19.x) + (r6.w * lerp(const_5.wzyx, r6.wzyx, 0.5));			// partial precision
    r8.xyz = ((saturate((1 - att1.x) - att46.x) * ((q21.x * r6.xyz) + r1.wzy)) + q16.xyz) + AmbientColor.rgb;			// partial precision

    return OUT;
};

// approximately 82 instruction slots used (9 texture, 73 arithmetic)
