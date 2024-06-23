

// Requires the following registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   EyePos          const_1       1
//   shallowColor    const_2       1
//   deepColor       const_3       1
//   ReflectionColor const_4       1
//   FresnelRI       const_5       1  //x: reflectamount, y:fresnel, w: opacity, z:speed
//   BlendRadius     const_6       1
//   VarAmounts      const_8       1  // x: water glossiness y: reflectivity z: refrac, w: lod
//   FogParam        const_9       1
//   FogColor        const_10      1
//   DepthFalloff    const_11      1  // start / end depth fog
//   SunDir          const_12      1
//   SunColor        const_13      1
//   ReflectionMap   texture_0       1
//   RefractionMap   texture_1       1
//   NoiseMap        texture_2       1
//   DisplacementMap texture_3       1
//   DepthMap        texture_4       1
//   float4 TESR_WaveParams : register(c14); // x: choppiness, y:wave width, z: wave speed, w: reflectivity?
//   float4 TESR_WaterVolume : register(c15); // x: caustic strength, y:shoreFactor, w: turbidity, z: caustic strength S ?
//   float4 TESR_WaterSettings : register(c16); // x: caustic strength, y:depthDarkness, w: turbidity, z: caustic strength S ?
//   float4 TESR_GameTime : register(c17);
//   float4 TESR_SkyColor : register(c18);
//   float4 TESR_SunDirection : register(c19);
//   sampler2D TESR_samplerWater : register(s5);


