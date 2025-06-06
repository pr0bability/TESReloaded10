//
// Generated by Microsoft (R) D3DX9 Shader Compiler 9.08.299.0000
//
//   psa shaderdump19/WATERHMAP005.pso /Fcshaderdump19/WATERHMAP005.pso.dis
//
//
// Parameters:
//
float fNoiseScale : register(c3);
float4 TESR_GameTime : register(c6);
float4 TESR_WaveParams : register(c7);

sampler2D NoiseMap : register(s0);
sampler3D TESR_samplerWater : register(s1) < string ResourceName = "Water\water_NRM.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; ADDRESSW = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   fTexScroll0  c0       1
//   fTexScroll1  c1       1
//   fTexScroll2  c2       1
//   fTexScale    c4       1
//   fAmplitude   c5       1
//   NoiseMap     s0       1
//

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   fTexScroll0  const_0       1
//   fTexScroll1  const_1       1
//   fTexScroll2  const_2       1
//   fNoiseScale  const_3       1
//   fTexScale    const_4       1
//   fAmplitude   const_5       1
//   NoiseMap     texture_0       1
//


// Structures:

struct VS_OUTPUT {
    float2 texcoord_0 : TEXCOORD0;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

// Code:

PS_OUTPUT main(VS_OUTPUT IN) {
    PS_OUTPUT OUT;

    float frame = TESR_GameTime.z * TESR_WaveParams.z;
    OUT.color_0 = tex3D(TESR_samplerWater, float3(IN.texcoord_0.xy, frac(frame)));
    return OUT;
};
