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
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;			// partial precision
    float3 texcoord_6 : TEXCOORD6_centroid;			// partial precision
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

    const float4 const_0 = {-0.5, 2, 1, 0.5};

    float1 att1;
    float1 att28;
    float3 noxel2;
    float3 q0;
    float1 q10;
    float3 q12;
    float3 q13;
    float3 q14;
    float3 q15;
    float3 q16;
    float3 q3;
    float3 q31;
    float3 q4;
    float1 q5;
    float1 q6;
    float1 q7;
    float1 q8;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r6;
    float4 r7;

    noxel2.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r3.xyzw = tex2D(GlowMap, IN.BaseUV.xy);			// partial precision
    r7.xyzw = tex2D(FaceGenMap1, IN.BaseUV.xy);			// partial precision
    r6.xyzw = tex2D(FaceGenMap0, IN.BaseUV.xy);			// partial precision
    r0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    q3.xyz = normalize(expand(noxel2.xyz));			// partial precision
    q0.xyz = normalize(IN.texcoord_6.xyz);			// partial precision
    q10.x = sqr(1 - shades(q3.xyz, q0.xyz));			// partial precision
    q4.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    q5.x = dot(q3.xyz, q4.xyz);
    q6.x = saturate(q5.x);			// partial precision
    q7.x = saturate((q5.x + 0.3) * 0.769230783);
    q8.x = saturate(((3 - (q7.x * 2)) * sqr(q7.x)) - ((3 - (q6.x * 2)) * sqr(q6.x)));			// partial precision
    r1.zw = const_0.zw;
    q14.xyz = 2 * ((expand(r6.xyz) + r0.xyz) * (2 * r7.xyz));			// partial precision
    q15.xyz = (Toggles.x <= 0.0 ? q14.xyz : (q14.xyz * IN.color_0.rgb));			// partial precision
    r2.xyzw = (AmbientColor.a >= r1.z ? 0 : (r0.w - Toggles.w));
    r2.xyz = lerp(const_4.xyz, r3.xyz, r1.w);			// partial precision
    r1.w = r0.w * AmbientColor.a;			// partial precision
    q31.xyz = (q8.x * r3.xyz) + ((q6.x * const_4.xyz) + ((q10.x * shades(q0.xyz, -q4.xyz)) * r2.xyz));			// partial precision
    r1.xyz = ((q10.x * shades(q0.xyz, -IN.texcoord_1)) * const_3.xyz) * 0.5;			// partial precision
    clip(r2.xyzw);
    att28.x = tex2D(AttenuationMap, IN.texcoord_4.zw);			// partial precision
    att1.x = tex2D(AttenuationMap, IN.texcoord_4.xy);			// partial precision
    q12.xyz = (saturate((1 - att1.x) - att28.x) * q31.xyz) + ((shades(q3.xyz, IN.texcoord_1.xyz) * const_3.xyz) + r1.xyz);			// partial precision
    q13.xyz = max(q12.xyz + AmbientColor.rgb, 0);			// partial precision
    q16.xyz = q13.xyz * q15.xyz;			// partial precision
    r1.xyz = (Toggles.y <= 0.0 ? q16.xyz : ((IN.color_1.a * (IN.color_1.rgb - (q15.xyz * q13.xyz))) + q16.xyz));			// partial precision
    OUT.color_0.rgba = r1.xyzw;			// partial precision

    return OUT;
};

// approximately 69 instruction slots used (7 texture, 62 arithmetic)
