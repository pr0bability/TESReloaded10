// Image space shadows shader for Oblivion Reloaded

float4x4 TESR_WorldViewProjectionTransform;
float4x4 TESR_ShadowCameraToLightTransformNear;
float4x4 TESR_ShadowCameraToLightTransformMiddle;
float4x4 TESR_ShadowCameraToLightTransformFar;
float4x4 TESR_ShadowCameraToLightTransformLod;
float4 TESR_ReciprocalResolution;
float4 TESR_ViewSpaceLightDir;
float4 TESR_ShadowData; // x: quality, y: darkness, z: nearmap resolution, w: farmap resolution
float4 TESR_ShadowScreenSpaceData; // x: Enabled, y: blurRadius, z: renderDistance
float4 TESR_ShadowRadius; // radius of the 4 cascades
float4 TESR_SunAmbient;
float4 TESR_SunColor;
float4 TESR_ShadowFade; // x: sunset attenuation, y: shadows maps active, z: point lights shadows active

sampler2D TESR_DepthBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
sampler2D TESR_ShadowMapBufferNear : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
sampler2D TESR_ShadowMapBufferMiddle : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
sampler2D TESR_ShadowMapBufferFar : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
sampler2D TESR_ShadowMapBufferLod : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = ANISOTROPIC; MIPFILTER = LINEAR; };
sampler2D TESR_NormalsBuffer : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_PointShadowBuffer : register(s6)  = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_NoiseSampler : register(s7) < string ResourceName = "Effects\bluenoise256.dds"; > = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

#define SSS_STEPNUM 10

static const float DARKNESS = 1-TESR_ShadowData.y;
static const float SSS_DIST = 40;
static const float SSS_THICKNESS = 20;
static const float SSS_MAXDEPTH = TESR_ShadowScreenSpaceData.z * TESR_ShadowScreenSpaceData.x;

// blur constants
#define cKernelSize 12
static const float2 OffsetMaskH = float2(1.0f, 0.0f);
static const float2 OffsetMaskV = float2(0.0f, 1.0f);

static const float BlurWeights[cKernelSize] = 
{
	0.057424882f,
	0.058107773f,
	0.061460144f,
	0.071020611f,
	0.088092873f,
	0.106530916f,
	0.106530916f,
	0.088092873f,
	0.071020611f,
	0.061460144f,
	0.058107773f,
	0.057424882f
};
 
static const float2 BlurOffsets[cKernelSize] = 
{
	float2(-6.0f * TESR_ReciprocalResolution.x, -6.0f * TESR_ReciprocalResolution.y),
	float2(-5.0f * TESR_ReciprocalResolution.x, -5.0f * TESR_ReciprocalResolution.y),
	float2(-4.0f * TESR_ReciprocalResolution.x, -4.0f * TESR_ReciprocalResolution.y),
	float2(-3.0f * TESR_ReciprocalResolution.x, -3.0f * TESR_ReciprocalResolution.y),
	float2(-2.0f * TESR_ReciprocalResolution.x, -2.0f * TESR_ReciprocalResolution.y),
	float2(-1.0f * TESR_ReciprocalResolution.x, -1.0f * TESR_ReciprocalResolution.y),
	float2( 1.0f * TESR_ReciprocalResolution.x,  1.0f * TESR_ReciprocalResolution.y),
	float2( 2.0f * TESR_ReciprocalResolution.x,  2.0f * TESR_ReciprocalResolution.y),
	float2( 3.0f * TESR_ReciprocalResolution.x,  3.0f * TESR_ReciprocalResolution.y),
	float2( 4.0f * TESR_ReciprocalResolution.x,  4.0f * TESR_ReciprocalResolution.y),
	float2( 5.0f * TESR_ReciprocalResolution.x,  5.0f * TESR_ReciprocalResolution.y),
	float2( 6.0f * TESR_ReciprocalResolution.x,  6.0f * TESR_ReciprocalResolution.y)
};

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

float GetLightAmountValue(sampler2D shadowBuffer, float4x4 lightTransform, float4 coord){
	// Quality : shadow technique
	// 0: disabled
	// 1: VSM
	// 2: simple ESM
	// 3: filtered ESM

	float4 LightSpaceCoord = ScreenCoordToTexCoord(mul(coord, lightTransform));
	float4 shadowBufferValue = tex2D(shadowBuffer, LightSpaceCoord.xy);

	float4 shadowMode = {TESR_ShadowData.w == 1.0f, TESR_ShadowData.w == 2.0f, TESR_ShadowData.w == 3.0f, TESR_ShadowData.w == 4.0f};
	float4 shadows = {
		GetLightAmountValueVSM(shadowBufferValue.xy, LightSpaceCoord.z),
		GetLightAmountValueESM(shadowBufferValue.x, LightSpaceCoord.z),
		GetLightAmountValueESSM(shadowBufferValue.x, LightSpaceCoord.z),
		0.0,
	};
	return dot(shadows, shadowMode);
}

