static const float BIAS = 0.018;
static const float MIN_VARIANCE = 0.000005;
static const float BLEED_CORRECTION = 0.6;
{
	float2(0.6191136, 0.2113712),
	float2(0.6427037, -0.1334814),
	float2(0.4774624, 0.4735623),
	float2(0.9512414, -0.0845135),
	float2(0.3028566, -0.2758633),
	float2(0.9008041, 0.3133074),
	float2(0.174379, 0.1873058),
	float2(0.6970178, 0.7017708),
	float2(0.2076945, 0.6009082),
	float2(0.5810612, -0.4956908),
	float2(0.2425405, 0.89517),
	float2(0.02547226, -0.4618422),
	float2(-0.2735305, -0.2142529),
	float2(0.271616, -0.7971022),
	float2(0.01671731, -0.07621266),
	float2(-0.2458005, 0.3294578),
	float2(-0.3065021, -0.5580449),
	float2(-0.441579, 0.04194611),
	float2(-0.3955874, 0.6485842),
	float2(-0.7150231, 0.2960931),
	float2(-0.03953541, 0.7715158),
	float2(0.8766894, -0.3817451),
	float2(-0.7316095, -0.1075699),
	float2(-0.5734336, -0.4176584),
	float2(-0.3110258, 0.94859),
	float2(-0.7118847, 0.65051),
	float2(-0.09932458, -0.7863928),
	float2(-0.391506, -0.8877813),
	float2(-0.6333145, -0.7103693),
	float2(-0.8684595, -0.4634778),
	float2(-0.9837686, 0.07260807),
	float2(0.5894084, -0.8077373)
};


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
float GetLightAmountValuePoisson(sampler2D shadowBuffer, float4 coord)
{
    float p = 0;
	[unroll(32)]
    for (int i = 0; i < 32; i++)
    {

        float4 tempshadowBufferValue = tex2D(shadowBuffer, (coord.xy + poissonDisk[i]/700));
        if (tempshadowBufferValue.x > coord.z -0.0002)
        {
            p += 0.03125;
        }
    }
    return p;
}
