//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10] : register(c3);
sampler2D ShadowMap : register(s6);
sampler2D ShadowMaskMap : register(s7);
float4 Toggles : register(c27);


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   AmbientColor  const_1       1
//   PSLightColor[0]  const_3       1
//   Toggles       const_27      1
//   BaseMap       texture_0       1
//   NormalMap     texture_1       1
//   FaceGenMap0   texture_2       1
//   FaceGenMap1   texture_3       1
//   ShadowMap     texture_6       1
//   ShadowMaskMap texture_7       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;			// partial precision
    float3 texcoord_6 : TEXCOORD6_centroid;			// partial precision
    float4 texcoord_7 : TEXCOORD7;			// partial precision
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float3 texcoord_1 : TEXCOORD1_centroid;			// partial precision
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

    float3 noxel0;
    float1 q3;
    float3 q48;
    float3 q5;
    float3 q7;
    float3 q8;
    float3 q9;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float1 t15;

    t15.x = tex2D(ShadowMaskMap, IN.texcoord_7.zw);			// partial precision
    r3.xyz = tex2D(ShadowMap, IN.texcoord_7.xy);			// partial precision
    clip(r1.xyzw);
    noxel0.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r5.xyzw = tex2D(FaceGenMap1, IN.BaseUV.xy);			// partial precision
    r4.xyzw = tex2D(FaceGenMap0, IN.BaseUV.xy);			// partial precision
    r0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    r3.w = shades(normalize(expand(noxel0.xyz)), IN.texcoord_1.xyz);			// partial precision
    r2.yzw = r3.wzyx - 1;			// partial precision
    q7.xyz = 2 * ((expand(r4.xyz) + r0.xyz) * (2 * r5.xyz));			// partial precision
    q3.x = 1 - shades(normalize(expand(noxel0.xyz)), normalize(IN.texcoord_6.xyz));			// partial precision
    q48.xyz = (shades(normalize(IN.texcoord_6.xyz), -IN.texcoord_1) * sqr(q3.x) * PSLightColor[0].rgb) * 0.5;			// partial precision
    q8.xyz = (Toggles.x <= 0.0 ? q7.xyz : (q7.xyz * IN.color_0.rgb));			// partial precision
    r1.xyzw = (AmbientColor.a >= 1 ? 0 : (r0.w - Toggles.w));
    r1.w = r0.w * AmbientColor.a;			// partial precision
    q5.xyz = (((t15.x * r2.wzy) + 1) * ((r3.w * PSLightColor[0].rgb) + q48.xyz)) + AmbientColor.rgb;			// partial precision
    q9.xyz = max(q5.xyz, 0) * q8.xyz;			// partial precision
    r1.xyz = (Toggles.y <= 0.0 ? q9.xyz : ((IN.color_1.a * (IN.color_1.rgb - (q8.xyz * max(q5.xyz, 0)))) + q9.xyz));			// partial precision
    OUT.color_0.rgba = r1.xyzw;			// partial precision

    return OUT;
};

// approximately 48 instruction slots used (6 texture, 42 arithmetic)
