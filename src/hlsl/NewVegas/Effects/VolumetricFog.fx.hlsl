// Volumetric Fog fullscreen shader for Oblivion/Skyrim Reloaded

float4 TESR_FogColor;
float4 TESR_FogData;  // weather values : x: fogNear, y:fogFar, z: sunglare, w:fogpower
float4 TESR_ReciprocalResolution;
float4 TESR_SunDirection;
float4 TESR_SunPosition;
float4 TESR_SunColor;
float4 TESR_SunDiskColor;
float4 TESR_SunAmbient;
float4 TESR_HorizonColor;
float4 TESR_SkyLowColor;
float4 TESR_SkyColor; // top sky color
float4 TESR_SkyData; // x: athmosphere thickness, y: sun influence, z: sun strength w: sky strength
float4 TESR_DebugVar; 
float4 TESR_SunsetColor; // color boost for sun when near the horizon
float4 TESR_SunAmount; // x: isDaytime
float4 TESR_VolumetricFogLow; // Low Fog
float4 TESR_VolumetricFogHigh; // General Fog
float4 TESR_VolumetricFogSimple; // Simple Fog
float4 TESR_VolumetricFogBlend; // Blend factor for each Fog
float4 TESR_VolumetricFogHeight; // Height of each Fog
float4 TESR_VolumetricFogData; // General shader settings

sampler2D TESR_SourceBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_RenderedBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

/*Height-based fog settings*/
static const float FOG_GROUND =	-10000;
static const float SUNINFLUENCE = 1/TESR_SkyData.y;
static const float nearFog = TESR_FogData.x;
static const float farFog = TESR_FogData.y;
static const float SunGlare = TESR_FogData.z;
static const float FogPower = TESR_FogData.w;

// scale settings for easier tuning
static const float BaseFogStrength = max(0, TESR_VolumetricFogData.x);
static const float FogAmount = max(0, TESR_VolumetricFogData.z);

static const float FogSaturation = max(0, TESR_VolumetricFogLow.x);
static const float WeatherImpact = max(0, TESR_VolumetricFogLow.y);
static const float DistantFogRange = clamp(TESR_VolumetricFogLow.z, 0.00000001, 1.0) * 0.5;
static const float SunPower = max(0, TESR_VolumetricFogLow.w);

static const float HeightFogDensity = max(0, TESR_VolumetricFogHigh.x);
static const float HeightFogFalloff = max(0, TESR_VolumetricFogHigh.y);
static const float HeightFogDist = TESR_VolumetricFogHigh.z;
static const float HeightFogSkyColor = 2 * TESR_VolumetricFogHigh.w;

static const float DistantFogHeight = TESR_VolumetricFogHeight.x;
static const float HeightFogHeight = TESR_VolumetricFogHeight.y;
static const float SimpleFogHeight = TESR_VolumetricFogHeight.z * 1.5;
static const float isExterior = TESR_VolumetricFogHeight.w; // 0 or 1 to activate/cancel fog in interiors

static const float Extinction = max(0.0, TESR_VolumetricFogSimple.x) * 0.1;
static const float Inscattering = max(0.0, TESR_VolumetricFogSimple.y) * 0.1;
static const float FogNight = max(0.0, TESR_VolumetricFogSimple.z);
static const float SimpleFogSkyColor = max(0, TESR_VolumetricFogSimple.w);

static const float DistantFogBlend = TESR_VolumetricFogBlend.x;
static const float HeightFogBlend = TESR_VolumetricFogBlend.y;
static const float HeightFogRolloff = TESR_VolumetricFogBlend.z;
static const float SimpleFogBlend = TESR_VolumetricFogBlend.w;

//static const float4x4 ditherMat = {{0.0588, 0.5294, 0.1765, 0.6471},
//									{0.7647, 0.2941, 0.8824, 0.4118},
//									{0.2353, 0.7059, 0.1176, 0.5882},
//									{0.9412, 0.4706, 0.8235, 0.3259}};

#include "Includes/Depth.hlsl"
#include "Includes/Helpers.hlsl"
#include "Includes/Sky.hlsl"


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


float3 getFog(float distance, float3 density){
	return 1 - exp(-distance * density * 0.00001);
}

