// AvgLuma for Oblivion Reloaded

float4 TESR_MotionBlurData;
float4 TESR_DepthOfFieldData;
float4 TESR_GameTime;
float4 TESR_ExposureData; // x:min brightness, y;max brightness, z:dark adapt speed, w: light adapt speed

sampler2D TESR_RenderedBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_AvgLumaBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

static const float decreaseRate = -TESR_ExposureData.z; // max value for adaptation speed towards darker screens
// static const float decreaseRate = -TESR_ExposureData.z * 0.001; // max value for adaptation speed towards darker screens
static const float increaseRate = TESR_ExposureData.w; // max value for adaptation speed towards brighter screens
// static const float increaseRate = TESR_ExposureData.w * 0.001; // max value for adaptation speed towards brighter screens
static const float2 center = float2(0.5, 0.5); // this shader is to be applied on a 1x1 texture so we sample at the center

#include "Includes/Helpers.hlsl"
#include "Includes/Depth.hlsl"

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

static float2 taps[12] =
{
    float2(-0.326212, -0.405810),
    float2(-0.840144, -0.073580),
    float2(-0.695914,  0.457137),
    float2(-0.203345,  0.620716),
    float2( 0.962340, -0.194983),
    float2( 0.473434, -0.480026),
    float2( 0.519456,  0.767022),
    float2( 0.185461, -0.893124),
    float2( 0.507431,  0.064425),
    float2( 0.896420,  0.412458),
    float2(-0.321940, -0.932615),
    float2(-0.791559, -0.597710)
};

// returns a value from start to end within a min and max step from start
float stepTo(float startValue, float endValue, float minStep, float maxStep){
	// make sure min is negative and max is positive
	minStep = abs(minStep) * -1;
	maxStep = abs(maxStep);

	float diff = clamp(endValue - startValue, minStep, maxStep);
	return startValue + diff;
}

static const float HyperFocalDistance = max(0.0000001, TESR_DepthOfFieldData.x * 1000); // the distance at which DOF is greatest (infinite towards the distance and closest to the camera)

// calculates the current focal distance for depth of field shader and animates the value.
float getFocalDistance() {
	float oldFocus = tex2D(TESR_AvgLumaBuffer, center).b;

	// gets the autofocus depth based on a local average, scaled with player movement speed (to avoid flicker during fast movement)
	float2 speed = saturate(abs(float2(TESR_MotionBlurData.x, TESR_MotionBlurData.y))) + 0.1;

	float centerDepth = readDepth(center);
	float depth = centerDepth;
	float2 samplingRange = speed;
	for (int i=0; i<12; i++){
		depth += readDepth(center + 0.3 * taps[i] * samplingRange);
	}
	depth /= 12;

	depth = centerDepth > depth ? centerDepth:depth; //if pixel at the center is farther than the average, we use that value (fix for some iron sights)
	depth = saturate(depth/HyperFocalDistance); // remaps to [0-1] to save as color value in the texture

	float step = pow(max(oldFocus, depth), 0.1); // because of the nature of depth, we scale the speed with distance
	return stepTo(oldFocus, depth, 3 * step * TESR_GameTime.w, 1 * step * TESR_GameTime.w);
}

float4 AvgLuma(VSOUT IN) : COLOR0
{	
	float2 oldLuma = tex2D(TESR_AvgLumaBuffer, center).rg;

	// samples 100 different locations around the screen to calculate an average
	float4 color = float4(0, 0, 0, 0);
	float total = 0;
	for (float i = 0.05; i < 1; i+= 0.1){
		for (float j = 0.05; j < 1; j+= 0.1){
			color += linearize(tex2D(TESR_RenderedBuffer, float2(i, j)));
			total++;
		}
	}
	for (i= 0 ; i < 12; i++){
		color += linearize(tex2D(TESR_RenderedBuffer, center + taps[i]));
		total++;
	}
	color /= total;
	float newLuma = (luma(color) + (7 * oldLuma.r)) / 8; // average over 8 frames

	// gradually change average luma
	float animatedLuma = stepTo(oldLuma.g, newLuma, TESR_GameTime.w * decreaseRate, TESR_GameTime.w * increaseRate);

	// texture will store the actual current luma, the animated current luma, and the animated focal distance for DoF.
	return float4(newLuma, animatedLuma, getFocalDistance(), 1);
}
 
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 AvgLuma();
	}
}
