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
//   PSLightColor[0]    const_3       2
//   PSLightPosition[0] const_19      1
//   Toggles         const_27      1
//   BaseMap         texture_0       1
//   NormalMap       texture_1       1
//   FaceGenMap0     texture_2       1
//   FaceGenMap1     texture_3       1
//


// Structures:

struct VS_INPUT {
    float2 BaseUV : TEXCOORD0;
    float4 texcoord_1 : TEXCOORD1;
    float3 texcoord_2 : TEXCOORD2;
    float3 color_0 : COLOR0;
    float4 color_1 : COLOR1;
    float3 texcoord_3 : TEXCOORD3;
    float3 texcoord_4 : TEXCOORD4;
    float3 texcoord_5 : TEXCOORD5;
    float3 texcoord_6 : TEXCOORD6;
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

    const float4 const_0 = {-0.5, 2, 1, 0.2};
    const float4 const_6 = {0.3, 0, 0, 1};

    float3 lit2;
    float3 noxel0;
    float3 q1;
    float1 q10;
    float3 q11;
    float3 q23;
    float3 q25;
    float1 q3;
    float3 q4;
    float1 q5;
    float3 q52;
    float3 q6;
    float1 q7;
    float1 q8;
    float1 q9;
    float4 r0;
    float4 r1;
    float4 r2;
    float4 r3;
    float3 r4;
    float4 r5;

    r4.xyz = normalize(IN.texcoord_6.xyz);			// partial precision
    q4.xyz = normalize(IN.texcoord_4.xyz);			// partial precision
    q6.xyz = normalize(IN.texcoord_2.xyz);			// partial precision
    clip(r2.xyzw);
    r1.xz = const_0.xz;
    lit2.xyz = (PSLightPosition[0].xyz - IN.texcoord_1.xyz) / PSLightPosition[0].w;			// partial precision
    q3.x = 1 - shades(lit2.xyz, lit2.xyz);			// partial precision
    log r3.x, r5.w			// partial precision
    noxel0.xyz = tex2D(NormalMap, IN.BaseUV.xy);			// partial precision
    r3.xyzw = tex2D(FaceGenMap1, IN.BaseUV.xy);			// partial precision
    r0.xyzw = tex2D(BaseMap, IN.BaseUV.xy);			// partial precision
    q1.xyz = normalize(expand(noxel0.xyz));			// partial precision
    r5.w = shades(q1.xyz, normalize(IN.texcoord_5.xyz));			// partial precision
    r5.w = shades(q1.xyz, r4.xyz);			// partial precision
    r4.y = exp2(r3.y);			// partial precision
    r4.x = exp2(r3.x);			// partial precision
    q5.x = dot(q1.xyz, q4.xyz);
    q7.x = saturate(q5.x);			// partial precision
    q8.x = saturate((q5.x + 0.3) * 0.769230783);
    q10.x = saturate(((3 - (q8.x * 2)) * sqr(q8.x)) - ((3 - (q7.x * 2)) * sqr(q7.x)));			// partial precision
    q9.x = 1 - shades(q1.xyz, q6.xyz);			// partial precision
    q23.xyz = (sqr(q9.x) * shades(q6.xyz, -q4.xyz)) * (const_4.xyz - (r1.x * (const_6.xyz - const_4.xyz)));			// partial precision
    r2.xyzw = (AmbientColor.a >= r1.z ? 0 : (r0.w - Toggles.w));
    r2.xyzw = tex2D(FaceGenMap0, IN.BaseUV.xy);			// partial precision
    r2.w = r0.w * AmbientColor.a;			// partial precision
    r0.xyz = 2 * ((expand(r2.xyz) + r0.xyz) * (2 * r3.xyz));			// partial precision
    r3.xy = (    log r3.y, r5.w			// partial precision
) * Toggles.z;			// partial precision
    r3.xy = r4.w * r4.xy;			// partial precision
    r3.y = q3.x * r3.y;			// partial precision
    r4.y = dot(q1.xyz, IN.texcoord_4.xyz);			// partial precision
    r4.x = dot(q1.xyz, IN.texcoord_3.xyz);			// partial precision
    q52.xyz = saturate(r4.x) * const_3.xyz;			// partial precision
    r3.zw = r3.yx * saturate(r4.wzyx + 0.5);			// partial precision
    r3.xy = (0.2 >= r4.xy ? r3.wzyx : r3.xy);			// partial precision
    r4.xyz = r3.x * const_3.xyz;			// partial precision
    r3.w = r1.z - EmittanceColor.a;
    q11.xyz = (r3.w >= 0.0 ? q52.xyz : ((q3.x * ((q10.x * const_6.xyz) + ((q7.x * const_4.xyz) + q23.xyz))) + q52.xyz));			// partial precision
    r3.xyz = (r3.y * const_4.xyz) + r4.xyz;			// partial precision
    r1.yzw = (r3.w >= 0.0 ? r4.wzyx : r3.wzyx) * IN.texcoord_1.w;			// partial precision
    q25.xyz = ((Toggles.x <= 0.0 ? r0.xyz : (r0.xyz * IN.color_0.rgb)) * max(q11.xyz + AmbientColor.rgb, 0)) + r1.wzy;			// partial precision
    r2.xyz = (Toggles.y <= 0.0 ? q25.xyz : lerp(q25.xyz, IN.color_1.rgb, IN.color_1.a));			// partial precision
    OUT.color_0.rgba = r2.xyzw;			// partial precision

    return OUT;
};

// approximately 91 instruction slots used (4 texture, 87 arithmetic)
