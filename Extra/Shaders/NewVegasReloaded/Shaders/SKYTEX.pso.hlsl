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
float4 TESR_SkyData : register(c11);
float4 TESR_ReciprocalResolution : register(c12);


// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   Params       const_4       1
//   TexMap       texture_0       1
//   TexMapBlend  texture_1       1
//


// Structures:

struct VS_INPUT {
    float2 TexUV : TEXCOORD0;
    float2 position: VPOS;
    float2 TexBlendUV : TEXCOORD1;
    float4 color_0 : COLOR0;
};

struct VS_OUTPUT {
    float4 color_0 : COLOR0; // sunglare active region/suncolor
};

// Code:
#include "Includes/Helpers.hlsl"
#include "Includes/Position.hlsl"


// gets the rotation of the vector based on the up direction
float3x3 getRotationMatrix(float3 vect, float3 up = blue){
    float3x3 I = {{1, 0, 0},
                  {0, 1, 0},
                  {0, 0, 1}};

    // if (dot(vect, up) == 1) return mat;

    float dotproduct = dot(up, vect);
    float3 crossproduct = cross(vect, up);
    float lengthCross = length(crossproduct);

    float3x3 G = {{dotproduct, -lengthCross, 0},
                    {lengthCross, dotproduct, 0},
                    {0, 0, 1}};

    float3 v = vect - dotproduct * up;
    float3x3 Fi = {up, (v / normalize(v)), crossproduct};

    return mul(Fi, mul(G, transpose(Fi)));
}

