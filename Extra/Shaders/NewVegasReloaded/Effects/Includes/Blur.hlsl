#define cKernelSize 12

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


// downsample/upsample a part of the screen given by the scaleFactor
float4 Scale(VSOUT IN, uniform float scaleFactor) : COLOR0
{
	float2 uv = IN.UVCoord / scaleFactor;// scale the uv by wanted scale
	clip((uv < 1) - 1); // discard uvs outside of [0-1]range

	return tex2D(TESR_RenderedBuffer, uv);	
}

// simple local average without weights. Use scaleFactor to only blur a portion of the screen starting from the top left corner
float4 BoxBlur (VSOUT IN, uniform float2 offsetMask, uniform float scaleFactor) :COLOR0
{
	clip((IN.UVCoord <= scaleFactor) - 1);

	float2 maxuv = scaleFactor - 1.5 * TESR_ReciprocalResolution.xy;
	float4 color = tex2D(TESR_RenderedBuffer, IN.UVCoord);
	color += tex2D(TESR_RenderedBuffer, min(IN.UVCoord + offsetMask * 1 * TESR_ReciprocalResolution.xy, maxuv));
	color += tex2D(TESR_RenderedBuffer, min(IN.UVCoord + offsetMask * -1 * TESR_ReciprocalResolution.xy, maxuv));
	color += tex2D(TESR_RenderedBuffer, min(IN.UVCoord + offsetMask * -2 * TESR_ReciprocalResolution.xy, maxuv));
	color += tex2D(TESR_RenderedBuffer, min(IN.UVCoord + offsetMask * 2 * TESR_ReciprocalResolution.xy, maxuv));

	// return color;
	return color /= 5;
}

float4 Blur(VSOUT IN, uniform float2 OffsetMask, uniform float blurRadius, uniform float scale) : COLOR0
{
	// blur using a gaussian blur
	float WeightSum = 0.114725602f;
	float2 uv = IN.UVCoord;
	float4 color = tex2D(TESR_RenderedBuffer, uv) * WeightSum;
	clip ((uv <= scale) - 1);

    for (int i = 0; i < cKernelSize; i++)
    {
		float2 uvOff = (BlurOffsets[i] * OffsetMask) * blurRadius;
		float isValid = ((uv.x + uvOff.x) < scale) && ((uv.y + uvOff.y) < scale); //discard samples outside the screen area
		color += BlurWeights[i] * tex2D(TESR_RenderedBuffer, uv + uvOff) * isValid;
		WeightSum += BlurWeights[i] * isValid;
    }
	color /= WeightSum;
    return color;
}


// perform depth aware 12 taps blur along the direction of the offsetmask
float4 BlurRChannel(VSOUT IN, uniform float2 OffsetMask, uniform float blurRadius,uniform float depthDrop,uniform float endFade) : COLOR0
{
	float WeightSum = 0.114725602f;
	float4 color1 = tex2D(TESR_RenderedBuffer, IN.UVCoord) * WeightSum;

	float depth1 = readDepth(IN.UVCoord);
	clip(endFade - depth1);

	[unroll]
    for (int i = 0; i < cKernelSize; i++)
    {
		float2 uv = IN.UVCoord + (BlurOffsets[i] * OffsetMask) * blurRadius;
		float4 color2 = tex2D(TESR_RenderedBuffer,  uv);
		float depth2 = readDepth(uv);
		float diff = abs(float(depth1 - depth2));

		int useForBlur = (diff <= depthDrop);
		color1 += BlurWeights[i] * color2 * useForBlur;
		WeightSum += BlurWeights[i] * useForBlur;
    }
	color1 /= WeightSum;
    return color1;
}