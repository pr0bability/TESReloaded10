// Template for object refraction shaders (pass RenderNormals).
//
// VS
// SLS2068 - Base
// SLS2069 - SKIN
// SLS2070 - FIRE
// SLS2071 - CLEAR
// SLS2072 - CLEAR, SKIN

// SM3033 - SM3
// SM3034 - SM3, SKIN
// SM3035 - SM3, FIRE
// SM3036 - SM3, CLEAR
// SM3037 - SM3, CLEAR, SKIN
//
// PS
// SLS2075 - Base
// SLS2076 - FIRE
// SLS2077 - CLEAR

// SM3032 - SM3
// SM3033 - SM3
// SM3034 - SM3, FIRE
// SM3035 - SM3, CLEAR
// SM3036 - SM3, CLEAR

#if defined(__INTELLISENSE__)
    #define VS
    #define SKIN
#endif

#include "includes/Helpers.hlsl"

#ifdef SKIN
    #include "includes/SkinHelpers.hlsl"
#endif

#ifdef VS

struct VS_INPUT {
    float4 position : POSITION;
    float3 normal : NORMAL;
#ifndef FIRE
    float4 uv : TEXCOORD0;
#endif
#ifdef SKIN
    float3 blendWeight : BLENDWEIGHT;
    float4 blendIndices : BLENDINDICES;
#endif
};

struct VS_OUTPUT {
    float4 sPosition : POSITION;
#ifdef FIRE
    float4 uv : TEXCOORD0;
    float NdotV : TEXCOORD1;
#elif !defined(CLEAR)
    float3 uv : TEXCOORD0;
    float2 normalOffset : TEXCOORD1;
#endif
};

#ifndef SKIN
    row_major float4x4 ModelViewProj : register(c0);
    row_major float4x4 WorldView : register(c4);
#else
    #ifdef SM3
        row_major float4x4 SkinModelViewProj : register(c20);
        row_major float4x4 SkinWorldView : register(c24);
        float4 Bones[54] : register(c52);
    #else
        row_major float4x4 SkinModelViewProj : register(c1);
        row_major float4x4 SkinWorldView : register(c5);
        float4 Bones[54] : register(c44);
    #endif
#endif

#ifdef FIRE
    #ifdef SM3
        float Time : register(c38);
        float4 VEyePosition : register(c40);
    #else
        float Time : register(c13);
        float4 VEyePosition : register(c16);
    #endif
#endif

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;
    
    #ifdef SKIN
        float4 offset = IN.blendIndices.zyxw * 765.01001;
        float4 blend = IN.blendWeight.xyzz;
        blend.w = 1 - weight(IN.blendWeight.xyz);
        
        float4 position = IN.position.xyzw;
        position.w = 1;
        position.xyz = BonesTransformPosition(Bones, offset, blend, position);
    
        OUT.sPosition.xyzw = mul(SkinModelViewProj, position.xyzw);
        
        #ifndef CLEAR
            OUT.uv.xy = IN.uv.xy;
            OUT.uv.z = max(OUT.sPosition.z / 750 + 0.8, 1);
    
            float3 normal = BonesTransformVector(Bones, offset, blend, IN.normal.xyz);
            float2 normalOffset;
            normalOffset.x = dot(SkinWorldView[0].xyz, normalize(normal));
            normalOffset.y = dot(SkinWorldView[1].xyz, normalize(normal));
            OUT.normalOffset = clamp(normalOffset, -0.1, 0.1);
        #endif
    #elif defined(FIRE)
        OUT.sPosition.xyzw = mul(ModelViewProj, IN.position.xyzw);
    
        float4 scaledPos = IN.position.xyzw / 300;
        float3 uv;
        uv.x = dot(WorldView[0], scaledPos);
        uv.y = dot(WorldView[1], scaledPos);
        uv.z = dot(WorldView[2], scaledPos);
        OUT.uv.xy = uv.xy - Time.x;
        OUT.uv.z = uv.z;
        OUT.uv.w = max(uv.z, 1);
    
        OUT.NdotV = dot(IN.normal.xyz, normalize(VEyePosition.xyz - IN.position.xyz));
    #else
        OUT.sPosition.xyzw = mul(ModelViewProj, IN.position.xyzw);
        
        #ifndef CLEAR
            OUT.uv.xy = IN.uv.xy;
            OUT.uv.z = max(OUT.sPosition.z / 750 + 0.8, 1);
    
            float2 normalOffset;
            normalOffset.x = dot(WorldView[0].xyz, IN.normal.xyz);
            normalOffset.y = dot(WorldView[1].xyz, IN.normal.xyz);
            OUT.normalOffset = clamp(normalOffset, -0.1, 0.1);
        #endif
    #endif

    return OUT;
}
#endif // Vertex shaders.

#ifdef PS

struct PS_INPUT {
#ifdef FIRE
    float4 uv : TEXCOORD0;
    float NdotV : TEXCOORD1;
#else
    float3 uv : TEXCOORD0;
    float2 normalOffset : TEXCOORD1;
#endif
};

struct PS_OUTPUT {
    float4 color : COLOR0;
};

#ifndef CLEAR
    sampler2D NormalMap : register(s0);

    #ifdef SM3
        float4 PSRefractionPower : register(c6);
    #else
        float4 PSRefractionPower : register(c29);
    #endif
#endif

PS_OUTPUT main(PS_INPUT IN) {
    PS_OUTPUT OUT;
    
#ifdef CLEAR
    OUT.color.rgba = float4(0.5, 0.5, 0, 0);
#else
    OUT.color.b = PSRefractionPower.x;
    
    float2 normal = tex2D(NormalMap, IN.uv.xy).xy;

    #ifdef FIRE
        OUT.color.rg = compress(normalize(expand(normal.xy)) / IN.uv.w);
        OUT.color.a = sqr(IN.NdotV) * 0.5;
    #else
        OUT.color.rg = compress((0.9 * clamp(expand(normal.xy), -0.1, 0.1) + IN.normalOffset.xy) / IN.uv.z);
        OUT.color.a = PSRefractionPower.y;
    #endif
#endif
    
    return OUT;
}

#endif // Pixel shaders.