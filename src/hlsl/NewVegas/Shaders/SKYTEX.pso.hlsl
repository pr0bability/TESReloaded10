// Shader to blend cloud layers from weathers transitions with the sky
//
// Parameters:

float2 Params : register(c4);
sampler2D TexMap : register(s0);
sampler2D TexMapBlend : register(s1);

float4 TESR_DebugVar : register(c5);
float4 TESR_SunColor : register(c6);
float4 TESR_SkyColor : register(c7);
float4 TESR_SkyLowColor : register(c8);
float4 TESR_HorizonColor : register(c9);
float4 TESR_SunDirection : register(c10);
float4 TESR_SkyData : register(c11);   // x:AthmosphereThickness y:SunInfluence z:SunStrength w:StarStrength
float4 TESR_CloudData : register(c12); // x:UseNormals y:SphericalNormals z:Transparency w: cloud Brightness
float4 TESR_SunAmount : register(c13); // x:isDayTime, w:GlareStrength
float4 TESR_SunPosition : register(c14);
float4 TESR_SunsetColor : register(c15);
float4 TESR_HDRBloomData : register(c16);
float4 TESR_SunDiskColor : register(c17);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Params       const_4       1
//   TexMap       texture_0       1
//   TexMapBlend  texture_1       1
//


static const float UseNormals = TESR_CloudData.x;
static const float SphericalNormals = 1;//TESR_CloudData.y;
static const float SUNINFLUENCE = 1/TESR_SkyData.y;
static const float SunStrength = TESR_SkyData.z;
static const float SkyMultiplier = TESR_SunsetColor.w;

// Structures:

struct VS_INPUT {
    float2 TexUV : TEXCOORD0;
    float2 position: VPOS;
    float2 TexBlendUV : TEXCOORD1;
    float3 location: TEXCOORD2;
    float4 color_0 : COLOR0;
    float4 color_1 : COLOR1;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0; // sunglare active region/suncolor
};


struct SunValues {
    float athmosphere;
    float sunHeight;
    float sunInfluence;
    float3 sunColor;
    float3 sunDir;
    float isDayTime;
};

// Code:
#include "Includes/Helpers.hlsl"
#include "Includes/Sky.hlsl"

static const float3 up = blue.xyz;

float3 getNormal(float2 partial, float3 eyeDir){

    // if spherical normals are not activated, we must convert the normals pointing up
    if (!SphericalNormals){
        float2 dir = normalize(eyeDir.xy);
        float2x2 R = {{dir.x, dir.y}, { dir.y, -dir.x}};
        partial = compress(mul(expand(partial.xy), R));
        partial.y = 1 - partial.y;
    }

    partial = expand(partial);

    // reconstruct Z component
    float z = sqrt(1 - saturate(dot(partial, partial)));
    float3 normal = normalize(float3(partial, z));

    // get TBN matrix by getting the plane perpendicular to the eye direction
    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
    float3 N = eyeDir;
    float3 T = float3(N.y, -N.x, 0);  // horizontal X axis. Maybe we could rotate it to get spherical normals?
    float3 B = cross(T, N);
    // float3 B = cross(N, T);

    float3x3 TBN = {T, B, N};
    return normalize(mul(normal, TBN));
}


/*
* Shades the sun, moon and sunglare textures
*/
float4 ShadeSun(SunValues Sun, float4 texColor, float4 vertexColor){
    Sun.isDayTime = smoothstep(0.498, 0.502, TESR_SunAmount.x); // very short transition time to get mostly a value about wether the sun is up or not

    float sunTexLuma = luma(texColor.rgb); 
    // detect sundisk texture using full alpha and brightness, as well as daytime (to differentiate with the moon)
    float isSunOrMoon = saturate(smoothstep(0.9, 1.0, texColor.w)) * smoothstep(0.9, 1, sunTexLuma) * Sun.isDayTime;
    float isSun = isSunOrMoon * Sun.isDayTime;

    if (isSun){
        float isSunset = smoothstep(0.3, 0.0, Sun.sunHeight);
        texColor.rgb += isSunset * Sun.sunColor;
        texColor.rgb += Sun.sunColor * TESR_SunAmount.w;
        texColor.a = Sun.isDayTime; // force alpha 1 for sun disk in the daytime
    }else{
        texColor.rgb *= vertexColor.rgb * Params.y * lerp(TESR_SunAmount.w, 1, isSunOrMoon); // vertex color (animated by the engine)
        texColor.a *= vertexColor.a;
    }

    return delinearize(texColor);
}


