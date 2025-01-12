
float4 TESR_ShadowData : register(c0);
float4 TESR_ShadowExtraData : register(c1);

sampler2D DiffuseMap : register(s0) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = POINT; };
sampler2D LeafDiffuseMap : register(s1) = sampler_state { ADDRESSU = WRAP; ADDRESSV = WRAP; MAGFILTER = POINT; MINFILTER = POINT; MIPFILTER = POINT; };

struct VS_OUTPUT {
    float4 texcoord_0 : TEXCOORD0;
	float4 texcoord_1 : TEXCOORD1;
};

struct PS_OUTPUT {
    float4 color_0 : COLOR0;
};

PS_OUTPUT main(VS_OUTPUT IN) {
    PS_OUTPUT OUT;

	if (TESR_ShadowData.y == 1.0f) { // Leaves (Speedtrees) or alpha is required
		float4 diffuse = (TESR_ShadowData.x == 2.0f) ? tex2D(LeafDiffuseMap, IN.texcoord_1.xy) : tex2D(DiffuseMap, IN.texcoord_1.xy);
        if (diffuse.a < 0.5f)
            discard;
    }
	
	float depth = IN.texcoord_0.z / IN.texcoord_0.w;

	// TESR_ShadowData.w : shadow technique
	// 0: disabled
	// 1: VSM
	// 2: simple ESM
	// 3: filtered ESM
	// 4: PCF or Orthomap
	if (!TESR_ShadowData.z){
		// VSM
		// Cheat to reduce shadow acne in variance maps.
		float dx = ddx(depth);
		float dy = ddy(depth);
		float moment2 = depth * depth + 0.25 * (dx * dx + dy * dy);
        OUT.color_0 = float4(depth, moment2, 0.0f, 1.0f);
    } else {
		// Only depth.
		OUT.color_0 = float4(depth, 0.0f, 0.0f, 1.0f);
	}

	return OUT;	
};