float3 getNormal(float2 partial, float3 eyeDir){
    partial = expand(partial);

    // rotate eyedir.xy to float(0, 1), then rotate partial the same
    // float l = length(eyeDir.xy);
    // float a = 1/(l * l);
    // float2 dir = eyeDir.xy * a;
    // float2x2 R= {{-dir.y, dir.x},{ dir.x, dir.y}};
    // partial = mul(partial, R);

    // reconstruct Z component
    float z = sqrt(1 - saturate(dot(partial, partial)));
    float3 normal = normalize(float3(partial, z));

    // get TBN matrix by getting the plane perpendicular to the eye direction
    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
    float3 N = eyeDir;
    float3 T = float3(N.y, -N.x, 0);
    float3 B = cross(T, N);

    float3x3 TBN = {T, B, N};
    return normalize(mul(normal, TBN));
}

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float3 up = float3(0, 0, 1);
    float2 uv = IN.position.xy * TESR_ReciprocalResolution.xy;
    float3 eyeDir = normalize(toWorld(uv));
    float verticality = pows(compress(dot(eyeDir, up)), 3);
    float sunHeight = shade(TESR_SunDirection.xyz, up);

    float sunDir = dot(eyeDir, TESR_SunDirection.xyz);
    float athmosphere = pows(1 - verticality, 8) * TESR_SkyData.x;
    float sunInfluence = pows(compress(sunDir), TESR_SkyData.y);
    float sunDisk = pows(saturate(sunDir), 200);

    float cloudsPower = Params.x;
    float4 cloudPanorama = tex2D(TexMap, IN.TexUV.xy);
    float4 cloudsLayer = tex2D(TexMapBlend, IN.TexBlendUV.xy);

    float4 cloudsWeatherBlend = lerp(cloudPanorama, cloudsLayer, cloudsPower); // weather transition

    float4 finalColor = (weight(cloudPanorama.xyz) == 0.0 ? cloudsLayer : (weight(cloudsLayer.xyz) == 0.0 ? cloudPanorama : cloudsWeatherBlend)); // select either weather or blend
    finalColor.w = cloudsWeatherBlend.w; // first cloud layer

    // float greyScale = finalColor.w;

    /////////////////////////////////////////////////////////////
    // Tests for normals lit cloud
    float3 normal = getNormal(cloudPanorama.xy, -eyeDir);

    float greyScale = lerp(TESR_DebugVar.x, 1, cloudPanorama.z);
    float alpha = lerp(0, cloudPanorama.w * TESR_DebugVar.y, saturate(1 - athmosphere * 0.8 + sunDisk));

    float3 diffuse = shade(normal, TESR_SunDirection.xyz) * TESR_SunColor.rgb;
    float3 ambient = lerp(TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, greyScale) * 0.6;
    float3 fresnel = 0;//pow(1 - shade(-eyeDir, normal), 5) * shade(eyeDir, TESR_SunDirection.xyz) * TESR_SunColor.rgb * 0.1;
    float3 bounce = shade(normal, -up) * TESR_HorizonColor.rgb * 0.2;

    float3 scattering = shade(eyeDir, TESR_SunDirection.xyz) * (1 - alpha) * TESR_SunColor.rgb * 0.2;

    float4 color = float4(ambient + diffuse + fresnel + scattering + bounce, alpha);
    color = lerp(color, finalColor, saturate(sunDisk));
    /////////////////////////////////////////////////////////////

    // float3 cloudTint = lerp(pow(TESR_SkyLowColor, 5.0), TESR_SunColor * 1.5, saturate((sunDisk + (1 - pow(compress(sunDir), 3.0))) * saturate(greyScale)));
    // cloudTint = lerp(cloudTint, white, sunHeight); // tint the clouds less when the sun is high in the sky

    // float dayLight = saturate(sunDisk + luma(TESR_SunColor));
    // cloudTint = lerp(float3(0.2, 0.2, 0.3), cloudTint, dayLight); // nightime cloud tint

    // finalColor.rgb *= cloudTint;

    // float4 color = float4((finalColor.xyz * IN.color_0.rgb) * Params.y, finalColor.w * IN.color_0.a);
    // color.a = lerp(color.a, color.a * 0.7, dayLight); // add setting for cloud alpha for daytime
    
    // finalColor: sunglare
    // finalColor alpha: clouds first layer
    // OUT.color_0.rgba = selectColor(TESR_DebugVar.x, color, float4(cloudTint, 1), float4(sunDisk.xxx, 1), float4(finalColor.www, 1), finalColor, float4(finalColor.rgb, 1), float4(cloudsWeather2.rgb, 1), float4((finalColor.xyz * IN.color_0.rgb) * Params.y, 1), IN.color_0, Params.yyyy);

    // OUT.color_0 = color; // lerp(float3(0.2, 0.2, 0.3) * TESR_DebugVar.y, color.rgb, dayLight);
    // OUT.color_0.a = lerp(color.a * TESR_DebugVar.x, color.a * 0.7, dayLight); 

    // OUT.color_0.rgb = selectColor(TESR_DebugVar.w, color, OUT.color_0, black, sunDisk.xxx, normal, finalColor.aaa, finalColor, finalColor, cloudsWeather2, float4((finalColor.xyz * IN.color_0.rgb) * Params.y, 1));
    OUT.color_0.rgb = selectColor(TESR_DebugVar.w, color, normal, eyeDir.xyz, fresnel.xxx, shade(-eyeDir, normal).xxx, float3(cloudPanorama.xy, 1), greyScale.xxx, float(saturate(1 - athmosphere + sunDisk)).xxx, finalColor, float4((finalColor.xyz * IN.color_0.rgb) * Params.y, 1));
    // OUT.color_0.rgb = selectColor(TESR_DebugVar.w, color, normal, eyeDir.xyz, N, T, B, fresnel.xxx, shade(-eyeDir, normal).xxx, float3(cloudsWeather1.xy, 1), greyScale.xxx);

    // OUT.color_0.rgb = selectColor(TESR_DebugVar.w, color, OUT.color_0, black, sunDisk.xxx, cloudsWeatherBlend, finalColor.aaa, finalColor, finalColor, cloudsWeather2, float4((finalColor.xyz * IN.color_0.rgb) * Params.y, 1));
    OUT.color_0.a = color.a * TESR_DebugVar.z;


    return OUT;
};

// approximately 15 instruction slots used (2 texture, 13 arithmetic)
