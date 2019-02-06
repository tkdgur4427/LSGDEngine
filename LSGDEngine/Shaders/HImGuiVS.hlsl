cbuffer VertexBuffer : register(b0)
{
    float4x4 ProjectionMatrix;
};

struct VS_INPUT
{
    float2 Pos : POSITION;
    float4 Col : COLOR0;
    float2 UV : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Col : COLOR0;
    float2 UV : TEXCOORD0;
};

PS_INPUT main(VS_INPUT Input)
{
    PS_INPUT Output;
    Output.Pos = mul(ProjectionMatrix, float4(Input.Pos.xy, 0.f, 1.f));
    Output.Col = Input.Col;
    Output.UV = Input.UV;
    return Output;
}