/*
* Shades the cloud textures
*/
float4 ShadeClouds(float4 finalColor, float4 vertexColor, float3 skyColor, SunValues Sun, float3 eyeDir){
    // shade clouds 
    float greyScale = lerp(luma(finalColor), 1.0, saturate(TESR_CloudData.w));
    float alpha = finalColor.w * TESR_CloudData.z;

    // calculate sky color to blend in the clouds
    float3 scattering = pows(Sun.sunInfluence, 20) * smoothstep(0.5, 1, 1.0 - alpha) * Sun.sunColor;
    float sunDir = Sun.sunDir;
    float3 baseSkyColor = linearize(TESR_SkyColor).rgb;

    if (!UseNormals){
        // simply tint the clouds
        greyScale = (greyScale - 0.5) * 1.5 + 0.5; // tests for increasing cloud contrast before shading
        float3 cloudTint = lerp(pows(baseSkyColor * 0.5, 5.0), lerp(baseSkyColor, Sun.sunColor * 5, 0.7 * Sun.sunDir + 0.3), (1 - saturate(Sun.sunInfluence)) * greyScale).rgb;
        cloudTint = lerp(white.rgb, cloudTint * TESR_CloudData.w * 1.333, (1 - Sun.sunHeight) * Sun.isDayTime); // tint the clouds less when the sun is high in the sky and at night

        // finalColor.rgb *= lerp(1.0, cloudTint * TESR_CloudData.w * 1.333, isDayTime); // cancel tint at night
        finalColor.rgb *= cloudTint;
        finalColor.rgb += lerp(black.rgb, scattering * 4.0, (1 - Sun.sunHeight) * Sun.isDayTime);
    } else {
        // Tests for normals lit cloud
        float2 normal2D = finalColor.xy; // normal x and y are in red and green, blue is reconstructed
        float3 normal = getNormal(normal2D, -eyeDir); // reconstruct world normal from red and green

        greyScale = lerp(saturate(TESR_CloudData.w), 1.0f, finalColor.z); // greyscale is stored in blue channel
        float3 ambient = skyColor * greyScale * lerp(0.5, 0.7, Sun.sunDir); // fade ambient with sun direction
        float3 diffuse = compress(dot(normal, TESR_SunPosition.xyz)) * Sun.sunColor * (1.0 - luma(ambient)) * lerp(0.8, 1, Sun.sunDir); // scale diffuse if ambient is high
        float3 fresnel = pows(1.0 - shade(-eyeDir, normal), 4.0) * pows(saturate(expand(Sun.sunDir)), 2.0) * shade(normal, up) * (Sun.sunColor + skyColor) * 0.2;
        float3 bounce = shade(normal, -up) * pows(TESR_HorizonColor.rgb, 2.2) * 0.1 * Sun.sunHeight; // linearise, light from the ground bouncing up to the underside of clouds

        finalColor = float4(ambient + diffuse + fresnel + scattering + bounce, alpha);
        // finalColor.rgb = selectColor(TESR_DebugVar.x, finalColor, ambient, diffuse, fresnel, bounce, scattering, sunColor, skyColor, normal, float3(IN.TexUV, 1));
    }

    finalColor = float4(finalColor.rgb * vertexColor.rgb * Params.y, pows(saturate(finalColor.w * vertexColor.a), 1/TESR_CloudData.z)); // scale alpha with setting

    return delinearize(float4(finalColor.rgb * SkyMultiplier * TESR_CloudData.a, finalColor.a));
}


VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 color = IN.color_0;
    color = linearize(color);

    float3 eyeDir = normalize(IN.location);
    float verticality = pows(compress(dot(eyeDir, up)), 3);

    SunValues Sun;
    Sun.sunHeight = shade(TESR_SunPosition.xyz, up);
    Sun.sunDir = compress(dot(eyeDir, TESR_SunPosition.xyz)); // stores wether the camera is looking in the direction of the sun in range 0/1
    Sun.athmosphere = pows(1 - verticality, 8) * TESR_SkyData.x;
    Sun.sunInfluence = pows(Sun.sunDir, SUNINFLUENCE);
    Sun.isDayTime = smoothstep(0, 0.5, TESR_SunAmount.x);
    Sun.sunColor = GetSunColor(Sun.sunHeight, TESR_SkyData.x, TESR_SunAmount.x, TESR_SunColor.rgb, TESR_SunsetColor.rgb);

    float cloudsPower = Params.x;
    float4 cloudsWeather1 = linearize(tex2D(TexMap, IN.TexUV.xy));
    float4 cloudsWeather2 = linearize(tex2D(TexMapBlend, IN.TexBlendUV.xy));
    float4 cloudsWeatherBlend = lerp(cloudsWeather1, cloudsWeather2, cloudsPower); // weather transition

    float4 finalColor = (weight(cloudsWeather1.xyz) == 0.0 ? cloudsWeather2 : (weight(cloudsWeather2.xyz) == 0.0 ? cloudsWeather1 : cloudsWeatherBlend)); // select either weather or blend
    finalColor.a = cloudsWeatherBlend.a;

    if (IN.color_1.r){ // early out if this texture is sun/moon*
        OUT.color_0 = ShadeSun(Sun, finalColor, color);
        return OUT;
    } else {
        float3 skyColor = GetSkyColor(verticality, Sun.athmosphere, Sun.sunHeight, Sun.sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, Sun.sunColor);
        OUT.color_0 = ShadeClouds(finalColor, color, skyColor, Sun, eyeDir);
        return OUT;
    }
};
