// WetWorld fullscreen shader for Oblivion/NewVegas Reloaded
//----------------------------------------------------------
//Todo:
//----------------------------------------------------------
// Fix viewmodel issue
// Look into adding code from refraction for better looking water(?)
// Reflection buffer(?)
// Make better ripple normal(?)
// Add ripples to other water sources(?)
// Add world spec multi(?)
//----------------------------------------------------------
// Boomstick was here

float4 TESR_SunDirection;
float4 TESR_ReciprocalResolution;
float4x4 TESR_WorldViewProjectionTransform;
float4x4 TESR_ShadowCameraToLightTransformOrtho;

float4 TESR_GameTime;
float4 TESR_SkyColor;
float4 TESR_HorizonColor;
float4 TESR_SunColor;
float4 TESR_SunAmbient;
float4 TESR_OrthoData; // max ortho radius
float4 TESR_WetWorldCoeffs; // Puddle color R, G, B + spec multiplier
float4 TESR_WaterSettings; // for water height to avoid rendering puddles underwater
float4 TESR_WetWorldData; // x: current rain amount, y: max rain amount, z: puddle amount, w:puddle darkness/intensity
float4 TESR_SunPosition;
float4 TESR_SkyData;
float4 TESR_SkyLowColor;
float4 TESR_SunAmount;
float4 TESR_LightPosition[12];
float4 TESR_ShadowLightPosition[12];
float4 TESR_LightColor[24];
float4 TESR_DebugVar;

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_SourceBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_RippleSampler : register(s3) < string ResourceName = "Precipitations\ripples.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_OrthoMapBuffer : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };
sampler2D TESR_PointShadowBuffer : register(s6) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBufferViewModel : register(s7) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };


//------------------------------------------------------
// Custimizable
//------------------------------------------------------
static const float SUNINFLUENCE = 1/TESR_SkyData.y;
static const float PI = 3.14159265;
static const float time1 = TESR_GameTime.z * 0.96f; // Ripple timing, make sure to offset each by atleast a few
static const float time2 = TESR_GameTime.z * 0.97f; // Ripple timing, original 1.0-1.4
static const float time3 = TESR_GameTime.z * 0.98f; // Ripple timing
static const float time4 = TESR_GameTime.z * 0.99f; // Ripple timing
static const float DrawD = TESR_OrthoData.x * 0.9; // Max draw distance for puddles 0-1000000
static const float rippleScale = 120.0f;
// static const float rippleScale = 80.0f;
static const float refractionScale = 25;
//------------------------------------------------------

struct VSOUT
{
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"
#include "Includes/Blur.hlsl"
#include "Includes/Normals.hlsl"
#include "Includes/Sky.hlsl"
#include "Includes/PBR.hlsl"


float3 ComputeRipple(float2 UV, float CurrentTime, float Weight)
{
    float4 Ripple = tex2D(TESR_RippleSampler, UV);
	
    Ripple.yz = expand(Ripple.yz); // convert from 0/1 to -1/1 

    float period = frac(Ripple.w + CurrentTime);
    float TimeFrac = period - 1.0f + Ripple.x;
    float DropFactor = saturate(0.2f + Weight * 0.8f - period);
    float FinalFactor = DropFactor * Ripple.x * sin( clamp(TimeFrac * 9.0f, 0.0f, 3.0f) * PI);

    return float3(Ripple.yz * FinalFactor * 0.35f, 1.0f);
}


float3 getPointLightSpecular(float3 surfaceNormal, float4 lightPosition, float3 worldPosition, float3 eyeDirection, float3 specColor, float roughness){
	if (lightPosition.w == 0) return float3(0, 0, 0);
	float specularBoost = 10;
	float glossiness = 20;

	float3 lightDir = lightPosition.xyz - worldPosition;
	float distance = length(lightDir) / lightPosition.w;

	// radius based attenuation based on https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
	float s = saturate(distance * distance); 
	float atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));

	// return pows(shades(H, surfaceNormal), glossiness) * linearize(float4(specColor, 1)) * specularBoost * atten;
    lightDir = normalize(lightDir);
	float3 H = normalize(lightDir + eyeDirection);

    float NdotL = shades(surfaceNormal, lightDir);
    float NdotV = shades(surfaceNormal, eyeDirection);
    float NdotH = shades(surfaceNormal, H);

    float3 Ks = FresnelShlick(0.02, H, lightDir);
	return modifiedBRDF(roughness, NdotL, NdotV, NdotH, Ks) * linearize(specColor) * atten;
}


float4 ScreenCoordToTexCoord(float4 coord, float mult){
	// apply perspective (perspective division) and convert from -1/1 to range to 0/1 (shadowMap range);
	coord.xyz /= coord.w;
	coord.x = coord.x * 0.5f + 0.5f;
	coord.y = coord.y * -0.5f + 0.5f;
	coord *= mult;

	return coord;
}


