//
//
// Parameters:

float4 AmbientColor : register(c1);
sampler2D BaseMap : register(s0);
float4 EmittanceColor : register(c2);
sampler2D FaceGenMap0 : register(s2);
sampler2D FaceGenMap1 : register(s3);
sampler2D NormalMap : register(s1);
float4 PSLightColor[10];
float4 PSLightPosition[8] : register(c19);
float4 Toggles : register(c27);


// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   AmbientColor    const_1       1
//   EmittanceColor  const_2       1
//   PSLightColor[0]    const_3       5
//   PSLightPosition[0] const_19      4
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
    float3 texcoord_7 : TEXCOORD7;
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
    const float4 const_8 = {0.769230783, -2, 3, 4};
    const float4 const_9 = {0.3, 0, 0, 1};

    float3 lit1;
    float3 noxel6;
    float3 q0;
    float1 q10;
    float1 q11;
    float1 q13;
    float3 q14;
    float3 q15;
    float3 q16;
    float1 q17;
    float3 q18;
    float1 q19;
    float3 q2;
    float1 q20;
    float3 q21;
    float1 q22;
    float1 q23;
    float3 q24;
    float1 q25;
    float1 q26;
    float3 q27;
    float1 q28;
    float1 q29;
    float3 q3;
    float3 q30;
    float1 q31;
    float1 q32;
    float3 q33;
    float3 q34;
    float3 q4;
    float3 q5;
    float3 q68;
    float3 q69;
    float3 q7;
    float3 q72;
    float1 q8;
    float1 q9;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float4 r4;
    float4 r5;
    float4 r6;
    float3 r8;

    noxel6.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r4.xyzw = tex2D(FaceGenMap1, IN.BaseUV.xy);			// partial precision
    r3.xyzw = tex2D(FaceGenMap0, IN.BaseUV.xy);			// partial precision
    r0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    q7.xyz = normalize(expand(noxel6.xyz));			// partial precision
    q15.xyz = shades(q7.xyz, IN.texcoord_3.xyz) * const_3.xyz;			// partial precision
    q16.xyz = normalize(IN.texcoord_5.xyz);			// partial precision
    q5.xyz = normalize(IN.texcoord_4.xyz);			// partial precision
    q0.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    q13.x = sqr(1 - shades(q7.xyz, q0.xyz));			// partial precision
    q8.x = dot(q7.xyz, q5.xyz);
    q9.x = saturate(q8.x);			// partial precision
    q10.x = saturate((q8.x + 0.3) * 0.769230783);
    q11.x = saturate(((3 - (q10.x * 2)) * sqr(q10.x)) - ((3 - (q9.x * 2)) * sqr(q9.x)));			// partial precision
    q17.x = dot(q7.xyz, q16.xyz);
    r6.w = saturate(q17.x);			// partial precision
    q19.x = saturate((q17.x + 0.3) * 0.769230783);
    r1.xyz = const_0.xyz;
    q18.xyz = (r6.w * const_5.xyz) + ((q13.x * shades(q0.xyz, -q16.xyz)) * lerp(const_5.xyz, const_9.xyz, -r1.x));			// partial precision
    q14.xyz = (q9.x * const_4.xyz) + lerp(const_4.xyz, const_9.xyz, -r1.x) * (shades(q0.xyz, -q5.xyz) * q13.x);			// partial precision
    q4.xyz = (const_22.xyz - IN.texcoord_1.xyz) / const_22.w;			// partial precision
    r5.w = shades(q4.xyz, q4.xyz);			// partial precision
    q3.xyz = (const_21.xyz - IN.texcoord_1.xyz) / const_21.w;			// partial precision
    r5.z = shades(q3.xyz, q3.xyz);			// partial precision
    q33.xyz = 2 * ((expand(r3.xyz) + r0.xyz) * (2 * r4.xyz));			// partial precision	// [0,1] to [-1,+1]
    q2.xyz = (const_20.xyz - IN.texcoord_1.xyz) / const_20.w;			// partial precision
    r5.y = shades(q2.xyz, q2.xyz);			// partial precision
    q20.x = saturate(((3 - (q19.x * 2)) * sqr(q19.x)) - ((3 - (r6.w * 2)) * sqr(r6.w)));			// partial precision
    q27.xyz = normalize(IN.texcoord_7.xyz);			// partial precision
    q21.xyz = normalize(IN.texcoord_6.xyz);			// partial precision
    q28.x = dot(q7.xyz, q27.xyz);
    q29.x = saturate(q28.x);			// partial precision
    q31.x = saturate((q28.x + 0.3) * 0.769230783);
    q32.x = saturate(((3 - (q31.x * 2)) * sqr(q31.x)) - ((3 - (q29.x * 2)) * sqr(q29.x)));			// partial precision
    q22.x = dot(q7.xyz, q21.xyz);
    q23.x = saturate(q22.x);			// partial precision
    q25.x = saturate((q22.x + 0.3) * 0.769230783);
    q26.x = saturate(((3 - (q25.x * 2)) * sqr(q25.x)) - ((3 - (q23.x * 2)) * sqr(q23.x)));			// partial precision
    q30.xyz = (q29.x * const_7.xyz) + ((q13.x * shades(q0.xyz, -q27.xyz)) * lerp(const_7.xyz, const_9.xyz, -r1.x));			// partial precision
    q24.xyz = (q23.x * const_6.xyz) + ((q13.x * shades(q0.xyz, -q21.xyz)) * lerp(const_6.xyz, const_9.xyz, -r1.x));			// partial precision
    q34.xyz = (Toggles.x <= 0.0 ? q33.xyz : (q33.xyz * IN.color_0.rgb));			// partial precision
    r2.xyzw = (AmbientColor.a >= r1.z ? 0 : (r0.w - Toggles.w));
    clip(r2.xyzw);
    lit1.xyz = (PSLightPosition[0].xyz - IN.texcoord_1.xyz) / PSLightPosition[0].w;			// partial precision
    r5.x = shades(lit1.xyz, lit1.xyz);			// partial precision
    r5.xyzw = 1 - r5.xyzw;			// partial precision
    r6.xyz = (r1.z >= EmittanceColor.a ? q15.xyz : ((r5.x * ((q11.x * const_9.xyz) + q14.xyz)) + q15.xyz));			// partial precision
    r8.xyz = (r5.y * ((q20.x * const_9.xyz) + q18.xyz)) + r6.xyz;			// partial precision
    r1.yzw = (r1.y >= EmittanceColor.a ? r6.wzyx : r8.wzyx);			// partial precision
    r6.zw = const_8.zw;
    r5.xyz = (r5.z * ((q26.x * const_9.xyz) + q24.xyz)) + r1.wzy;			// partial precision
    r1.yzw = (r6.z >= EmittanceColor.a ? r1.yzw : r5.wzyx);			// partial precision
    q68.xyz = (r6.w >= EmittanceColor.a ? r1.wzyx : ((r5.w * ((q32.x * const_9.xyz) + q30.xyz)) + r1.wzy));			// partial precision
    r1.w = r0.w * AmbientColor.a;			// partial precision
    q69.xyz = max(q68.xyz + AmbientColor.rgb, 0);			// partial precision
    q72.xyz = q69.xyz * q34.xyz;			// partial precision
    r1.xyz = (Toggles.y <= 0.0 ? q72.xyz : ((IN.color_1.a * (IN.color_1.rgb - (q34.xyz * q69.xyz))) + q72.xyz));			// partial precision
    OUT.color_0.rgba = r1.xyzw;			// partial precision

    return OUT;
};

// approximately 145 instruction slots used (4 texture, 141 arithmetic)
