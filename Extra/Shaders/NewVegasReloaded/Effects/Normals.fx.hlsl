float4 TESR_ReciprocalResolution;

sampler2D TESR_DepthBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };
sampler2D TESR_NormalsBuffer : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = NONE; MINFILTER = NONE; MIPFILTER = NONE; };

static const float dropTreshold = 0.82;
static const float blurRadius = 0.6;
static const int KernelSize = 24;
static const float2 OffsetMaskH = float2(1.0f, 0.0f);
static const float2 OffsetMaskV = float2(0.0f, 1.0f);

static const float BlurNormalsWeights[KernelSize] = 
{
	0.019956226f,
	0.021463016f,
	0.032969806f,
	0.044476596f,
	0.055983386f,
	0.067490176f,
	0.078996966f,
	0.080503756f,
	0.092010546f,
	0.105024126f,
	0.116530916f,
	0.128037706f,
	0.128037706f,
	0.116530916f,
	0.105024126f,
	0.092010546f,
	0.080503756f,
	0.078996966f,
	0.067490176f,
	0.055983386f,
	0.044476596f,
	0.032969806f,
	0.021463016f,
	0.019956226f
};

static const float2 BlurNormalsOffsets[KernelSize] = 
{
	float2(-12.0f * TESR_ReciprocalResolution.x, -12.0f * TESR_ReciprocalResolution.y),
	float2(-11.0f * TESR_ReciprocalResolution.x, -11.0f * TESR_ReciprocalResolution.y),
	float2(-10.0f * TESR_ReciprocalResolution.x, -10.0f * TESR_ReciprocalResolution.y),
	float2( -9.0f * TESR_ReciprocalResolution.x,  -9.0f * TESR_ReciprocalResolution.y),
	float2( -8.0f * TESR_ReciprocalResolution.x,  -8.0f * TESR_ReciprocalResolution.y),
	float2( -7.0f * TESR_ReciprocalResolution.x,  -7.0f * TESR_ReciprocalResolution.y),
	float2( -6.0f * TESR_ReciprocalResolution.x,  -6.0f * TESR_ReciprocalResolution.y),
	float2( -5.0f * TESR_ReciprocalResolution.x,  -5.0f * TESR_ReciprocalResolution.y),
	float2( -4.0f * TESR_ReciprocalResolution.x,  -4.0f * TESR_ReciprocalResolution.y),
	float2( -3.0f * TESR_ReciprocalResolution.x,  -3.0f * TESR_ReciprocalResolution.y),
	float2( -2.0f * TESR_ReciprocalResolution.x,  -2.0f * TESR_ReciprocalResolution.y),
	float2( -1.0f * TESR_ReciprocalResolution.x,  -1.0f * TESR_ReciprocalResolution.y),
	float2(  1.0f * TESR_ReciprocalResolution.x,   1.0f * TESR_ReciprocalResolution.y),
	float2(  2.0f * TESR_ReciprocalResolution.x,   2.0f * TESR_ReciprocalResolution.y),
	float2(  3.0f * TESR_ReciprocalResolution.x,   3.0f * TESR_ReciprocalResolution.y),
	float2(  4.0f * TESR_ReciprocalResolution.x,   4.0f * TESR_ReciprocalResolution.y),
	float2(  5.0f * TESR_ReciprocalResolution.x,   5.0f * TESR_ReciprocalResolution.y),
	float2(  6.0f * TESR_ReciprocalResolution.x,   6.0f * TESR_ReciprocalResolution.y),
	float2(  7.0f * TESR_ReciprocalResolution.x,   7.0f * TESR_ReciprocalResolution.y),
	float2(  8.0f * TESR_ReciprocalResolution.x,   8.0f * TESR_ReciprocalResolution.y),
	float2(  9.0f * TESR_ReciprocalResolution.x,   9.0f * TESR_ReciprocalResolution.y),
	float2( 10.0f * TESR_ReciprocalResolution.x,  10.0f * TESR_ReciprocalResolution.y),
	float2( 11.0f * TESR_ReciprocalResolution.x,  11.0f * TESR_ReciprocalResolution.y),
	float2( 12.0f * TESR_ReciprocalResolution.x,  12.0f * TESR_ReciprocalResolution.y)
};


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