// exponential fog based on https://iquilezles.org/articles/fog/
float3 getHeightFog(float distance, float falloff, float height, float heightOffset){
	float eyepos = TESR_CameraPosition.z + FOG_GROUND + (heightOffset) * 1000;
	float pointHeight = height - eyepos;
	pointHeight = max(pointHeight, 0.01f);

	return (1 / falloff) * exp(-eyepos * falloff) * (1.0 - exp(-distance * pointHeight * falloff)) / pointHeight;
}

float3 mixFog(float3 color, float3 fogColor, float3 extinctionColor, float3 inscatteringColor, float distance, float density){
	float3 extColor = getFog(distance, density * extinctionColor);
	float3 insColor = getFog(distance, density * inscatteringColor);
	return color * saturate(1 - extColor) + fogColor * saturate(insColor);
}

float3 mixHeightFog(float3 color, float3 fogColor, float3 extinctionColor, float3 inscatteringColor, float distance, float density, float falloff, float height, float offset){
	float fog = density * 0.00001 * getHeightFog(distance, falloff * 0.000000005f, height, offset);
	float3 extColor = fog * extinctionColor;
	float3 insColor = fog * inscatteringColor;
	return color * saturate(1 - extColor) + fogColor * saturate(insColor);
}


float getSky(float2 uv) {
	float4 color = linearize(tex2D(TESR_SourceBuffer, uv));

	float distance = 0.5 * TESR_ReciprocalResolution.xy;
	distance *= 1;

	float3 coeffs = float3(float2(1, -1) * distance, 0.9961);
    float depth = readDepth(uv) / farZ >= coeffs.z;
    float depth1 = readDepth(uv + coeffs.xx) / farZ >= coeffs.z;
    float depth2 = readDepth(uv + coeffs.xy) / farZ >= coeffs.z;
    float depth3 = readDepth(uv + coeffs.yx) / farZ >= coeffs.z;
    float depth4 = readDepth(uv + coeffs.yy) / farZ >= coeffs.z;

	float total = (depth || depth1 || depth2 || depth3 || depth4) * luma(color); // we scale the resutlt with the point luma since sky tends to be brighter

	return saturate(total);
}

// scale the fog color between sky, purefog, and sun contribution
float4 fogColor (float4 skyColor, float4 pureFogColor, float fogStrength, float skyColorCoeff, float4 sunScattering, float saturation){
	float4 fogColor = lerp(skyColor, pureFogColor, saturate(pows(fogStrength, skyColorCoeff)));    // modulate between sky and pure fog color if strength is high
	fogColor += sunScattering;                              									   // add sun influence
	fogColor = lerp(luma(fogColor), fogColor, saturation/(1 + fogStrength));                       // boost fog color saturation
	return fogColor;
}


