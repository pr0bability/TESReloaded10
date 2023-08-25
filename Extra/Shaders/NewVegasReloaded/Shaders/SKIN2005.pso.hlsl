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
//   PSLightColor[0]   const_3       2
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   GlowMap        texture_3       1
//   AttenuationMap texture_4       1
//   ShadowMap      texture_5       1
//   ShadowMaskMap  texture_6       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;			// partial precision
    float3 texcoord_7 : TEXCOORD7_centroid;			// partial precision
    float4 texcoord_6 : TEXCOORD6;			// partial precision
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

    float1 att14;
    float1 att2;
    float3 noxel3;
    float3 q10;
    float1 q12;
    float3 q17;
    float3 q4;
    float1 q6;
    float1 q7;
    float1 q8;
    float1 q9;
    float3 q91;
    float3 r0;
    float4 r1;
    float3 r2;
    float3 r6;
    float3 t1;
    float1 t13;
    float4 texel0;

    t1.xyz = tex2D(ShadowMap, IN.texcoord_6.xy);			// partial precision
    t13.x = tex2D(ShadowMaskMap, IN.texcoord_6.zw);			// partial precision
    noxel3.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r1.xyzw = tex2D(GlowMap, IN.BaseUV.xy);			// partial precision
    texel0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    OUT.color_0.rgba = texel0.xyzw;			// partial precision
    att2.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    att14.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    q4.xyz = normalize(expand(noxel3.xyz));			// partial precision
    q10.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    q12.x = sqr(1 - shades(q4.xyz, q10.xyz));			// partial precision
    q6.x = dot(q4.xyz, normalize(IN.texcoord_2.xyz));
    q7.x = saturate(q6.x);			// partial precision
    q8.x = saturate((q6.x + 0.3) * 0.769230783);
    q9.x = saturate(((3 - (q8.x * 2)) * sqr(q8.x)) - ((3 - (q7.x * 2)) * sqr(q7.x)));			// partial precision
    q91.xyz = (q12.x * shades(q10.xyz, -normalize(IN.texcoord_2.xyz))) * lerp(const_4.xyz, r1.xyz, 0.5);			// partial precision
    r0.xyz = (q9.x * r1.xyz) + ((q7.x * const_4.xyz) + q91.xyz);			// partial precision
    r2.xyz = ((q12.x * shades(q10.xyz, -IN.texcoord_1)) * const_3.xyz) * 0.5;			// partial precision
    r2.xyz = (shades(q4.xyz, IN.texcoord_1.xyz) * const_3.xyz) + r2.xyz;			// partial precision
    q17.xyz = (((t13.x * (t1.xyz - 1)) + 1) * r2.xyz) + (r0.xyz * saturate((1 - att2.x) - att14.x));			// partial precision
    r6.xyz = q17.xyz + AmbientColor.rgb;			// partial precision

    return OUT;
};

// approximately 56 instruction slots used (7 texture, 49 arithmetic)
