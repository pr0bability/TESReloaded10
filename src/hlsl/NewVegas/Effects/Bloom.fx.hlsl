// Bloom shader

float4 TESR_ReciprocalResolution;
float4 TESR_BloomResolution;
float4 TESR_SunColor;
float4 TESR_SunAmount;
float4 TESR_LensData; // x: lens strength, y: luma threshold
float4 TESR_DebugVar; // used for the luma threshold used for bloom
float4 TESR_BloomData; // used for the luma threshold used for bloom

sampler2D TESR_BloomBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer2 : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer4 : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer8 : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer16 : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer32 : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_RenderedBuffer : register(s6) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_DepthBuffer : register(s7) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

static const float scale = 0.5;

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

#include "Includes/Helpers.hlsl"
#include "Includes/Blur.hlsl"


float4 sampleBox(float2 uv, sampler2D buffer, float offset) {
	float4 color = tex2D(buffer, uv + float2(-1, -1) * offset * TESR_BloomResolution.zw);
	color += tex2D(buffer, uv + float2(-1, 1) * offset * TESR_BloomResolution.zw);
	color += tex2D(buffer, uv + float2(1, -1) * offset * TESR_BloomResolution.zw);
	color += tex2D(buffer, uv + float2(1, 1) * offset * TESR_BloomResolution.zw);
	return float4(color.rgb * 0.25, 1);
}

// downsample/upsample a part of the screen given by the scaleFactor
float4 ScaleDown(VSOUT IN, uniform sampler2D buffer) : COLOR0
{
	float2 uv = IN.UVCoord - float2(0.0, 0.0) * TESR_BloomResolution.zw;
	// float2 uv = IN.UVCoord;
	return sampleBox(uv, buffer, 0.5);
}

// downsample/upsample a part of the screen given by the scaleFactor
float4 ScaleUp(VSOUT IN, uniform sampler2D buffer, uniform sampler2D addBuffer) : COLOR0
{
	float2 uv = IN.UVCoord + float2(1.0, 1.0)* 0.8 * TESR_BloomResolution.zw;
	// float2 uv = IN.UVCoord;
	float4 color = sampleBox(uv, buffer, 1);

	float4 addColor = tex2D(addBuffer, uv);
	return float4(color.rgb + addColor.rgb, 1);
}

float4 Transfert(VSOUT IN, uniform sampler2D buffer) : COLOR0
{
	return tex2D(buffer, IN.UVCoord);
}

float4 Bloom(VSOUT IN ):COLOR0{

	// quick average lum with 4 samples at corner pixels
	float4 color = linearize(sampleBox(IN.UVCoord, TESR_RenderedBuffer, 0.5));

	float threshold = TESR_BloomData.x;
	float brightness = max(0.000001, luma(color));
	float bloomScale = TESR_BloomData.y; 

	float bloom = bloomScale * sqr(max(0.0, brightness - threshold)) / brightness;

	return float4(saturate(bloom) * color.rgb, 1);
}

technique // 0
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 Bloom();  // output to BloomBuffer
	}	
}

technique // 1
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleDown(TESR_BloomBuffer);  // output to BloomBuffer2
	}	
}

technique // 2
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleDown(TESR_BloomBuffer2);  //output to BloomBuffer4
	}	
}

technique // 3
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleDown(TESR_BloomBuffer4); // output to BloomBuffer8
	}	
}

technique // 4
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleDown(TESR_BloomBuffer8); // output to BloomBuffer16
	}	
}

technique // 5
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleDown(TESR_BloomBuffer16); // output to BloomBuffer32
	}	
}

technique // 6
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer32, TESR_BloomBuffer16); // output to BloomBuffer16
	}	
}

technique // 7
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer16, TESR_BloomBuffer8); // output to BloomBuffer8
	}	
}

technique // 8
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer8, TESR_BloomBuffer4); // output to BloomBuffer4
	}	
}

technique // 9
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer4, TESR_BloomBuffer2); // output to BloomBuffer2
	}	
}

technique // 10
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer2, TESR_BloomBuffer); // output to BloomBuffer
	}	
}