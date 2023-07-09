static const float BIAS = 0.005f;
static const float MIN_VARIANCE = 0.000005;
static const float BLEED_CORRECTION = 0.6;


float GetPointLightAmountValue(samplerCUBE ShadowCubeMapBuffer, float3 LightDir, float Distance) {
	if (TESR_ShadowFade.z == 0) return 1;

	float lightDepth = texCUBE(ShadowCubeMapBuffer, LightDir).r;
	float Shadow = lightDepth + BIAS > Distance;

	// ignore shadow sample if lightDepth is not within range of cube map or if pointLights are disabled (ShadowFade.z set to 0)
	return lerp(1, Shadow, lightDepth > 0.0f && lightDepth < 1.0f);
}


float GetPointLightAtten(float3 LightDir, float Distance, float4 normal) {

	// radius based attenuation based on https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
	float s = Distance * Distance; 
	float atten = saturate((1 - s) / (1 + s));

	LightDir = normalize(LightDir); // normalize
	float diffuse = dot(LightDir, normal.xyz);

	return saturate(diffuse * atten);
}


// returns a point light contribution using a shadow map
float GetPointLightAmount(samplerCUBE ShadowCubeMapBuffer, float4 WorldPos, float4 LightPos, float4 normal) {
	if (!LightPos.w) return 0; // w is light radius.

	float3 LightDir = LightPos.xyz - WorldPos.xyz;
	float3 LightUV = LightDir * float3(-1, -1, 1);

	float Distance = length(LightDir) / LightPos.w; // normalize distance over light range

	float LightAmount = GetPointLightAmountValue(ShadowCubeMapBuffer, LightUV, Distance) * GetPointLightAtten(LightDir, Distance, normal);
	return saturate(LightAmount);
}


// get the normalized distance between the light and a point as a ratio of the light radius
float4 GetPointLightDistance(float4 WorldPos, float4 LightPos){
	float3 LightDir = LightPos.xyz - WorldPos.xyz;
	float Distance = length(LightDir) / LightPos.w; // normalize distance over light range
	return float4(LightDir, Distance);
}


// returns a point light contribution with no shadow map sampling
float GetPointLightContribution(float4 worldPos, float4 LightPos, float4 normal){
	float4 light = GetPointLightDistance(worldPos, LightPos);
	return GetPointLightAtten(light.xyz, light.w, normal);
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
