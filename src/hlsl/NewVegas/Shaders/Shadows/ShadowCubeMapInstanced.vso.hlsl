row_major float4x4 TESR_ShadowViewProjTransform : register(c4);
float4 TESR_ShadowCubeMapLightPosition : register(c63);

struct VS_INPUT {
    // Per geometry.
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
    
    // Per instance.
    float4 model_matrix_0 : TEXCOORD4;
    float4 model_matrix_1 : TEXCOORD5;
    float4 model_matrix_2 : TEXCOORD6;
    float4 model_matrix_3 : TEXCOORD7;
};

struct VS_OUTPUT {
    float4 position : POSITION;
    float4 texcoord_0 : TEXCOORD0;
	float4 texcoord_1 : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT IN) {
    VS_OUTPUT OUT;

    float4x4 modelMatrix = {
        IN.model_matrix_0, IN.model_matrix_1,
        IN.model_matrix_2, IN.model_matrix_3
    };
    
    float4 posWS = mul(IN.position, modelMatrix);
	
    OUT.position = mul(posWS, TESR_ShadowViewProjTransform);
    OUT.texcoord_0.xyz = TESR_ShadowCubeMapLightPosition.xyz - posWS.xyz;
    OUT.texcoord_0.w = 0.0f;
    OUT.texcoord_1 = IN.texcoord_0;
    
    return OUT;
	
};