struct PS_INPUT {
    float4 LTEXCOORD_0 : TEXCOORD0_centroid;     // world position of underwater points
    float4 LTEXCOORD_1 : TEXCOORD1_centroid;     // local position on plane object surface
    float4 LTEXCOORD_2 : TEXCOORD2_centroid;     // modelviewproj matrix 1st row 
    float4 LTEXCOORD_3 : TEXCOORD3_centroid;     // modelviewproj matrix 2nd row 
    float4 LTEXCOORD_4 : TEXCOORD4_centroid;     // modelviewproj matrix 3rd row 
    float4 LTEXCOORD_5 : TEXCOORD5_centroid;     // modelviewproj matrix 4th row 
    float4 LTEXCOORD_6 : TEXCOORD6;              // displacement sampling position
    float2 LTEXCOORD_7 : TEXCOORD7;              // waves sampling position
    float4 WorldPosition : TEXCOORD8;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

#include "Includes/PBR.hlsl"

float4 getScreenpos(PS_INPUT IN){
    float4 screenPos;  // point coordinates in screen space for water surface
    screenPos.x = dot(IN.LTEXCOORD_2, IN.LTEXCOORD_1);
    screenPos.w = dot(IN.LTEXCOORD_5, IN.LTEXCOORD_1);
    screenPos.y = screenPos.w - dot(IN.LTEXCOORD_3, IN.LTEXCOORD_1);
    screenPos.z = dot(IN.LTEXCOORD_4, IN.LTEXCOORD_1);
    
    return screenPos;
}

float4 getWaveTexture(PS_INPUT IN, float LODfade, float4 waveParams) {

    float2 texPos = IN.LTEXCOORD_7;

	float waveWidth = waveParams.y;
    float choppiness = waveParams.x;
    float speed = TESR_GameTime.x * 0.002 * waveParams.z;
    float smallScale = 0.5;
    float bigScale = 2;
    float4 waveTexture = expand(tex2D(TESR_samplerWater, texPos * smallScale * waveWidth + normalize(float2(1, 4)) * speed)) * 0.5;
    float4 waveTextureLarge = expand(tex2D(TESR_samplerWater, texPos * bigScale * waveWidth + normalize(float2(-3, -2)) * speed)) * 1;
    float4 waveTextureMicro = expand(tex2D(TESR_samplerWater, texPos * bigScale * 2 * waveWidth + normalize(float2(2, 2)) * speed)) * 0.3;

    // combine waves
    waveTexture = float4(waveTextureLarge.xy + waveTexture.xy + waveTextureMicro.xy,  waveTextureLarge.z + waveTexture.z + waveTextureMicro.z, 1);
    waveTexture.z *= 1/max(choppiness, 0.000001) * lerp(1, 0.5, LODfade);

    waveTexture = normalize(waveTexture);

    return waveTexture;
}

float4 getReflectionSamplePosition(PS_INPUT IN, float3 surfaceNormal, float refractionCoeff) {
    int4 const_7 = {0, 2, -1, 1}; // used to cancel/double/invert vector components

    float4 samplePosition;
    samplePosition.xy = ((refractionCoeff * surfaceNormal.xy)) + IN.LTEXCOORD_1.xy;
    // waveTexture.xy = ((refractionCoeff * surfaceNormal.xy) / IN.LTEXCOORD_0.w) + IN.LTEXCOORD_1.xy;
    samplePosition.zw = (IN.LTEXCOORD_1.z * const_7.wx) + const_7.xw;

    float4 reflectionPos = mul(float4x4(IN.LTEXCOORD_2, IN.LTEXCOORD_3, IN.LTEXCOORD_4, IN.LTEXCOORD_5), samplePosition); // convert local normal to view space

    return reflectionPos;
}

float3 getDisplacement(PS_INPUT IN, float blendRadius, float3 surfaceNormal){
    // sample displacement and mix with the wave texture
    float4 displacement = tex2D(DisplacementMap, IN.LTEXCOORD_6.xy);

    displacement.xy = (displacement.zw - 0.5) * blendRadius / 2;
    displacement.xyz = reconstructZ(displacement.xy);

    // sample displacement and mix with the wave texture
    float3 DisplacementNormal = expand(displacement.xyz);

    // surfaceNormal = compress(surfaceNormal);
    // surfaceNormal += DisplacementNormal;
    // surfaceNormal = normalize(surfaceNormal);
    return surfaceNormal;
}

float4 getLightTravel(float3 refractedDepth, float4 shallowColor, float4 deepColor, float sunLuma, float4 waterSettings, float4 color){
    float4 waterColor = lerp(shallowColor, deepColor, refractedDepth.y); 
    //float4 waterColor = shallowColor; 
    float depthDarknessPower = saturate(pows((1 - waterSettings.y), 3)); // high darkness means low values
    float3 result = color.rgb * lerp(0.7, lerp(waterColor.rgb * depthDarknessPower, 1, depthDarknessPower) , refractedDepth.x) ; //never reach 1 so that water is always absorbing some light
    return float4(result, 1);
}

float4 getTurbidityFog(float3 refractedDepth, float4 shallowColor, float4 waterVolume, float sunLuma, float4 color){
    float turbidity = waterVolume.z;

    float depth = pows(refractedDepth.x, turbidity);

    float fogCoeff = 1 - saturate((FogParam.z - (refractedDepth.x * FogParam.z)) / FogParam.w);
    float3 fog = shallowColor.rgb * sunLuma;

    float3 result = lerp(color.rgb, fog.rgb, saturate(fogCoeff * FogColor.a * turbidity));

    // return float4(1 - refractedDepth.yyy, 1);
    return float4(result, 1);
}

float4 getDiffuse(float3 surfaceNormal, float3 lightDir, float3 eyeDirection, float distance, float4 diffuseColor, float4 color){
    float verticalityFade =  (1 - shades(eyeDirection, float3(0, 0, 1)));
    float distanceFade = smoothstep(0, 1, distance * 0.001);
    float diffuse = shades(lightDir, surfaceNormal) * verticalityFade * distanceFade; // increase intensity with distance
    float3 result = lerp(color.rgb, diffuseColor.rgb, saturate(diffuse));

    return float4(result, 1);
}

float4 getFresnel(float3 surfaceNormal, float3 eyeDirection, float4 reflection, float reflectivity, float4 color){
    // float4 getReflections(float3 surfaceNormal, eyeDirection, float4 reflection, float4 color){
    float fresnelCoeff = saturate(pow(1 - dot(eyeDirection, surfaceNormal), 5));
    float reflectionLuma = luma(reflection);
    float lumaDiff = saturate(reflectionLuma - luma(color));

    //float4 reflectionColor = lerp (reflectionLuma * linearize(ReflectionColor), reflection, reflectionLuma * VarAmounts.y) * 0.7;
    float4 reflectionColor = lerp (reflectionLuma * linearize(ReflectionColor), reflection, reflectionLuma) * 0.7;
	float3 result = lerp(color.rgb, reflection.rgb , saturate((fresnelCoeff * 0.8 + 0.2 * lumaDiff) * reflectivity));

    return float4(result, 1);
}

float4 getSpecular(float3 surfaceNormal, float3 lightDir, float3 eyeDirection, float3 specColor, float4 reflections, float4 color){
    float specularBoost = 10 * TESR_DebugVar.x;
    float glossiness = 10000;

    float3 normal = normalize(surfaceNormal);
    float3 halfway = normalize(eyeDirection + lightDir);
    float NdotH = shades(normal, halfway);

    float3 result;
    if (true){
    // if (TESR_DebugVar.y){
        float NdotL = shades(normal, lightDir);
        float NdotV = shades(normal, eyeDirection);

        float3 Ks = FresnelShlick(white.rgb * TESR_DebugVar.y, halfway, lightDir);
        result = color.rgb + modifiedBRDF(TESR_DebugVar.w, NdotL, NdotV, NdotH, Ks) * specColor * specularBoost;
        // return result;
        // result = color.rgb + BRDF( 0.1 * TESR_DebugVar.y, 0.05 * TESR_DebugVar.z, surfaceNormal, eyeDirection, lightDir) * specColor * specularBoost;
    } else{
        // phong blinn specular
        float specular = pows(NdotH, glossiness);

        // float specular = pow(abs(shades(reflect(-eyeDirection, surfaceNormal), lightDir)), VarAmounts.x);
        result = color.rgb + specular * specColor.rgb * specularBoost;
    }
    // float3 result = color.rgb + PBR(TESR_DebugVar.y, TESR_DebugVar.z, surfaceNormal, eyeDirection, lightDir) * specColor * specularBoost;

    return float4(result, 1);
}

float4 getPointLightSpecular(float3 surfaceNormal, float4 lightPosition, float3 worldPosition, float3 eyeDirection, float3 specColor, float4 color){
    if (lightPosition.w == 0) return color;

    float specularBoost = 10;
    float glossiness = 20;

    float3 lightDir = lightPosition.xyz - worldPosition;
    float distance = length(lightDir) / lightPosition.w;

        // radius based attenuation based on https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
    float s = saturate(distance * distance); 
    float atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));

    //return color + getSpecular(surfaceNormal, normalize(lightDir), eyeDirection, specColor * atten, color);
    float3 H = normalize(normalize(lightDir) + eyeDirection);
    color.rgb += pows(shades(H, surfaceNormal), glossiness) * linearize(float4(specColor, 1)).rgb * specularBoost * atten;
    // color.rgb += pows(shades(H, surfaceNormal), 100) * specColor * 10 * atten;
    return color;
}


