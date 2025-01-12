static const float BIAS = 0.018;


float GetPointLightAmountValue(samplerCUBE ShadowCubeMapBuffer, float3 LightDir, float Distance) {
	if (TESR_ShadowFade.z == 0) return 1;

	float lightDepth = texCUBE(ShadowCubeMapBuffer, LightDir).r;
	float Shadow = lightDepth + BIAS * Distance > Distance; // increase BIAS with distance

	// ignore shadow sample if lightDepth is not within range of cube map or if pointLights are disabled (ShadowFade.z set to 0)
	return lerp(1, Shadow, lightDepth > 0.0f && lightDepth < 1.0f);
}


float GetPointLightAtten(float3 LightDir, float Distance, float4 normal) {

	// radius based attenuation based on https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
	float s = saturate(Distance * Distance); 
	float atten = saturate(((1 - s) * (1 - s)) / (1 + 5.0 * s));

	LightDir = normalize(LightDir); // normalize
	float diffuse = lerp(1, dot(LightDir, normal.xyz), smoothstep(0, 0.2, Distance)); // add some light bleeding at very short distance

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


// Reduces VSM light bleedning
float ReduceLightBleeding(float pMax, float amount) {
    // Remove the [0, amount] tail and linearly rescale (amount, 1].
    return invlerps(amount, 1.0f, pMax);
}


float ChebyshevUpperBound(float2 moments, float mean, float minVariance,
                          float lightBleedingReduction) {
    // Compute variance
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, minVariance);

    // Compute probabilistic upper bound
    float d = mean - moments.x;
    float pMax = variance / (variance + (d * d));

    pMax = ReduceLightBleeding(pMax, lightBleedingReduction);

    // One-tailed Chebyshev
    return (mean <= moments.x ? 1.0f : pMax);
}


float GetLightAmountValueVSM(float2 moments, float depth, float bias, float bleedReduction) {
    return ChebyshevUpperBound(moments, depth, bias, bleedReduction);
}
