//
//
// Parameters:

// float2 Params : register(c4);
float4 TESR_SkyColor;
float4 TESR_SkyLowColor;
float4 TESR_HorizonColor;
float4 TESR_SunColor;
float4 TESR_SunDirection;
float4 TESR_ReciprocalResolution;

float4 TESR_SkyData; // x: athmosphere thickness, y: sun influence, z: sun strength

static const float4x4 ditherMat = {{0.0588, 0.5294, 0.1765, 0.6471},
									{0.7647, 0.2941, 0.8824, 0.4118},
									{0.2353, 0.7059, 0.1176, 0.5882},
									{0.9412, 0.4706, 0.8235, 0.3259}};

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Params       const_4       1
//


// Structures:

struct VS_INPUT {
    float4 color_0 : COLOR0;
    float3 position : VPOS;			// light data in tangent space
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

#include "Includes/Helpers.hlsl"
#include "Includes/Position.hlsl"


VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 up = float3(0, 0, 1);
    float2 uv = IN.position.xy * TESR_ReciprocalResolution.xy;
    float3 eyeDir = normalize(toWorld(uv));
    float verticality = pows(compress(dot(eyeDir, up)), 3);
    float sunHeight = shade(TESR_SunDirection.xyz, up);

    float athmosphere = pows(1 - verticality, 8) * TESR_SkyData.x;
    float sunInfluence = pows(compress(dot(eyeDir, TESR_SunDirection.xyz)), TESR_SkyData.y);

    float3 skyColor = lerp(TESR_SkyLowColor.rgb, TESR_SkyColor.rgb, verticality);
    skyColor = lerp(skyColor, TESR_HorizonColor.rgb, athmosphere * (0.5 + sunInfluence));
    skyColor += sunInfluence * (1 - sunHeight) * (0.5 + 0.5 * athmosphere) * TESR_SunColor * TESR_SkyData.z;

    OUT.color_0.rgb = skyColor;
    OUT.color_0.a = 1;

    // dithering
	OUT.color_0.rgb += ditherMat[ (IN.position.x)%4 ][ (IN.position.y)%4 ] / 255;

    return OUT;
};