float4 WetMap (VSOUT IN ) : COLOR0
{
	// sample the ortho map and detect pockets by sampling around the center and comparing depth
	float2 uv = IN.UVCoord;
	//float4 color = tex2D(TESR_SourceBuffer, uv);
	float bias = 0.000001;

	float radius = 50 * TESR_WetWorldData.z;// radius will increase with rain status
	float center = tex2D(TESR_OrthoMapBuffer, IN.UVCoord).r - bias;
	float left = tex2D(TESR_OrthoMapBuffer, IN.UVCoord + float2(0, -1) * TESR_ReciprocalResolution.xy * radius).r;
	float right = tex2D(TESR_OrthoMapBuffer, IN.UVCoord + float2(0, 1) * TESR_ReciprocalResolution.xy * radius).r;
	float top = tex2D(TESR_OrthoMapBuffer, IN.UVCoord + float2(-1, 0) * TESR_ReciprocalResolution.xy * radius).r;
	float bottom = tex2D(TESR_OrthoMapBuffer, IN.UVCoord + float2(1, 0) * TESR_ReciprocalResolution.xy * radius).r;

	float crease = (center > left && center > right && center > top && center > bottom);

	return float4(crease, center, 0, 1);
}


float4 BlurWetMap(VSOUT IN, uniform float2 OffsetMask, uniform float blurRadius) : COLOR0
{
	// blur puddles using a gaussian blur
	float WeightSum = 0.114725602f;
	float2 uv = IN.UVCoord;
	float color = tex2D(TESR_RenderedBuffer, uv).r * WeightSum;

    for (int i = 0; i < cKernelSize; i++)
    {
		float2 uvOff = (BlurOffsets[i] * OffsetMask) * blurRadius;
		color += BlurWeights[i] * tex2D(TESR_RenderedBuffer, uv + uvOff).r;
		WeightSum += BlurWeights[i];
    }
	color /= WeightSum;
    return float4(color.rrr, 1);
}


