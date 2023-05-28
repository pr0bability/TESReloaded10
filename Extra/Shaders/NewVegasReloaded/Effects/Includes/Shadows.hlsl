static const float BIAS = 0.005f;
static const float MIN_VARIANCE = 0.000005;
static const float BLEED_CORRECTION = 0.4;


float GetPointLightAmountValue(samplerCUBE ShadowCubeMapBuffer, float3 LightDir, float Distance) {
	float lightDepth = texCUBE(ShadowCubeMapBuffer, LightDir).r;

	float Shadow = lightDepth + BIAS > Distance;

	// ignore shadow sample if lightDepth is not within range of cube map or if pointLights are disabled (ShadowFade.z set to 0)
	return lerp(1, Shadow, lightDepth > 0.0f && lightDepth < 1.0f && (TESR_ShadowFade.z != 0));
}


float GetPointLightAtten(float4 WorldPos, float4 LightPos, float4 normal) {
	float3 LightDir = LightPos.xyz - WorldPos.xyz;
	float Distance = length(LightDir);

	// radius based attenuation based on https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
	Distance = Distance / LightPos.w;
	float s = Distance * Distance; 
	float atten = saturate((1 - s) / (1 + s));

	LightDir = normalize(LightDir); // normalize
	float diffuse = dot(LightDir, normal.xyz);

	return saturate(diffuse * atten);
}


float GetPointLightAmount(samplerCUBE ShadowCubeMapBuffer, float4 WorldPos, float4 LightPos, float4 normal) {
	if (!LightPos.w) return 0;

	float3 LightDir = LightPos.xyz - WorldPos.xyz;
	float3 LightUV = LightDir * float3(-1, -1, 1);

	float Distance = length(LightDir);
	Distance = Distance / LightPos.w;

	float LightAmount = GetPointLightAmountValue(ShadowCubeMapBuffer, LightUV, Distance) * GetPointLightAtten(WorldPos, LightPos, normal);
	return saturate(LightAmount);
}

float ChebyshevUpperBound(float2 moments, float distance)
{
	// get traditional shadow value
	float p = (moments.x > distance); //0: in shadow, 1: in light

	// Compute variance.    
	float Variance = moments.y - moments.x * moments.x;
	Variance = max(Variance, MIN_VARIANCE);

	// Compute the Chebyshev upper bound.
	float d = distance - moments.x;
	float p_max = invlerps(BLEED_CORRECTION, 1.0, Variance / (Variance + d*d));
	return max(p, p_max);
}

// Exponential Soft Shadow Maps
float GetLightAmountValueESSM(float depth, float depthCompare){
	return exp(-80 * depthCompare) * depth;
}

// Exponential Shadow Maps
float GetLightAmountValueESM(float depth, float depthCompare){
	return exp(-500 * (depthCompare - depth));
}

float GetLightAmountValueVSM(float2 moments, float depthCompare)
{
	//returns wether the coordinates are in shadow (0), light (1) or penumbra.
	return ChebyshevUpperBound(moments, depthCompare);
}