float GetLightAmount(float4 coord, float depth)
{
	float blendArea = 0.9; // 20 % of each cascade to overlap
	float shadow;

	// getting all shadow values from cascades as negative (to be able to use the dot product to chose the correct one)
	// shadows are inverted to mean 1 if in shadow to allow dot product filtering
	float4 shadows = {
		1 - GetLightAmountValue(TESR_ShadowMapBufferNear, TESR_ShadowCameraToLightTransformNear, coord),
		1 - GetLightAmountValue(TESR_ShadowMapBufferMiddle, TESR_ShadowCameraToLightTransformMiddle, coord),
		1 - GetLightAmountValue(TESR_ShadowMapBufferFar, TESR_ShadowCameraToLightTransformFar, coord),
		1 - GetLightAmountValue(TESR_ShadowMapBufferLod, TESR_ShadowCameraToLightTransformLod, coord),
	};

	float4 cascade = {
		depth < TESR_ShadowRadius.x,
		depth >= TESR_ShadowRadius.x * blendArea && depth < TESR_ShadowRadius.y,
		depth >= TESR_ShadowRadius.y * blendArea && depth < TESR_ShadowRadius.z,
		depth >= TESR_ShadowRadius.z * blendArea && depth < TESR_ShadowRadius.w,
	};

	// calculate blending areas coefficients between cascades
	float4 fadeIn = {
		1.0,
		clamp(0.0, 1.0, invlerp(blendArea * TESR_ShadowRadius.x, TESR_ShadowRadius.x, depth)),
		clamp(0.0, 1.0, invlerp(blendArea * TESR_ShadowRadius.y, TESR_ShadowRadius.y, depth)),
		clamp(0.0, 1.0, invlerp(blendArea * TESR_ShadowRadius.z, TESR_ShadowRadius.z, depth)),
	};

	float4 fadeOut = {
		clamp(0.0, 1.0, 1 - invlerp(blendArea * TESR_ShadowRadius.x, TESR_ShadowRadius.x, depth)),
		clamp(0.0, 1.0, 1 - invlerp(blendArea * TESR_ShadowRadius.y, TESR_ShadowRadius.y, depth)),
		clamp(0.0, 1.0, 1 - invlerp(blendArea * TESR_ShadowRadius.z, TESR_ShadowRadius.z, depth)),
		// clamp(0.0, 1.0, 1 - smoothstep(TESR_ShadowRadius.z, TESR_ShadowRadius.w, depth)),
		clamp(0.0, 1.0, 1), 
	};

	// apply blending to each cascade shadow
	shadows *= fadeIn * fadeOut;

	// filter the shadow based on the current valid cascades
	return 1 - dot(shadows, cascade);
}

// returns a semi random float3 between 0 and 1 based on the given seed.
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

	float rand = lerp(0.2, 1, random(uv).r); // some noise to vary the ray length
	float3 pos = reconstructPosition(uv) ; 

	float bias = 0.0;
	if (pos.z > SSS_MAXDEPTH) return float4(1.0, color.g, 0, 1); // early out for pixels further away than the max render distance

	float occlusion = 0.0;
	[unroll]
	// Doing two steps at once to optimize the depth march
	for (float i = 1; i < SSS_STEPNUM/2; i++){
		float4 step = SSS_DIST / SSS_STEPNUM * i * TESR_ViewSpaceLightDir * rand;

		float3 pos1 = pos + step.xyz; // we move to the light
		float3 pos2 = pos1 + step.xyz; // we move to the light
		float3 screen_pos1 = projectPosition(pos1);
		float3 screen_pos2 = projectPosition(pos2);
		
		// if (screen_pos.x > 0 && screen_pos.x < 1.0 && screen_pos.y > 0 && screen_pos.y <1){
		float2 depth = {pos1.z, pos2.z};
		float2 depthCompare = {
			readDepth(screen_pos1.xy),
			readDepth(screen_pos2.xy),
		};

		float2 depthDelta = depth - depthCompare;

		occlusion += depthDelta.x > bias && depthDelta.x < SSS_THICKNESS; // in Shadow
		occlusion += depthDelta.y > bias && depthDelta.y < SSS_THICKNESS; // in Shadow
		pos = pos2;
	}
	occlusion = saturate(occlusion);

    // save result of SSS in red channel, and fade contrribution with distance
	color.r = lerp(1.0f - occlusion, 1.0, invlerps(200, SSS_MAXDEPTH, pos.z));
    return color;
}

