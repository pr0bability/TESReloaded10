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
sampler2D TESR_BloomBuffer64 : register(s6) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_RenderedBuffer : register(s7) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

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

// static const float3 offsets[9] = {
// 	float3(0, 0, 4), 
// 	float3(1, 0, 2), 
// 	float3(-1, 0, 2), 
// 	float3(0, 1, 2), 
// 	float3(0, -1, 2), 
// 	float3(-1, -1, 1), 
// 	float3(1, -1, 1), 
// 	float3(-1, 1, 1), 
// 	float3(1, 1, 1)
// };

static const float3 offsets[5] = {
	float3(0, 0, 1), 
	float3(-1, -1, 1), 
	float3(1, -1, 1), 
	float3(-1, 1, 1), 
	float3(1, 1, 1)
};


float4 sampleBox(float2 uv, sampler2D buffer, float offset){
    float4 color = float4(0, 0, 0, 0);
    float total = 0;

    for (int i = 0; i < 5; i ++){
        float2 coords = uv + TESR_BloomResolution.zw * offsets[i].xy * offset;
        float2 safety = TESR_ReciprocalResolution.xy * float2(0.5, 0.5);
        float isValid = (coords.x > safety.x && coords.x < 1 - safety.x && coords.y > safety.y && coords.y < 1 - safety.y) * offsets[i].z;
		color += tex2D(buffer, coords) * isValid;
        total += isValid;
    }
    color /= total;

    return color;
}


// downsample/upsample a part of the screen given by the scaleFactor
float4 ScaleDown(VSOUT IN, uniform sampler2D buffer) : COLOR0
{
	float2 uv = IN.UVCoord;
	return sampleBox(uv, buffer, 0.5);
}

// downsample/upsample a part of the screen given by the scaleFactor
float4 ScaleUp(VSOUT IN, uniform sampler2D buffer, uniform sampler2D addBuffer) : COLOR0
{
	float2 uv = IN.UVCoord;
	float4 color = sampleBox(uv, buffer, 0.5);

	float4 addColor = tex2D(addBuffer, IN.UVCoord);
	return float4(color.rgb + addColor.rgb, 1);
}


float4 Bloom(VSOUT IN ):COLOR0
{
	// quick average lum with 4 samples at corner pixels
	float4 color = linearize(sampleBox(IN.UVCoord, TESR_RenderedBuffer, 1.0f));

	float threshold = TESR_BloomData.x;
	if (threshold == 0) {
		return color; // cancel out tresholding if treshold is set to 0
	}

	float brightness = luma(color);
	float bloomScale = TESR_BloomData.y; 

	float bloom = bloomScale * (brightness > 0.0 ? (sqr(max(0.0, brightness - threshold)) / brightness) : 0.0);

	return float4(bloom * color.rgb, 1);
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
		PixelShader = compile ps_3_0 ScaleDown(TESR_BloomBuffer32); // output to BloomBuffer64
	}	
}

technique // 7
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer64, TESR_BloomBuffer32); // output to BloomBuffer32
	}	
}


technique // 8
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer32, TESR_BloomBuffer16); // output to BloomBuffer16
	}	
}

technique // 9
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer16, TESR_BloomBuffer8); // output to BloomBuffer8
	}	
}

technique // 10
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer8, TESR_BloomBuffer4); // output to BloomBuffer4
	}	
}

technique // 11
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer4, TESR_BloomBuffer2); // output to BloomBuffer2
	}	
}

technique // 12
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 ScaleUp(TESR_BloomBuffer2, TESR_BloomBuffer); // output to BloomBuffer
	}	
}