float4 Wet( VSOUT IN ) : COLOR0
{
	float4 baseColor = tex2D(TESR_SourceBuffer, IN.UVCoord);

	float depth = readDepth(IN.UVCoord);
	float3 eyeDirection = toWorld(IN.UVCoord);
	float3 camera_vector = eyeDirection * depth;
	float4 worldPos = float4(TESR_CameraPosition.xyz + camera_vector, 1.0f);
	float3 normal = GetWorldNormal(IN.UVCoord);
	float3 up = float3(0, 0, 1);
	float floorAngle = smoothstep(0.94,1, dot(normal, up));
	eyeDirection = normalize(eyeDirection);

	// early out to avoid computing pixels that aren't puddles
	float waterTreshold = (depth/farZ) * 500;
	float isWaterSurface = (floorAngle > 0.9) && (worldPos.z > TESR_WaterSettings.x - waterTreshold) && (worldPos.z < TESR_WaterSettings.x + waterTreshold);
    if (depth > DrawD || floorAngle == 0 || isWaterSurface) return baseColor;
	float viewmodelDepth = tex2D(TESR_DepthBufferViewModel, IN.UVCoord).x;
	if ((TESR_DepthConstants.z == 0 && viewmodelDepth < 0.9) || (TESR_DepthConstants.z > 0 && viewmodelDepth > 0.01)) return baseColor; // filter out viewmodel

	float LODfade = smoothstep(DrawD, 0, depth);
	float thickness = 0.003; // thickness of the valid areas around the ortho map depth that will receive the effect (cancels out too far above or below ortho value)

	// get puddle mask from ortho map
	float4 pos = mul(worldPos, TESR_WorldViewProjectionTransform);
	float4 ortho_pos = mul(pos, TESR_ShadowCameraToLightTransformOrtho);
	ortho_pos.xy = ScreenCoordToTexCoord(ortho_pos, 1).xy;
	float puddles = tex2D(TESR_RenderedBuffer, ortho_pos.xy).r; // puddles, ortho height
	float ortho = tex2D(TESR_OrthoMapBuffer, ortho_pos.xy).r; // puddles, ortho height

	float aboveGround = ortho_pos.z < ortho + thickness;
	float belowGround = ortho_pos.z > ortho - thickness;

	puddles = puddles * 2 * belowGround;
	float puddlemask = pow(puddles, 3);  // sharpen puddle mask to get the deeper part of the puddle
	puddlemask = saturate(puddlemask); 

	// sample and combine rain ripples
	float2 rippleUV = worldPos.xy / rippleScale;
	float4 Weights = float4(1, 0.75, 0.5, 0.25) * TESR_WetWorldData.x;
	Weights = saturate(Weights * 4);
	float3 Ripple1 = ComputeRipple(rippleUV + float2( 0.25f,0.0f), time1, Weights.x);
	float3 Ripple2 = ComputeRipple(rippleUV * 1.1 + float2(-0.55f,0.3f), time2, Weights.y);
	float3 Ripple3 = ComputeRipple(rippleUV * 1.3 + float2(0.6f, 0.85f), time3, Weights.z);
	float3 Ripple4 = ComputeRipple(rippleUV * 1.5 + float2(0.5f,-0.75f), time4, Weights.w);

	float4 Z = lerp(1, float4(Ripple1.z, Ripple2.z, Ripple3.z, Ripple4.z), Weights);
	float3 ripple = float3( Weights.x * Ripple1.xy + Weights.y * Ripple2.xy + Weights.z * Ripple3.xy + Weights.w * Ripple4.xy, Z.x * Z.y * Z.z * Z.w);
	float3 ripnormal = normalize(ripple);
	// float3 combinedNormals = float3(ripnormal.xy, ripnormal.z); //only add ripple to surfaces that match ortho depth
	float3 combinedNormals = ripnormal; //only add ripple to surfaces that match ortho depth

	combinedNormals = normalize(lerp(blue.xyz, combinedNormals, aboveGround * LODfade));

	// refract image through ripple normals
	float2 refractionUV = expand(projectPosition(combinedNormals)).xy * TESR_ReciprocalResolution.xy * (refractionScale);
	float4 rippleColor = linearize(tex2D(TESR_SourceBuffer, refractionUV + IN.UVCoord)); 

	// sample and strenghten the shadow map
	float sunAmbient = luma(linearize(TESR_SunAmbient));
	float4 shadows = tex2D(TESR_PointShadowBuffer, IN.UVCoord);
	float inShadow = saturate(pow((shadows.r + shadows.g) / sunAmbient, 5));

	// calculate sky color
    float sunHeight = shade(TESR_SunPosition.xyz, up);
    float sunDir = dot(eyeDirection, TESR_SunPosition.xyz);
	float sunInfluence = pows(compress(sunDir), SUNINFLUENCE);
	float3 fresnelColor = GetSkyColor(0, 0.5, sunHeight, sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, TESR_SunColor.rgb);

	// calculate puddle color
	float3 puddleColor = rippleColor.rgb * lerp(1, 0.5, TESR_WetWorldData.w); // base color is just darkened ground color

	float3 halfwayDir = normalize(TESR_SunDirection.xyz - eyeDirection);
	float glossiness = 1000;
	// float specularMask = saturate(puddlemask + invlerp(1.0, 0.98, combinedNormals.z) * 0.5);
	float specularMask = saturate(puddlemask + pow((1 - shades(combinedNormals, blue.xyz)), 0.5));

	float4 sunColor = linearize(TESR_SunColor);
	// float3 specular = pow(shades(combinedNormals, halfwayDir), lerp(1, 5, specularMask)) * inShadow * sunColor * 1000;

	float fresnel = lerp(0, pow(1 - dot(-eyeDirection, combinedNormals), 5) * inShadow, 0.5 * TESR_WetWorldData.w);
	float3 Ks = FresnelShlick(0.02, halfwayDir, TESR_SunDirection.xyz);

	float3 puddleNormal = normalize(lerp(normal, combinedNormals, specularMask));
	float roughness = lerp(TESR_DebugVar.x, TESR_DebugVar.y, puddlemask);
	// float roughness = lerp(0.00015, 0.00007, puddlemask);
	float3 specular = modifiedBRDF(lerp(TESR_DebugVar.x * 0.00015, TESR_DebugVar.y * 0.0001, puddlemask), shades(puddleNormal, sunDir), shades(puddleNormal, eyeDirection), shades(puddleNormal, halfwayDir), Ks);
	// float3 specular = PBR(0, 0.0002, fresnelColor, puddleNormal, eyeDirection, TESR_SunDirection.xyz, sunColor.rgb * 5);

	for (int i=0; i < 12; i++){
		specular += getPointLightSpecular(combinedNormals, TESR_ShadowLightPosition[i], worldPos.rgb, eyeDirection, TESR_LightColor[i].rgb, roughness);
		specular += getPointLightSpecular(combinedNormals, TESR_LightPosition[i], worldPos.rgb, eyeDirection, TESR_LightColor[i+12].rgb, roughness);
	}

	// transition between surface ripple and deeper puddles
	puddleColor = lerp(rippleColor.rgb, puddleColor.rgb, puddlemask);

	// float4 color = float4(lerp(puddleColor, fresnelColor, fresnel * specularMask), 1.0);
	// float4 color = float4(lerp(puddleColor, fresnelColor, fresnel * specularMask), 1.0);
	float4 color = float4(puddleColor, 1);
	// color.rgb += specular * specularMask * 1000;
	color.rgb += specular * sunColor.rgb * specularMask * inShadow * lerp(10, 50, saturate(sunHeight));
	
	color = delinearize(color);
    return float4(lerp(baseColor.rgb, color.rgb, LODfade), 1); // fade out puddles
}


technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 WetMap();
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 BlurWetMap(float2(1.0f, 0.0f), 2);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 BlurWetMap(float2(0.0f, 1.0f), 2);
	}
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Wet();
	}
}
