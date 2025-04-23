// Bloom.
// https://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare/
// https://learnopengl.com/Guest-Articles/2022/Phys.-Based-Bloom
// https://www.froyok.fr/blog/2021-12-ue4-custom-bloom/
//
// Designed to work without thresholding, for HDR rendering.
// Should minimize potential bloom issues of filtering artifacts and fireflies.

float4 TESR_BloomResolution;
float4 TESR_BloomData; // .x filterRadius x axis, .y filterRadius y axis, .z blendingCoefficient, .w inverse of number of passes for upscale

sampler2D TESR_BloomBuffer : register(s0) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer2 : register(s1) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer4 : register(s2) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer8 : register(s3) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer16 : register(s4) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer32 : register(s5) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer64 : register(s6) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_BloomBuffer128 : register(s7) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };
sampler2D TESR_RenderedBuffer : register(s8) = sampler_state { ADDRESSU = CLAMP; ADDRESSV = CLAMP; MAGFILTER = LINEAR; MINFILTER = LINEAR; MIPFILTER = LINEAR; };

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

#include "Includes/Sampling.hlsl"

// Downsample with the 13 tap box.
float4 Downsample(VSOUT IN, uniform sampler2D buffer) : COLOR0 {
    float2 uv = IN.UVCoord;
    
    float2 texelSize = { TESR_BloomResolution.z, TESR_BloomResolution.w };

    float4 downsample = DownsampleBox13(buffer, uv, texelSize);
    
    return downsample;
}

// Intermediary upsamples.
float4 Upsample(VSOUT IN, uniform sampler2D buffer, uniform sampler2D addBuffer) : COLOR0 {
    float2 uv = IN.UVCoord;
    
    const float2 filterRadius = { TESR_BloomData.x, TESR_BloomData.y };
    
    float4 upsample = UpsampleTent9(buffer, uv, filterRadius);
    float4 nextMip = tex2D(addBuffer, uv);
    
    [branch] if (TESR_BloomData.z > 0.0) {
        return float4(lerp(nextMip.rgb, upsample.rgb, TESR_BloomData.z), 1);
    } else {
        return float4(upsample.rgb + nextMip.rgb, 1);
    }
}

// Last upsample that normalizes the result if needed.
float4 UpsampleLast(VSOUT IN, uniform sampler2D buffer, uniform sampler2D addBuffer) : COLOR0 {
    float2 uv = IN.UVCoord;
    
    const float2 filterRadius = { TESR_BloomData.x, TESR_BloomData.y };
    
    float4 upsample = UpsampleTent9(buffer, uv, filterRadius);
    float4 nextMip = tex2D(addBuffer, uv);
    
    [branch] if (TESR_BloomData.z > 0.0) {
        return float4(lerp(nextMip.rgb, upsample.rgb, TESR_BloomData.z), 1);
    } else {
        return float4((upsample.rgb + nextMip.rgb) * TESR_BloomData.w, 1);
    }
}

technique // 0
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Downsample(TESR_RenderedBuffer); // output to BloomBuffer
    }	
}

technique // 1
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Downsample(TESR_BloomBuffer); // output to BloomBuffer2
    }	
}

technique // 2
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Downsample(TESR_BloomBuffer2); //output to BloomBuffer4
    }	
}

technique // 3
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Downsample(TESR_BloomBuffer4); // output to BloomBuffer8
    }	
}

technique // 4
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Downsample(TESR_BloomBuffer8); // output to BloomBuffer16
    }	
}

technique // 5
{
    pass {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Downsample(TESR_BloomBuffer16); // output to BloomBuffer32
    }
}

technique // 6
{
    pass {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Downsample(TESR_BloomBuffer32); // output to BloomBuffer64
    }
}

technique // 7
{
    pass {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Downsample(TESR_BloomBuffer64); // output to BloomBuffer128
    }
}

technique // 8
{
    pass {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Upsample(TESR_BloomBuffer128, TESR_BloomBuffer64); // output to BloomBuffer64
    }
}

technique // 9
{
    pass {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Upsample(TESR_BloomBuffer64, TESR_BloomBuffer32); // output to BloomBuffer32
    }
}

technique // 10
{
    pass {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Upsample(TESR_BloomBuffer32, TESR_BloomBuffer16); // output to BloomBuffer16
    }
}

technique // 11
{
    pass {
        VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Upsample(TESR_BloomBuffer16, TESR_BloomBuffer8); // output to BloomBuffer8
    }
}

technique // 12
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Upsample(TESR_BloomBuffer8, TESR_BloomBuffer4); // output to BloomBuffer4
    }	
}

technique // 13
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
        PixelShader = compile ps_3_0 Upsample(TESR_BloomBuffer4, TESR_BloomBuffer2); // output to BloomBuffer2
    }	
}

technique // 14
{
	pass
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 UpsampleLast(TESR_BloomBuffer2, TESR_BloomBuffer); // output to BloomBuffer
	}	
}
