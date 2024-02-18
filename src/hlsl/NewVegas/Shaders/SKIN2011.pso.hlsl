//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
float4 EmittanceColor : register(c2);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10] : register(c3);
float4 PSLightPosition[8];
float4 Toggles : register(c27);


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   AmbientColor    const_1       1
//   EmittanceColor  const_2       1
//   PSLightColor[0]    const_3       4
//   PSLightPosition[0] const_19      3
//   Toggles         const_27      1
//   BaseMap         texture_0       1
//   NormalMap       texture_1       1
//   FaceGenMap0     texture_2       1
//   FaceGenMap1     texture_3       1
//


// Structures:

struct VS_INPUT {
    float3 texcoord_3 : TEXCOORD3;
    float3 texcoord_4 : TEXCOORD4;
    float3 texcoord_5 : TEXCOORD5;
    float3 texcoord_6 : TEXCOORD6;
    float2 BaseUV : TEXCOORD0;
    float3 texcoord_1 : TEXCOORD1;
    float3 texcoord_2 : TEXCOORD2;
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
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

    const float4 const_0 = {-0.5, 2, 1, 0.3};
    const float4 const_8 = {0.3, 0, 0, 1};

    float3 noxel4;
    float3 q0;
    float3 q1;
    float1 q10;
    float1 q12;
    float3 q13;
    float3 q14;
    float3 q15;
    float1 q16;
    float1 q17;
    float3 q18;
    float1 q19;
    float3 q2;
    float1 q20;
    float3 q21;
    float1 q22;
    float1 q23;
    float1 q24;
    float3 q25;
    float1 q26;
    float3 q27;
    float3 q28;
    float3 q3;
    float3 q5;
    float3 q56;
    float3 q57;
    float3 q6;
    float3 q60;
    float1 q7;
    float1 q8;
    float1 q9;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float3 r5;
    float3 r8;

    q6.xyz = normalize(IN.texcoord_4.xyz);			// partial precision
    q0.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    r1.xyz = const_0.xyz;
    clip(r2.xyzw);
    noxel4.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r4.xyzw = tex2D(FaceGenMap1, IN.BaseUV.xy);			// partial precision
    r3.xyzw = tex2D(FaceGenMap0, IN.BaseUV.xy);			// partial precision
    r0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    q5.xyz = normalize(expand(noxel4.xyz));			// partial precision
    q7.x = dot(q5.xyz, q6.xyz);
    q8.x = saturate(q7.x);			// partial precision
    q9.x = saturate((q7.x + 0.3) * 0.769230783);
    q3.xyz = (const_21.xyz - IN.texcoord_1.xyz) / const_21.w;			// partial precision
    r5.z = shades(q3.xyz, q3.xyz);			// partial precision
    q2.xyz = (const_20.xyz - IN.texcoord_1.xyz) / const_20.w;			// partial precision
    r5.y = shades(q2.xyz, q2.xyz);			// partial precision
    q21.xyz = normalize(IN.texcoord_6.xyz);			// partial precision
    q22.x = dot(q5.xyz, q21.xyz);
    q23.x = saturate(q22.x);			// partial precision
    q24.x = saturate((q22.x + 0.3) * 0.769230783);
    q26.x = saturate(((3 - (q24.x * 2)) * sqr(q24.x)) - ((3 - (q23.x * 2)) * sqr(q23.x)));			// partial precision
    q27.xyz = 2 * ((expand(r3.xyz) + r0.xyz) * (2 * r4.xyz));			// partial precision	// [0,1] to [-1,+1]
    q1.xyz = (const_19.xyz - IN.texcoord_1.xyz) / const_19.w;			// partial precision
    q12.x = sqr(1 - shades(q5.xyz, q0.xyz));			// partial precision
    q14.xyz = shades(q5.xyz, IN.texcoord_3.xyz) * PSLightColor[0].rgb;			// partial precision
    r5.x = shades(q1.xyz, q1.xyz);			// partial precision
    q10.x = saturate(((3 - (q9.x * 2)) * sqr(q9.x)) - ((3 - (q8.x * 2)) * sqr(q8.x)));			// partial precision
    q15.xyz = normalize(IN.texcoord_5.xyz);			// partial precision
    q16.x = dot(q5.xyz, q15.xyz);
    q17.x = saturate(q16.x);			// partial precision
    q19.x = saturate((q16.x + 0.3) * 0.769230783);
    q20.x = saturate(((3 - (q19.x * 2)) * sqr(q19.x)) - ((3 - (q17.x * 2)) * sqr(q17.x)));			// partial precision
    r5.xyz = 1 - r5.xyz;			// partial precision
    q13.xyz = (q8.x * const_4.xyz) + lerp(const_4.xyz, const_8.xyz, -r1.x) * (shades(q0.xyz, -q6.xyz) * q12.x);			// partial precision
    q25.xyz = (q23.x * const_6.xyz) + ((q12.x * shades(q0.xyz, -q21.xyz)) * lerp(const_6.xyz, const_8.xyz, -r1.x));			// partial precision
    q18.xyz = (q17.x * const_5.xyz) + ((q12.x * shades(q0.xyz, -q15.xyz)) * lerp(const_5.xyz, const_8.xyz, -r1.x));			// partial precision
    q28.xyz = (Toggles.x <= 0.0 ? q27.xyz : (q27.xyz * IN.color_0.rgb));			// partial precision
    r2.xyzw = (AmbientColor.a >= r1.z ? 0 : (r0.w - Toggles.w));
    r2.w = r1.y - EmittanceColor.a;
    r2.xyz = (r1.z >= EmittanceColor.a ? q14.xyz : ((r5.x * ((q10.x * const_8.xyz) + q13.xyz)) + q14.xyz));			// partial precision
    r8.xyz = (r5.y * ((q20.x * const_8.xyz) + q18.xyz)) + r2.xyz;			// partial precision
    r1.yzw = (r2.w >= 0.0 ? r2.wzyx : r8.wzyx);			// partial precision
    q56.xyz = (3 >= EmittanceColor.a ? r1.wzyx : ((r5.z * ((q26.x * const_8.xyz) + q25.xyz)) + r1.wzy));			// partial precision
    r1.w = r0.w * AmbientColor.a;			// partial precision
    q57.xyz = max(q56.xyz + AmbientColor.rgb, 0);			// partial precision
    q60.xyz = q57.xyz * q28.xyz;			// partial precision
    r1.xyz = (Toggles.y <= 0.0 ? q60.xyz : ((IN.color_1.a * (IN.color_1.rgb - (q28.xyz * q57.xyz))) + q60.xyz));			// partial precision
    OUT.color_0.rgba = r1.xyzw;			// partial precision

    return OUT;
};

// approximately 119 instruction slots used (4 texture, 115 arithmetic)
