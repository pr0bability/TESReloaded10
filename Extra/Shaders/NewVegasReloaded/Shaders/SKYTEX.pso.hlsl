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
float4 TESR_CloudData : register(c12); // x:UseNormals y:SphericalNormals z:Transparency
float4 TESR_SunAmount : register(c13); // x:isDayTime, w:GlareStrength
float4 TESR_SunPosition : register(c14);
float4 TESR_SunsetColor : register(c15);
float4 TESR_HDRBloomData : register(c16);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Params       const_4       1
//   TexMap       texture_0       1
//   TexMapBlend  texture_1       1
//


static const float UseNormals = TESR_CloudData.x;
static const float SphericalNormals = TESR_CloudData.y;
static const float SUNINFLUENCE = 1/TESR_SkyData.y;

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

// Code:
#include "Includes/Helpers.hlsl"
#include "Includes/Sky.hlsl"

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

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4 color = IN.color_0;
    color.rgb = pows(color.rgb, 2.2); // linearise

    float3 up = float3(0, 0, 1);
    float3 eyeDir = normalize(IN.location);
    float verticality = pows(compress(dot(eyeDir, up)), 3);
    float sunHeight = shade(TESR_SunPosition.xyz, up);

    float sunDir = compress(dot(eyeDir, TESR_SunPosition.xyz)); // stores wether the camera is looking in the direction of the sun in range 0/1

    float athmosphere = pows(1 - verticality, 8) * TESR_SkyData.x;
    float sunInfluence = pows(sunDir, SUNINFLUENCE);

    float cloudsPower = Params.x;
    float4 cloudsWeather1 = tex2D(TexMap, IN.TexUV.xy);
    float4 cloudsWeather2 = tex2D(TexMapBlend, IN.TexBlendUV.xy);
    cloudsWeather1.rgb = pows(cloudsWeather1.rgb, 2.2); // linearise
    cloudsWeather2.rgb = pows(cloudsWeather2.rgb, 2.2); // linearise

    float4 cloudsWeatherBlend = lerp(cloudsWeather1, cloudsWeather2, cloudsPower); // weather transition

    float4 finalColor = (weight(cloudsWeather1.xyz) == 0.0 ? cloudsWeather2 : (weight(cloudsWeather2.xyz) == 0.0 ? cloudsWeather1 : cloudsWeatherBlend)); // select either weather or blend
    finalColor.a = cloudsWeatherBlend.a;
    // finalColor = cloudsWeather1;
    float3 sunColor = GetSunColor(sunHeight, TESR_SkyData.x, TESR_SunAmount.x, pows(TESR_SunColor.rgb, 2.2), pows(TESR_SunsetColor.rgb, 2.2));

    
		//ShaderConst.Sky.SkyData.y = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunInfluence");
		//ShaderConst.Sky.SkyData.z = TheSettingManager->GetSettingF("Shaders.Sky.Main", "SunStrength");

    if (IN.color_1.r){ // early out if this texture is sun/moon
        float isSun = saturate(smoothstep(0.9, 1.0, finalColor.w)); // detect sundisk texture using full alpha
        float isDayTime = TESR_SunAmount.x > 0 ? 1.0 : 0.0;
        float alpha = lerp(finalColor.w * IN.color_0.a, 1.0, saturate(isSun * isDayTime)); // force alpha 1 for sun disk in the daytime
        float isSunset = smoothstep(0.0, 0.3, sunHeight);

        finalColor.rgb *= color.rgb * Params.y;
        
        finalColor = float4(finalColor.rgb + (isDayTime * isSun * sunColor) * TESR_SunAmount.w, pows(alpha, TESR_SunAmount.w));
        finalColor.rgb = pows(finalColor.rgb, 1.0/2.2);

        OUT.color_0 = finalColor;

        return OUT;
    }

    // shade clouds 
    float greyScale = lerp(luma(finalColor), 1.0, saturate(TESR_CloudData.w));
    float alpha = finalColor.w * TESR_CloudData.z;

    // calculate sky color to blend in the clouds    
    float3 skyColor = GetSkyColor(verticality, athmosphere, sunHeight, sunInfluence, TESR_SkyData.z, pows(TESR_SkyColor.rgb, 2.2), pows(TESR_SkyLowColor.rgb, 2.2), pows(TESR_HorizonColor.rgb, 2.2), sunColor);
    float3 scattering = sunInfluence * lerp(0.3, 1.0, 1.0 - alpha) * (skyColor + sunColor);

    if ( UseNormals){
        // Tests for normals lit cloud
        float2 normal2D = finalColor.xy; // normal x and y are in red and green, blue is reconstructed
        float3 normal = getNormal(normal2D, -eyeDir); // reconstruct world normal from red and green

        greyScale = lerp(saturate(TESR_CloudData.w), 1.0f, finalColor.z); // greyscale is stored in blue channel
        float3 ambient = skyColor * greyScale * lerp(0.5, 0.7, sunDir); // fade ambient with sun direction
        float3 diffuse = compress(dot(normal, TESR_SunPosition.xyz)) * sunColor * (1.0 - luma(ambient)) * lerp(0.8, 1, sunDir); // scale diffuse if ambient is high
        float3 fresnel = pows(1.0 - shade(-eyeDir, normal), 4.0) * pows(saturate(expand(sunDir)), 2.0) * shade(normal, up) * (sunColor + skyColor) * 0.2;
        float3 bounce = shade(normal, -up) * pows(TESR_HorizonColor.rgb, 2.2) * 0.1 * sunHeight; // light from the ground bouncing up to the underside of clouds

        finalColor = float4(ambient + diffuse + fresnel + scattering + bounce, alpha);
        // finalColor.rgb = selectColor(TESR_DebugVar.x, finalColor, ambient, diffuse, fresnel, bounce, scattering, sunColor, skyColor, normal, float3(IN.TexUV, 1));
    } else {
        // simply tint the clouds
        float sunInfluence = 1.0 - pows(sunDir, 3.0);
        float3 cloudTint = lerp(pows(TESR_SkyLowColor.rgb, 2.2), sunColor, saturate(sunInfluence * saturate(greyScale))).rgb;
        cloudTint = lerp(cloudTint, pows(white.rgb, 2.2), sunHeight * TESR_SunAmount.x); // tint the clouds less when the sun is high in the sky

        float dayLight = saturate(luma(sunColor));

        finalColor.rgb *= cloudTint * TESR_CloudData.w * 1.5;
        finalColor.rgb += scattering;
    }
    finalColor = float4(finalColor.rgb * color.rgb * Params.y, saturate(finalColor.w * IN.color_0.a * TESR_CloudData.z));
    
    finalColor.rgb = pows(finalColor.rgb * TESR_SunsetColor.w, 1.0/2.2);
    
    OUT.color_0 = finalColor;
    return OUT;
};
