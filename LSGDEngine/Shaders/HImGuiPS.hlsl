struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR0;
    float2 UV : TEXCOORD0;
};

SamplerState Sampler0 : register(s0);
Texture2D Texture0 : register(t0);

float4 main(PS_INPUT Input) : SV_Target
{
    float4 OutCol = Input.Col * Texture0.Sample(Sampler0, Input.UV);
    return OutCol;
}
