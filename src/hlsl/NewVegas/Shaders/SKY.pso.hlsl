//
//
// Parameters:

float2 Params : register(c4);
float4 TESR_SkyColor : register(c5);
float4 TESR_SkyLowColor: register(c6);
float4 TESR_HorizonColor: register(c7);
float4 TESR_SunColor: register(c8);
float4 TESR_SunDirection: register(c9);
float4 TESR_SkyData: register(c10); // x: athmosphere thickness, y: sun influence, z: sun strength w: sky strength
float4 TESR_SunAmount : register(c11); // x: dayTime, y:sunGlareAmnt, z:replace sun
float4 TESR_DebugVar: register(c12);
float4 TESR_SunPosition: register(c13);
float4 TESR_SunsetColor: register(c14); // sunsetColor.w is sky strength multiplier
float4 TESR_HDRBloomData: register(c15);
float4 TESR_SunDiskColor: register(c16);

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
#include "Includes/Sky.hlsl"


VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 up = float3(0, 0, 1);
    float3 eyeDir = normalize(IN.eye.xyz);
    float verticality = pows(compress(dot(eyeDir, up)), 3);
    float sunHeight = shade(TESR_SunPosition.xyz, up);

    float athmosphere = pows(1 - verticality, 8) * TESR_SkyData.x;
    float sunDir = compress(dot(eyeDir, TESR_SunPosition.xyz));
    float sunInfluence = pows(sunDir, SUNINFLUENCE);

    float3 sunColor = GetSunColor(sunHeight, TESR_SkyData.x, TESR_SunAmount.x, TESR_SunDiskColor.rgb, TESR_SunsetColor.rgb);
    float3 skyColor = GetSkyColor(verticality, athmosphere, sunHeight, sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, sunColor);

    // draw the sun procedurally
    //float sunDisk = smoothstep(0.9990, 0.9991, sunDir);
    // float sunGlare = saturate(pow(saturate(sunDir), 400) * TESR_SunAmount.y * sunHeight);
    // skyColor = lerp(skyColor, white.rgb, saturate(sunDisk)); // add sun disk and boost brightness during sunrise/sunset
    // skyColor += sunColor * saturate(sunDisk + sunGlare) * TESR_SunAmount.z * (TESR_SunAmount.x > 0); // add sun disk and boost brightness during sunrise/sunset
    //skyColor += white * saturate(sunDisk); // add sun disk and boost brightness during sunrise/sunset
    // skyColor += sunColor * saturate(sunDisk) * TESR_SunAmount.z; // add sun disk and boost brightness during sunrise/sunset

    OUT.color_0 = delinearize(float4(skyColor * TESR_SunsetColor.w, 1)) ; // multiply sky strength for HDR

    // dithering
	OUT.color_0.rgb += ditherMat[ (IN.screen.x)%4 ][ (IN.screen.y)%4 ] / 255;

    // OUT.color_0.rgb = lerp(green, red, sunDir);

    return OUT;
};
