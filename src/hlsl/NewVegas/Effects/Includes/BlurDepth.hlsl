#include "Includes/Blur.hlsl"

/*
* A depth aware blur that requires the Depthbuffer and related functions from Depth.hlsl
*/

// perform depth aware 12 taps blur along the direction of the offsetmask
float4 DepthBlur(VSOUT IN, uniform sampler2D buffer, uniform float2 OffsetMask, uniform float blurRadius,uniform float depthDrop,uniform float endFade) : COLOR0
{
	float WeightSum = 0.114725602f;
	float4 color1 = tex2D(buffer, IN.UVCoord) * WeightSum;

	float depth1 = readDepth(IN.UVCoord);
	clip(endFade - depth1);

	depthDrop *= (depth1 / farZ);

	[unroll]
    for (int i = 0; i < cKernelSize; i++)
    {
		float2 uv = IN.UVCoord + (BlurOffsets[i] * OffsetMask) * blurRadius;
		float4 color2 = tex2D(buffer,  uv);
		float depth2 = readDepth(uv);
		float diff = abs(float(depth1 - depth2));

		int useForBlur = (diff <= depthDrop);
		color1 += BlurWeights[i] * color2 * useForBlur;
		WeightSum += BlurWeights[i] * useForBlur;
    }
	color1 /= WeightSum;
    return float4(color1.rgb, 1);
}