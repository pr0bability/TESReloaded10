//
//
// Parameters:

float2 Params : register(c4);
float4 TESR_SkyColor : register(c5);
float4 TESR_SkyLowColor: register(c6);
float4 TESR_HorizonColor: register(c7);
float4 TESR_SunColor: register(c8);
float4 TESR_SunDirection: register(c9);
float4 TESR_SkyData: register(c10); // x: athmosphere thickness, y: sun influence, z: sun strength
float4 TESR_SunAmount : register(c11); // x: dayTime, y:sunGlareAmnt, z:replace sun
float4 TESR_DebugVar: register(c12);
float4 TESR_SunPosition: register(c13);


static const float4x4 ditherMat = {{0.0588, 0.5294, 0.1765, 0.6471},
									{0.7647, 0.2941, 0.8824, 0.4118},
									{0.2353, 0.7059, 0.1176, 0.5882},
									{0.9412, 0.4706, 0.8235, 0.3259}};

static const float SUNINFLUENCE = 1/TESR_SkyData.y;

// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Params       const_4       1
//


// Structures:

struct VS_INPUT {
    float4 color_0 : COLOR0;
    float2 screen : VPOS;
    float3 eye : TEXCOORD0_centroid;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0;
};

#include "Includes/Helpers.hlsl"
#include "Includes/Position.hlsl"


VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 up = float3(0, 0, 1);
    float3 eyeDir = normalize(IN.eye.xyz);
    float verticality = pows(compress(dot(eyeDir, up)), 3);
    float sunHeight = shade(TESR_SunPosition.xyz, up);

    float athmosphere = pows(1 - verticality, 8) * TESR_SkyData.x;
    float sunDir = dot(eyeDir, TESR_SunPosition.xyz);
    float sunInfluence = pows(compress(sunDir), SUNINFLUENCE);

    float3 sunColor = (1 + sunHeight) * TESR_SunColor; // increase suncolor strength with sun height
    sunColor = lerp(sunColor, sunColor + float3(1, 0, 0.03), saturate(pows(1 - sunHeight, 8) * TESR_SkyData.x)); // add extra red to the sun at sunsets

    float3 skyColor = lerp(TESR_SkyLowColor.rgb, TESR_SkyColor.rgb, saturate(verticality));
    skyColor = lerp(skyColor, TESR_HorizonColor.rgb, saturate(athmosphere * (0.5 + sunInfluence)));
    skyColor += sunInfluence * (1 - sunHeight) * (0.5 + 0.5 * athmosphere) * sunColor * TESR_SkyData.z * TESR_SunAmount.x;

    // draw the sun procedurally
    float sunDisk = smoothstep(0.9996, 0.9997, sunDir);
    float sunGlare = saturate(pow(saturate(sunDir), 400) * TESR_SunAmount.y * sunHeight);
    // skyColor = lerp(skyColor, sunColor, saturate(sunDisk + sunGlare)); // add sun disk and boost brightness during sunrise/sunset
    skyColor += sunColor * saturate(sunDisk + sunGlare) * TESR_SunAmount.z; // add sun disk and boost brightness during sunrise/sunset

    OUT.color_0.rgb = skyColor;
    OUT.color_0.a = 1;

    // dithering
	OUT.color_0.rgb += ditherMat[ (IN.screen.x)%4 ][ (IN.screen.y)%4 ] / 255;

    return OUT;
};
