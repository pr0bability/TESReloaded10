//
// Generated by Microsoft (R) D3DX9 Shader Compiler 9.08.299.0000
//
//   vsa shaderdump19/NIGHTEYE000.vso /Fcshaderdump19/NIGHTEYE000.vso.dis
//
//
// Parameters:

float4 texRatio0 : register(c6);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   texRatio0    const_6       1
//


struct VS_INPUT {
    float4 LPOSITION : POSITION;
    float4 LTEXCOORD_0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.position.xyzw = IN.LPOSITION.xyzw;
    OUT.texcoord_0.xy = (IN.LTEXCOORD_0.xy * texRatio0.xy) + texRatio0.zw;

    return OUT;
};