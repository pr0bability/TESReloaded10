float4 TESR_DepthConstants;
float4 TESR_CameraData;

float4x4 TESR_InvProjectionTransform;

sampler2D TESR_DepthBufferWorld : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBufferViewModel : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

static const float invertedDepth = TESR_DepthConstants.z;
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
	
    float combinedDepth = min(worldDepth, viewModelDepth);
	if (invertedDepth){
        combinedDepth = max(worldDepth, viewModelDepth);
    }

    float x = IN.UVCoord.x * 2 - 1;
    float y = (1 - IN.UVCoord.y) * 2 - 1;
    float4 clipSpace = float4(x, y, combinedDepth, 1.0f);

    float4 viewSpace = mul(clipSpace, TESR_InvProjectionTransform);
	
    viewSpace /= viewSpace.w;

	return float4(viewSpace.z / farZ, combinedDepth, 1.0, 1.0); // scale values back to 0 - 1 to avoid overflow
}
 
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 CombineDepth();
	}
}
