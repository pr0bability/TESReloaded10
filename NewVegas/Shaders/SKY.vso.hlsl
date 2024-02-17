//
//
// Parameters:

float4 BlendColor[3] : register(c4);
row_major float4x4 ModelViewProj : register(c0);

float4 TESR_SunDirection;


// Registers:
//
//   Name          Reg   Size
//   ------------- ----- ----
//   ModelViewProj[0] const_0        1
//   ModelViewProj[1] const_1        1
//   ModelViewProj[2] const_2        1
//   ModelViewProj[3] const_3        1
//   BlendColor[0]    const_4        1
//   BlendColor[1]    const_5        1
//   BlendColor[2]    const_6        1
//


// Structures:

struct VS_INPUT {
    float4 position : POSITION;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float3 eye : TEXCOORD0_centroid;
    float4 color_0 : COLOR0;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 r0;

    r0.xyz = (IN.color_0.r * BlendColor[0].rgb) + (BlendColor[1].rgb * IN.color_0.g) + (IN.color_0.b * BlendColor[2].rgb);
    OUT.color_0.rgb = r0.xyz;
    OUT.color_0.a = BlendColor[0].a * IN.color_0.a;


    OUT.position.xyzw = mul(ModelViewProj, IN.position).xyww;
    OUT.eye.xyz = IN.position.xyz;


    return OUT;
};

// approximately 8 instruction slots used
