// AvgLuma for Oblivion Reloaded

sampler2D TESR_SourceBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

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

float4 AvgLuma(VSOUT IN) : COLOR0
{	// samples 100 different locations around the screen to calculate an average
	float4 color = float4(0, 0, 0, 0);
	for (float i=0.05; i<1; i+=0.1){
		for (float j=0.05; j<1; j+=0.1){
			color += tex2D(TESR_SourceBuffer, float2(i, j));
		}
	}
	return color/100;
}
 
technique
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 AvgLuma();
	}
}