float4 VolumetricFog(VSOUT IN) : COLOR0 
{
	float4 color = linearize(tex2D(TESR_SourceBuffer, IN.UVCoord));
	float4 pureFogColor = linearize(TESR_FogColor);

    float depth = readDepth(IN.UVCoord);
    float isDayTime = smoothstep(0.4, 0.8, TESR_SunAmount.x);
    float isDayTimeFog = smoothstep(0.1, 0.6, TESR_SunAmount.x);
	float SunsetFog = sin(TESR_SunAmount.x * PI) * 0.001 * isExterior;

	float3 eyeVector = toWorld(IN.UVCoord) * depth;
	float3 worldPos = eyeVector + TESR_CameraPosition.xyz;
    float pointHeight = worldPos.z;
	float3 eyeDirection = normalize(eyeVector);
	depth = length(eyeVector); // make sure depth is the same at the center and edges of the screen

	float fogPower = FogPower * lerp(HeightFogRolloff, 1/FogNight, (1 - isDayTimeFog) * isExterior);    	// boost fog power at night to allow it to show up better
	float normalizedDepth = pows(depth / farZ, fogPower);													// apply a curve to the distance to simulate pushing back fog
	float fogDepth = normalizedDepth * farZ / (HeightFogDist * lerp(0.005, 1, isExterior));     			// scale distances > shorter distances = further out fog

	float strength = pows((saturate(1 - farFog/farZ) + saturate(1 - nearFog/farZ)) / 2, 2) / (fogPower + 1); // deduce a strength of density from near/far and power values
	strength = BaseFogStrength + WeatherImpact * strength + SunsetFog;     			                         // scale strength so that strength 0 = no fog and strength 1 = fully fogged scene
	float heightFade = exp( -pointHeight / (80000 * SimpleFogHeight));            			                 // height fade to ensure we don't shade the sky

	// default values used by interiors
	float isSky, distantFog, distantHeightFade = 0.0; 
	float4 skyColor = pureFogColor;
	float inScattering = Inscattering;
	float extinction = Extinction;
	float4 sun = black;
	float4 finalColor = color;

	// sun/sky/distant fog coloring specific to exteriors
	if (isExterior){
		// calculate sun & sky color
		float3 up = blue.xyz;
		float sunHeight = shade(TESR_SunPosition.xyz, up);
		float sunDir = dot(eyeDirection, TESR_SunPosition.xyz);
		float sunInfluence = pows(compress(sunDir), SUNINFLUENCE);
		float4 sunColor = float4(GetSunColor(sunHeight, 1, TESR_SunAmount.x, TESR_SunDiskColor.rgb, TESR_SunsetColor.rgb), 1);
		sunColor *= isDayTime * isExterior; // set nighttime sun color

		// skyColor.rgb = GetSkyColor(0.5, 1, sunHeight, sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, sunColor.rgb) * TESR_SunsetColor.w;
		skyColor.rgb = GetSkyColor(0.5, 1, sunHeight, sunInfluence, TESR_SkyData.z, TESR_SkyColor.rgb, TESR_SkyLowColor.rgb, TESR_HorizonColor.rgb, black) * TESR_SunsetColor.w; // remove the sun influence as we add it in the scattering

		// add sun influence/scattering. Influence is wider with distance to simulate scattering. Influence is stronger when the sun is low
		float sunStrength = SunGlare / (1 + 2 * strength);                            // scale sun contribution with sunglare param and fog thickness
		sunStrength = lerp(sunStrength, max(sunStrength * 2, 10), normalizedDepth);   // modulate sun strength with distance

		float sunScattering = pows(compress(sunDir), 2 + sunStrength); 				  // more scattering looking towards the sun direction
		sunScattering *= pow(1 - sunHeight, 2) * isDayTimeFog * SunGlare;             // more scattering when sun is low or sunglare is high
		sun = sunColor * sunScattering * SunPower * 100; 

		isSky = getSky(IN.UVCoord);

		// enforce some level of fog on the horizon to hide Z fighting & sky transition
		distantFog = saturate(pows(invlerps(min((1 - DistantFogRange), 0.999998), 0.99, depth / farZ), 2.5)); 	    // simple curve between a custom distance and the horizon
		distantHeightFade = (DistantFogHeight == 0) ? (!isSky) : exp( -pointHeight / (80000 * DistantFogHeight)); 	// at HeightFade set to 0, filter out the sky

		inScattering = inScattering * lerp(1, 0.5, isSky) + SunsetFog;        // lower inscattering in the sky
		extinction *= (!isSky) * (1 - distantFog * isDayTimeFog);             // only apply extinction on No sky zones, and not in the range of distant fog during the day
	}

	float4 simpleFogColor = fogColor(skyColor, pureFogColor, strength, SimpleFogSkyColor, black, FogSaturation);
	float3 simpleFog = mixFog(finalColor.rgb, simpleFogColor.rgb, extinction, inScattering, fogDepth, strength * heightFade);
	finalColor = lerp(finalColor, float4(simpleFog, 1), saturate(SimpleFogBlend) * (1 - isSky));

	finalColor = lerp (finalColor, skyColor, distantFog * saturate(DistantFogBlend) * distantHeightFade * isExterior);

	float4 heightFogColor = fogColor(skyColor, pureFogColor, strength, HeightFogSkyColor, sun, FogSaturation);
	float3 heightFog = mixHeightFog(finalColor.rgb, heightFogColor.rgb, extinction, inScattering, fogDepth, strength * HeightFogDensity, 1.5 / (fogPower * HeightFogFalloff), pointHeight, HeightFogHeight);
	finalColor = lerp(finalColor, float4(heightFog, 1), saturate(HeightFogBlend));

	finalColor = lerp(color, finalColor, FogAmount);

	return delinearize(finalColor);
}

technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader  = compile ps_3_0 VolumetricFog();
	}
}
