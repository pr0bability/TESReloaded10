float4 TESR_DepthConstants;
float4 TESR_CameraData;

sampler2D TESR_DepthBufferWorld : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBufferViewModel : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

static const float nearZ = TESR_CameraData.x;
static const float farZ = TESR_CameraData.y;

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

// linearize depth
float readDepth(float depth, float nearZ, float farZ)
{
	float Q = farZ/(farZ - nearZ);
    float ViewZ = (-nearZ *Q) / (depth - Q);
	return ViewZ;
}

// convert back to usual depth buffer format for easier reconstruction
float packDepth(float viewZ, float nearZ, float farZ){
	float Q = farZ/(farZ - nearZ);
	return (Q* (viewZ - nearZ ))/ viewZ;
}


float4 CombineDepth(VSOUT IN) : COLOR0
{	
	float worldDepth = tex2D(TESR_DepthBufferWorld, IN.UVCoord).x;
	float viewModelDepth = tex2D(TESR_DepthBufferViewModel, IN.UVCoord).x;
	
	// convert depths to linear in order to combine them
	float linearWorldDepth = readDepth(worldDepth, nearZ, farZ);
	float linearViewModelDepth = readDepth(viewModelDepth, TESR_DepthConstants.x, farZ);

	float depth = min(linearWorldDepth, linearViewModelDepth); // Get closest value from both depths
	float packedDepth = packDepth(depth, nearZ, farZ); // encode back to non converted depth format for easier reconstruction using matrices
	
	return float4(depth / farZ, packedDepth, 1.0, 1.0) ;// scale values back to 0 - 1 to avoid overflow
}
 
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 CombineDepth();
	}
}