// float4 getPBRSpecular(float3 normal, float3 lightDir, float3 eyeDirection, float3 LightColor, float3 reflection, float4 color){
//     float specularBoost = 10;

//     float3 reflections = BRDF( 0.1 * TESR_DebugVar.y, 0.05 * TESR_DebugVar.z, normal, eyeDirection, lightDir) * LightColor * specularBoost;

//     // color = lerp(color, reflection, reflections);

//     return color;
// }



float4 getShoreFade(PS_INPUT IN, float depth, float shoreSpeed, float shoreFactor, float4 color){
    float scale = 0.07;
    shoreSpeed *= 0.1;
    shoreFactor *= 0.1;

    float shoreAnimation = sin(IN.LTEXCOORD_7.x/scale + TESR_GameTime.x * shoreSpeed);
    shoreAnimation *= cos(IN.LTEXCOORD_7.y/scale + TESR_GameTime.x * shoreSpeed);
    shoreAnimation = compress(shoreAnimation); // create a grid of gradient values from 0 to 1

    float depthGradient = smoothstep(saturate(shoreFactor) * compress(sin(TESR_GameTime.x * shoreSpeed) * shoreAnimation), 0, depth);

    color.a = 1 - depthGradient;
    return color;
}


float3 ComputeRipple(sampler2D puddlesSampler, float2 UV, float CurrentTime, float Weight)
{
    float4 Ripple = tex2D(puddlesSampler, UV);
    Ripple.yz = expand(Ripple.yz); // convert from 0/1 to -1/1 

    float period = frac(Ripple.w + CurrentTime);
    float TimeFrac = period - 1.0f + Ripple.x;
    float DropFactor = saturate(0.2f + Weight * 0.8f - period);
    float FinalFactor = DropFactor * Ripple.x * sin( clamp(TimeFrac * 9.0f, 0.0f, 3.0f) * PI);

    return float3(Ripple.yz * FinalFactor * 0.35f, 1.0f);
}


float3 getRipples(PS_INPUT IN, sampler2D puddlesSampler, float3 surfaceNormal, float distance, float rainCoeff){

    float distanceFade = 1 - saturate(invlerp(0, 3500, distance));

    if (!rainCoeff || !distanceFade) return surfaceNormal;

    // sample and combine rain ripples
    float4 time = float4(0.96f, 0.97f,  0.98f, 0.99f) * 0.07; // Ripple timing

	float2 rippleUV = IN.LTEXCOORD_7 * 5; // scale coordinates
	float4 Weights = float4(1, 0.75, 0.5, 0.25) * rainCoeff;
	Weights = saturate(Weights * 4) * 2 * distanceFade;
	float3 Ripple1 = ComputeRipple(puddlesSampler, rippleUV + float2( 0.25f,0.0f), time.x * TESR_GameTime.x, Weights.x);
	float3 Ripple2 = ComputeRipple(puddlesSampler, rippleUV * 1.1 + float2(-0.55f,0.3f), time.y * TESR_GameTime.x, Weights.y);
	float3 Ripple3 = ComputeRipple(puddlesSampler, rippleUV * 1.3 + float2(0.6f, 0.85f), time.z * TESR_GameTime.x, Weights.z);
	float3 Ripple4 = ComputeRipple(puddlesSampler, rippleUV * 1.5 + float2(0.5f,-0.75f), time.w * TESR_GameTime.x, Weights.w);

	float4 Z = lerp(1, float4(Ripple1.z, Ripple2.z, Ripple3.z, Ripple4.z), Weights);
	float3 ripple = float3( Weights.x * Ripple1.xy + Weights.y * Ripple2.xy + Weights.z * Ripple3.xy + Weights.w * Ripple4.xy, Z.x * Z.y * Z.z * Z.w);
	float3 ripnormal = normalize(ripple);
    
    float3 combnom = normalize(float3(ripnormal.xy + surfaceNormal.xy, surfaceNormal.z));

    return combnom;
}