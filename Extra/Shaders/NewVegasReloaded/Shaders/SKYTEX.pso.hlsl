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
float4 TESR_ReciprocalResolution : register(c13);


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
static const float sunInfluence = 1/TESR_SkyData.y;

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
#include "Includes/Position.hlsl"

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

    float3 up = float3(0, 0, 1);
    float2 uv = IN.position.xy * TESR_ReciprocalResolution.xy;
    float3 eyeDir = normalize(IN.location);
    float verticality = pows(compress(dot(eyeDir, up)), 3);
    float sunHeight = shade(TESR_SunDirection.xyz, up);

    float sunDir = dot(eyeDir, TESR_SunDirection.xyz);
    float athmosphere = pows(1 - verticality, 8) * TESR_SkyData.x;
    float sunInfluence = pows(compress(sunDir), sunInfluence);
    float sunDisk = pows(saturate(sunDir), 200);

    float cloudsPower = Params.x;
    float4 cloudsWeather1 = tex2D(TexMap, IN.TexUV.xy);
    float4 cloudsWeather2 = tex2D(TexMapBlend, IN.TexBlendUV.xy);

    float4 cloudsWeatherBlend = lerp(cloudsWeather1, cloudsWeather2, cloudsPower); // weather transition

    float4 finalColor = (weight(cloudsWeather1.xyz) == 0.0 ? cloudsWeather2 : (weight(cloudsWeather2.xyz) == 0.0 ? cloudsWeather1 : cloudsWeatherBlend)); // select either weather or blend
    finalColor.a = cloudsWeatherBlend.a;

    if (IN.color_1.r){ // early out if this texture is sun/moon
        OUT.color_0 = float4(finalColor.rgb * IN.color_0.rgb * Params.y, finalColor.w * IN.color_0.a);
        // OUT.color_0 = float4(IN.color_1.rgb, finalColor.w * IN.color_0.a);
        return OUT;
    }


    // shade clouds 
    // float cloudMask = saturate(1 - (athmosphere * 0.8) + sunDisk);
    // float alpha = lerp(0, finalColor.w * TESR_CloudData.z, cloudMask);

    float greyScale = lerp(luma(finalColor), 1, TESR_CloudData.w);
    float alpha = finalColor.w * TESR_CloudData.z;
    float3 scattering = sunDir * (1 - alpha) * TESR_SunColor.rgb * 0.3;
    float4 color;

    if ( UseNormals){
        // Tests for normals lit cloud
        float2 normal2D = finalColor.xy; // normal x and y are in red and green, blue is reconstructed
        normal2D = finalColor.xy * (1-sunDisk); // cancel out normals strenght around the sun location

        float3 normal = getNormal(normal2D, -eyeDir); // reconstruct world normal from red and green

        greyScale = lerp(TESR_CloudData.w, 1, finalColor.z); // greyscale is stored in blue channel
        float3 diffuse = shade(normal, TESR_SunDirection.xyz) * TESR_SunColor.rgb;
        float3 ambient = lerp(TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, greyScale) * 0.6;
        float3 fresnel = pow(1 - shade(-eyeDir, normal), 5) * sunDir * TESR_SunColor.rgb * 0.1;
        float3 bounce = shade(normal, -up) * TESR_HorizonColor.rgb * 0.2; // light from the ground bouncing up to the underside of clouds

        color = float4(ambient + diffuse + fresnel + scattering + bounce, alpha);
        // color.a = lerp(0, finalColor.a * TESR_CloudData.z, cloudMask);
        color.a = finalColor.a * TESR_CloudData.z;
        color.rgb = lerp(color, finalColor, saturate(sunDisk)).rgb * IN.color_0.rgb * Params.y;
        // color.rgb = lerp(color, finalColor, saturate(sunDisk)); // cancel out effect for sun texture area
    } else {
        // simply tint the clouds
        float3 cloudTint = lerp(pow(TESR_SkyLowColor, 5.0), TESR_SunColor * 1.5, saturate(((1 - pow(compress(sunDir), 3.0))) * saturate(greyScale))).rgb;
        cloudTint = lerp(cloudTint, white.rgb, sunHeight); // tint the clouds less when the sun is high in the sky

        float dayLight = saturate(luma(TESR_SunColor));

        finalColor.rgb *= cloudTint * TESR_CloudData.w;
        finalColor.rgb += scattering;

        // color = float4((finalColor.xyz) * Params.y, finalColor.w * IN.color_0.a);
        // color = float4(lerp(finalColor.rgb, finalColor.rgb * IN.color_0.rgb, 0.5 + 0.5 * sunDisk) * Params.y, finalColor.w * IN.color_0.a);
        color = float4(finalColor.rgb * IN.color_0.rgb * Params.y, finalColor.w * IN.color_0.a);
        color.a = lerp(color.a, color.a * 0.7, dayLight); // add setting for cloud alpha for daytime
    }
    
    OUT.color_0.rgb = color.rgb;
    OUT.color_0.a = color.a;

    return OUT;
};
