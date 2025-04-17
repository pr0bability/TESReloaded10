// Image space shadows shader for Oblivion Reloaded

float4x4 TESR_WorldViewProjectionTransform;
float4x4 TESR_ShadowCameraToLightTransformNear;
float4x4 TESR_ShadowCameraToLightTransformMiddle;
float4x4 TESR_ShadowCameraToLightTransformFar;
float4x4 TESR_ShadowCameraToLightTransformLod;
float4 TESR_ReciprocalResolution;
float4 TESR_SmoothedSunDir;
float4 TESR_ViewSpaceLightDir;
float4 TESR_ShadowData; // x: quality, y: darkness, z: texel size
float4 TESR_ShadowFormatData; // x: mode, y: format bits per pixels
float4 TESR_ShadowScreenSpaceData; // x: Enabled, y: blurRadius, z: renderDistance, w: intensity
float4 TESR_SunAmbient;
float4 TESR_ShadowFade; // x: sunset attenuation, y: shadows maps active, z: point lights shadows active
float4 TESR_ShadowNearCenter; // x,y,z: center (world space), w: radius
float4 TESR_ShadowMiddleCenter; // x,y,z: center (world space), w: radius
float4 TESR_ShadowFarCenter; // x,y,z: center (world space), w: radius
float4 TESR_ShadowLodCenter; // x,y,z: center (world space), w: radius

sampler2D TESR_DepthBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_ShadowAtlas : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_PointShadowBuffer : register(s3)  = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NoiseSampler : register(s4) < string ResourceName = "Effects\bluenoise256.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#define SSS_STEPNUM 5

static const float DARKNESS = 1-TESR_ShadowData.y;
static const float SSS_DIST = 2000;
static const float SSS_THICKNESS = 20;
static const float SSS_MAXDEPTH = TESR_ShadowScreenSpaceData.z * TESR_ShadowScreenSpaceData.x;

static const float Mode = TESR_ShadowFormatData.x;
static const float FormatBits = TESR_ShadowFormatData.y;

static const float NormalBias = 1.0f;

