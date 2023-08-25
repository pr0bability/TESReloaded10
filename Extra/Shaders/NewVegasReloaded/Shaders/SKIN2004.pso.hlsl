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
//   PSLightColor[0]   const_3       2
//   BaseMap        texture_0       1
//   NormalMap      texture_1       1
//   GlowMap        texture_3       1
//   AttenuationMap texture_4       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
    float3 texcoord_2 : TEXCOORD2_centroid;			// partial precision
    float2 BaseUV : TEXCOORD0;			// partial precision
    float3 texcoord_7 : TEXCOORD7_centroid;			// partial precision
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

    float1 att11;
    float1 att12;
    float3 noxel1;
    float1 q10;
    float3 q14;
    float3 q2;
    float1 q4;
    float1 q5;
    float1 q6;
    float1 q7;
    float3 q76;
    float3 q8;
    float3 r0;
    float4 r1;
    float3 r2;
    float3 r4;
    float4 texel0;

    noxel1.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r1.xyzw = tex2D(GlowMap, IN.BaseUV.xy);			// partial precision
    texel0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    OUT.color_0.rgba = texel0.xyzw;			// partial precision
    att12.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    att11.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    q2.xyz = normalize(expand(noxel1.xyz));			// partial precision
    q8.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    q10.x = sqr(1 - shades(q2.xyz, q8.xyz));			// partial precision
    q4.x = dot(q2.xyz, normalize(IN.texcoord_2.xyz));
    q5.x = saturate(q4.x);			// partial precision
    q6.x = saturate((q4.x + 0.3) * 0.769230783);
    q7.x = saturate(((3 - (q6.x * 2)) * sqr(q6.x)) - ((3 - (q5.x * 2)) * sqr(q5.x)));			// partial precision
    q76.xyz = (q10.x * shades(q8.xyz, -normalize(IN.texcoord_2.xyz))) * lerp(const_4.xyz, r1.xyz, 0.5);			// partial precision
    r0.xyz = (q7.x * r1.xyz) + ((q5.x * const_4.xyz) + q76.xyz);			// partial precision
    r2.xyz = ((q10.x * shades(q8.xyz, -IN.texcoord_1)) * const_3.xyz) * 0.5;			// partial precision
    q14.xyz = (saturate((1 - att11.x) - att12.x) * r0.xyz) + ((shades(q2.xyz, IN.texcoord_1.xyz) * const_3.xyz) + r2.xyz);			// partial precision
    r4.xyz = q14.xyz + AmbientColor.rgb;			// partial precision

    return OUT;
};

// approximately 49 instruction slots used (5 texture, 44 arithmetic)