float4 Shadow(VSOUT IN) : COLOR0
{
	float2 uv = IN.UVCoord;
	// clip((uv < 0.5) - 1); // compute in half res
	// uv *= 2;

	// returns a shadow value from darkness setting value (full shadow) to 1 (full light)
	float4 Shadow = tex2D(TESR_PointShadowBuffer, IN.UVCoord);
	float depth = readDepth(uv);
	float3 camera_vector = toWorld(uv) * depth;
	float4 world_pos = float4(TESR_CameraPosition.xyz + camera_vector, 1.0f);
	float4 pos = mul(world_pos, TESR_WorldViewProjectionTransform);

	// float sunShadows = lerp(1, GetLightAmount(pos, depth), TESR_ShadowFade.y); // disable shadow maps if ShadowFade.y == 0
	float sunShadows = lerp(1, GetLightAmount(pos, depth), TESR_ShadowFade.y); // disable shadow maps if ShadowFade.y == 0
	Shadow.r = min(Shadow.r, sunShadows); // get the darkest between Screenspace & Sun shadows

	Shadow.r = lerp(Shadow.r, 1.0f, TESR_ShadowFade.x); // apply darkness fading when sun is low or moon is not full

	// Shadow.r = Shadow.r * luma(TESR_SunColor) + luma(TESR_SunAmbient); // scale shadows strength to ambient before adding attenuation
	Shadow.r = lerp(0, lerp(1, luma(TESR_SunAmbient), TESR_ShadowFade.z), Shadow.r); // scale shadows strength to ambient before adding attenuation
	Shadow.r += Shadow.g; // Apply poing light attenuation
	Shadow = saturate(Shadow); // No point for the shadow buffer to be beyond the 0-1 range

	return Shadow.rrrr;
}


// perform depth aware 12 taps blur along the direction of the offsetmask
float4 NormalBlurRChannel(VSOUT IN, uniform float2 OffsetMask, uniform float blurRadius,uniform float depthDrop,uniform float endFade) : COLOR0
{
	float WeightSum = 0.114725602f;
	float4 color1 = tex2D(TESR_PointShadowBuffer, IN.UVCoord);
	float3 normal = GetNormal(IN.UVCoord);

	float blurPower = OffsetMask * (blurRadius / pow(tex2D(TESR_DepthBuffer, IN.UVCoord).r, 2));
	float depth1 = readDepth(IN.UVCoord);
	clip(endFade - depth1);

	// coeff for blurring to increase blur depthDrop on surfaces facing away from the camera
	float normalCoeff = (0.5 + 2 * compress(dot(normal, float3(0, 0, 1))));
	color1.rg = color1.rg * WeightSum;
    for (int i = 0; i < cKernelSize; i++)
    {
		float2 uvOff = BlurOffsets[i] * blurPower;
		float2 color2 = tex2D(TESR_PointShadowBuffer, IN.UVCoord + uvOff).rg;
		float depth2 = readDepth(IN.UVCoord + uvOff);
		//float3 normal2 = GetNormal(IN.UVCoord + uvOff);

		float diff = abs(depth1 - depth2);

		int useForBlur = (diff <= depthDrop * normalCoeff);
		color1.rg += BlurWeights[i] * color2.rg * useForBlur;
		WeightSum += BlurWeights[i] * useForBlur;
    }
	color1.rg /= WeightSum;
    return color1;
}


technique {

	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScreenSpaceShadow();
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Shadow();
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
	 	PixelShader = compile ps_3_0 DepthBlur(TESR_PointShadowBuffer, OffsetMaskH, TESR_ShadowScreenSpaceData.y, 5, TESR_ShadowRadius.w);
	}

	pass {
		VertexShader = compile vs_3_0 FrameVS();
	 	PixelShader = compile ps_3_0 DepthBlur(TESR_PointShadowBuffer, OffsetMaskV, TESR_ShadowScreenSpaceData.y, 5, TESR_ShadowRadius.w);
	}
}