struct VSOUT
{
	float4 vertPos : POSITION;
	float4 normal : TEXCOORD1;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"
#include "Includes/Shadows.hlsl"
#include "Includes/Normals.hlsl"
#include "Includes/BlurDepth.hlsl"


VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT = (VSOUT)0.0f;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

float4 ScreenCoordToTexCoord(float4 coord){
	// apply perspective (perspective division) and convert from -1/1 to range to 0/1 (shadowMap range);
	coord.xyz /= coord.w;
	coord.x = coord.x * 0.5f + 0.5f;
	coord.y = coord.y * -0.5f + 0.5f;

	return coord;
}

float GetLightAmountValue(float4x4 lightTransform, float4 coord, float offsetX, float offsetY, float bias, float bleedReduction) {
    float4 LightSpaceCoord = ScreenCoordToTexCoord(mul(coord, lightTransform));
	
	// Offset to the correct position in the atlas.
    LightSpaceCoord.xy *= 0.5;
    LightSpaceCoord.x += offsetX;
    LightSpaceCoord.y += offsetY;
	
    float4 shadowBufferValue = tex2D(TESR_ShadowAtlas, LightSpaceCoord.xy);
	
    float shadow;
	
	[branch]
    if (Mode == 0.0f)
        shadow = GetLightAmountValueVSM(shadowBufferValue.xy, LightSpaceCoord.z, bias, bleedReduction);
    else if (Mode == 1.0f)
        shadow = GetLightAmountValueEVSM2(shadowBufferValue.xy, LightSpaceCoord.z, bias, bleedReduction, FormatBits);
	else
        shadow = GetLightAmountValueEVSM4(shadowBufferValue, LightSpaceCoord.z, bias, bleedReduction, FormatBits);
	
    return shadow;
}

float GetLightAmount(float4 positionWS, float3 normal)
{
	// Normal offset.
    float NdotL = dot(normal, TESR_SmoothedSunDir.xyz);
    float offsetScale = saturate(1 - NdotL);
    float3 normalOffset = offsetScale * NormalBias * normal;
    
    float4 samplePos = float4(positionWS.xyz + normalOffset.xyz, 1.0f);
	
    const float bias = Mode == 0.0f ? 0.00001f : 0.01f;
	
    const float blend = 0.9f;
	
	float4 shadows = {
        GetLightAmountValue(TESR_ShadowCameraToLightTransformNear, samplePos, 0.0, 0.0, bias, 0.1f),
		GetLightAmountValue(TESR_ShadowCameraToLightTransformMiddle, samplePos, 0.5, 0.0, bias, 0.2f),
		GetLightAmountValue(TESR_ShadowCameraToLightTransformFar, samplePos, 0.0, 0.5, bias, 0.6f),
		GetLightAmountValue(TESR_ShadowCameraToLightTransformLod, samplePos, 0.5, 0.5, bias, 0.8f),
    };
	
    float4 distances = {
        length(positionWS.xyz - TESR_ShadowNearCenter.xyz),
		length(positionWS.xyz - TESR_ShadowMiddleCenter.xyz),
		length(positionWS.xyz - TESR_ShadowFarCenter.xyz),
		length(positionWS.xyz - TESR_ShadowLodCenter.xyz),
    };
	
    if (distances.x < TESR_ShadowNearCenter.w) {
        if (distances.x < TESR_ShadowNearCenter.w * blend)
            return shadows.x;
		
        return lerp(shadows.x, shadows.y, smoothstep(TESR_ShadowNearCenter.w * blend, TESR_ShadowNearCenter.w, distances.x));
    }
    else if (distances.y < TESR_ShadowMiddleCenter.w) {
        if (distances.y < TESR_ShadowMiddleCenter.w * blend)
            return shadows.y;
		
        return lerp(shadows.y, shadows.z, smoothstep(TESR_ShadowMiddleCenter.w * blend, TESR_ShadowMiddleCenter.w, distances.y));
    }
    else if (distances.z < TESR_ShadowFarCenter.w) {
        if (distances.z < TESR_ShadowFarCenter.w * blend)
            return shadows.z;
		
        return lerp(shadows.z, shadows.w, smoothstep(TESR_ShadowFarCenter.w * blend, TESR_ShadowFarCenter.w, distances.z));
    }
    else if (distances.w < TESR_ShadowLodCenter.w) {
        if (distances.w < TESR_ShadowLodCenter.w * blend)
            return shadows.w;
		
        return lerp(shadows.w, 1.0f, smoothstep(TESR_ShadowLodCenter.w * blend, TESR_ShadowLodCenter.w, distances.w));
    }
    else {
        return 1.0f;
    }
}

// returns a semi random float3 between 0 and 1 based on the given seed. (blue noise)
// tailored to return a different value for each uv coord of the screen.
float3 random(float2 seed)
{
	return tex2D(TESR_NoiseSampler, (seed/256 + 0.5) / TESR_ReciprocalResolution.xy).xyz;
}

float4 ScreenSpaceShadow(VSOUT IN) : COLOR0
{	
	// calculates wether a point is in shadow based on screen depth
	float2 uv = IN.UVCoord;
	// clip((uv < 0.5) - 1);
	// uv *= 2;

    float4 color = tex2D(TESR_PointShadowBuffer, IN.UVCoord);
	if (!TESR_ShadowScreenSpaceData.x) return float4(1.0, color.g, 0, 1); // skip is screenspace shadows are disabled

	float3 pos = reconstructPosition(uv);// + expand(random3); 

	float bias = 0.01;
	if (pos.z > SSS_MAXDEPTH) return float4(1.0, color.g, 0, 1); // early out for pixels further away than the max render distance
	
    float3 random3 = random(uv);
    float rand = lerp(min(0.8f, pos.z / SSS_MAXDEPTH), 1.0f, random3.r); // some noise to vary the ray length

	// scale the step with distance, and randomize length
	float depth = getHomogenousDepth(uv) / farZ;
	float3 step = pows(depth, 0.6) * (SSS_DIST / SSS_STEPNUM) * TESR_ViewSpaceLightDir.xyz * rand;
	float thickness = pows(depth, 0.6) * SSS_THICKNESS;

	float occlusion = 0.0;
	float total = 0;

	// Doing two steps at once to optimize the depth march
	[unroll]
	for (float i = 1; i < SSS_STEPNUM; i+=2){
		float step1 = i;
		float step2 = i + 1;

		float3 pos1 = pos + step1 * step; // we move to the light with bigger steps each time
		float3 pos2 = pos1 + step2 * step; // we move to the light with bigger steps each time
		
		// if (screen_pos.x > 0 && screen_pos.x < 1.0 && screen_pos.y > 0 && screen_pos.y <1){
		float2 depth = {pos1.z, pos2.z};
		float2 depthCompare = {
			readDepth(projectPosition(pos1).xy),
			readDepth(projectPosition(pos2).xy),
		};

		float2 depthDelta = depth - depthCompare;

		occlusion += (depthDelta.x > bias && depthDelta.x < SSS_THICKNESS)/step1; // in Shadow
		occlusion += (depthDelta.y > bias && depthDelta.y < SSS_THICKNESS)/step2; // in Shadow
		pos = pos2; 
		total += 1/step1 + 1/step2; // weight samples inversely with distance
	}

    occlusion = pows(occlusion / total, 0.3); // get an average shading based on total weights
	

    // save result of SSS in red channel, and fade contribution with distance
    color.r = lerp(1.0f - occlusion, 1.0, smoothstep(SSS_MAXDEPTH * 0.8, SSS_MAXDEPTH, pos.z));
    return color;
}

// returns a shadow value from darkness setting value (full shadow) to 1 (full light)
float4 Shadow(VSOUT IN) : COLOR0
{
	float2 uv = IN.UVCoord;

    float viewDepth;
    float4 worldPos = reconstructWorldPosition(uv, viewDepth);

	// Sample Screen Space shadows
	float4 Shadow = tex2D(TESR_PointShadowBuffer, IN.UVCoord);
    Shadow = pow(Shadow, TESR_ShadowScreenSpaceData.w);
	if (!TESR_ShadowFade.y) return Shadow; // disable shadow maps if ShadowFade.y == 0 (setting for shadow map disabled)

	// Sample shadows from shadowmaps
    float3 normal = GetWorldNormal(uv);
	float sunShadows = GetLightAmount(worldPos, normal); 

	Shadow.r = min(Shadow.r, sunShadows); // get the darkest between Screenspace & Sun shadows

	return Shadow;
}


technique {

	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScreenSpaceShadow();
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
	 	PixelShader = compile ps_3_0 DepthBlur(TESR_PointShadowBuffer, OffsetMaskH, TESR_ShadowScreenSpaceData.y, 3500, SSS_MAXDEPTH);
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
	 	PixelShader = compile ps_3_0 DepthBlur(TESR_PointShadowBuffer, OffsetMaskV, TESR_ShadowScreenSpaceData.y, 3500, SSS_MAXDEPTH);
	}

    pass {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Shadow();
    }

}
