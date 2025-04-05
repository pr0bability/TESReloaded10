// vertex shader for sun/sunglare/moon objects
//
// Parameters:

row_major float4x4 ModelViewProj : register(c0);
float4 BlendColor[3] : register(c4);


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
    float4 texcoord_0 : TEXCOORD0;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float2 texcoord_0 : TEXCOORD0;
    float2 texcoord_1 : TEXCOORD1;
    float3 location : TEXCOORD2;
    float4 color_0 : COLOR0;
    float4 color_1 : COLOR1;
};

// Code:

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    OUT.color_0.rgb = (IN.color_0.r * BlendColor[0].rgb) + (BlendColor[1].rgb * IN.color_0.g) + (IN.color_0.b * BlendColor[2].rgb);
    OUT.color_0.a = BlendColor[0].a * IN.color_0.a;

    OUT.position.xyzw = mul(ModelViewProj, IN.position).xyww;

    #ifdef REVERSED_DEPTH
        OUT.position.z *= 0.000100017f; // invert depth
    #else
        OUT.position.z *= 0.999899983f; // place in front of the moon mask that hides the stars
    #endif

    OUT.texcoord_0.xy = IN.texcoord_0.xy;
    OUT.texcoord_1.xy = IN.texcoord_0.xy;

    OUT.location = OUT.position.xyz;
    OUT.color_1 = float4(1, 0, 0, 1); // identify this object as being the sun/sunglare/moon

    return OUT;
};

// approximately 10 instruction slots used
