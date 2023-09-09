// Night sky
//
// Parameters:

sampler2D TexMap : register(s0);

float4 TESR_GameTime;
float4 TESR_DebugVar;
float4 TESR_ReciprocalResolution;
float4 TESR_SkyData;

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   TexMap       texture_0       1
//


// Structures:

struct VS_INPUT {
    float2 TexUV : TEXCOORD0;
    float2 position : VPOS;
    float texcoord_2 : TEXCOORD2;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};


// hash based 3d value noise
// function taken from https://www.shadertoy.com/view/XslGRr
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// ported from GLSL to HLSL by https://stackoverflow.com/questions/15628039/simplex-noise-shader
float hash( float n )
{
    return frac(sin(n)*43758.5453);
}

float noise( float3 x )
{
    // The noise function returns a value in the range -1.0f -> 1.0f

    float3 p = floor(x);
    float3 f = frac(x);

    f       = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;

    return lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
            lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
            lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
            lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

#include "Includes/Helpers.hlsl"
#include "Includes/Position.hlsl"

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 stars = tex2D(TexMap, IN.TexUV.xy);
    float4 sky = IN.color_0;

    // float starFlicker = 0.3;//TESR_DebugVar.y; //0.3
    // float noiseScale = 8;//TESR_DebugVar.z;//8;
    // float flickerSpeed = 0.06;//TESR_DebugVar.w;//0.06;
    float starFlicker = 0.3;
    float noiseScale = 8;
    float flickerSpeed = 0.06;

    float3 eyeDir = normalize(toWorld(IN.position.xy * TESR_ReciprocalResolution.xy));
  
    // create animated noise to modulate star brightness
    float n = noise(noiseScale * eyeDir);
    n *= noise((TESR_GameTime.x * flickerSpeed).xxx + noiseScale * eyeDir) * 2;
    n = saturate(saturate(n) * (1 - starFlicker) + starFlicker);

    OUT.color_0.a = (stars.a * sky.a) * IN.texcoord_2.x * TESR_SkyData.w * (n + 1);
    OUT.color_0.rgb = stars.rgb * sky.rgb;
    // float4 color = OUT.color_0;

    // OUT.color_0 = magenta;
    // OUT.color_0.rgba = selectColor(TESR_DebugVar.x, color.rgba, n.xxxx, stars, sky, stars.aaa, sky.aaa, IN.texcoord_2.xxx, OUT.color_0.aaa, black, black);

    return OUT;
};