#include "Includes/Depth.hlsl"
#include "Includes/Helpers.hlsl"


float4 ComputeNormals(VSOUT IN) :COLOR0
{
	float2 uv = IN.UVCoord;

	// improved normal reconstruction algorithm from 
	// https://gist.github.com/bgolus/a07ed65602c009d5e2f753826e8078a0

	// store coordinates at 1 and 2 pixels from center in all directions
	float4 rightUv = uv.xyxy + float4(1.0, 0.0, 2.0, 0.0) * TESR_ReciprocalResolution.xyxy; 
	float4 leftUv = uv.xyxy + float4(-1.0, 0.0, -2.0, 0.0) * TESR_ReciprocalResolution.xyxy; 
	float4 bottomUv = uv.xyxy + float4(0.0, 1.0, 0.0, 2.0) * TESR_ReciprocalResolution.xyxy; 
	float4 topUv =uv.xyxy + float4(0.0, -1.0, 0.0, -2.0) * TESR_ReciprocalResolution.xyxy; 

	float depth = readDepth(uv);

	// get depth values at 1 & 2 pixels offsets from current along the horizontal axis
	float4 H = float4(
		readDepth(rightUv.xy),
		readDepth(leftUv.xy),
		readDepth(rightUv.zw),
		readDepth(leftUv.zw)
	);

	// get depth values at 1 & 2 pixels offsets from current along the vertical axis
	float4 V = float4(
		readDepth(topUv.xy),
		readDepth(bottomUv.xy),
		readDepth(topUv.zw),
		readDepth(bottomUv.zw)
	);

	float2 he = abs((2 * H.xy - H.zw) - depth);
	float2 ve = abs((2 * V.xy - V.zw) - depth);

	// pick horizontal and vertical diff with the smallest depth difference from slopes
	float3 centerPoint = reconstructPosition(uv);
	float3 rightPoint = reconstructPosition(rightUv.xy);
	float3 leftPoint = reconstructPosition(leftUv.xy);
	float3 topPoint = reconstructPosition(topUv.xy);
	float3 bottomPoint = reconstructPosition(bottomUv.xy);
	float3 left = centerPoint - leftPoint;
	float3 right = rightPoint - centerPoint;
	float3 down = centerPoint - bottomPoint;
	float3 up = topPoint - centerPoint;

	float3 hDeriv = he.x > he.y ? left : right;
	float3 vDeriv = ve.x > ve.y ? down : up;

	// get view space normal from the cross product of the best derivatives
	// half3 viewNormal = normalize(cross(hDeriv, vDeriv));
	float3 viewNormal = normalize(cross(vDeriv, hDeriv));

	return float4 (compress(viewNormal), 1.0);
}
 

float4 BlurNormals(VSOUT IN, uniform float2 OffsetMask) : COLOR0
{
	float WeightSum = 0.12f * saturate(1 - dropTreshold);
	float3 normal = expand(tex2D(TESR_NormalsBuffer, IN.UVCoord).rgb);
	float3 finalNormal = normal * WeightSum;
	float depth = readDepth(IN.UVCoord);
	float depthBasedRadius = abs(log(depth/farZ)) * blurRadius;
	float depthDrop = (depth/farZ) * 7000; // difference of depth beyond which the sample will not count towards the blur

	for (int i = 0; i < KernelSize; i++) {
		float2 uvOff = (BlurNormalsOffsets[i] * OffsetMask) * depthBasedRadius;
		float3 newNormal = expand(tex2D(TESR_NormalsBuffer, IN.UVCoord + uvOff).rgb);
		float depth2 = readDepth(IN.UVCoord + uvOff);
		float useForBlur = abs(float(depth - depth2)) <= depthDrop;

		float weight = BlurNormalsWeights[i] * saturate(dot(newNormal, normal) - dropTreshold * 0.75f) * useForBlur;

		finalNormal += weight * newNormal;
		WeightSum += weight;
	}
	
	finalNormal /= WeightSum;
    return float4(compress(finalNormal), 1.0f);
}


technique
{
	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ComputeNormals();
	}
	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 BlurNormals(OffsetMaskH);
	}
	pass
	{ 
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 BlurNormals(OffsetMaskV);
	}
}