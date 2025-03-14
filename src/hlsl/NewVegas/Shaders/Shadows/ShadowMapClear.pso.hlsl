float4 TESR_ClearColor : register(c0);

float4 main() : COLOR0 {
    return TESR_ClearColor;
}
