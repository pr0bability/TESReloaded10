float4 TESR_DepthConstants;
float4 TESR_CameraData;

float4x4 TESR_InvProjectionTransform;

sampler2D TESR_DepthBufferWorld : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = NONE; };
sampler2D TESR_DepthBufferViewModel : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = NONE; };

static const float viewModelNearZ = TESR_DepthConstants.x;
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

// Convert depth to view space Z.
float ToVS(float depth, float nearZ, float farZ) {
    return nearZ * farZ / (nearZ + depth * (farZ - nearZ));
}

// Convert view space Z to depth.
float ToPS(float viewZ, float nearZ, float farZ){
    return nearZ * (farZ - viewZ) / (viewZ * (farZ - nearZ));
}


float4 CombineDepth(VSOUT IN) : COLOR0 {	
	float worldDepth = tex2D(TESR_DepthBufferWorld, IN.UVCoord).x;
	float viewModelDepth = tex2D(TESR_DepthBufferViewModel, IN.UVCoord).x;
	
    float worldViewZ = ToVS(worldDepth, nearZ, farZ);
    float viewModelViewZ = ToVS(viewModelDepth, viewModelNearZ, farZ);
	
    float combinedViewZ = worldViewZ;
    if (viewModelDepth > 0.0)
        combinedViewZ = viewModelViewZ;

    return float4(combinedViewZ / farZ, ToPS(combinedViewZ, nearZ, farZ), 1.0, 1.0);
}
 
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 CombineDepth();
	}
}
