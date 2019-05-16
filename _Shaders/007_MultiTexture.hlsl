cbuffer VS_ViewProjection : register(b0)
{
	//�ӽ� ����������ġ : �������� (�뷮���۴�)
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_Color : register(b0)
{
    float4 Color;
}

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : UV0;
};

struct PixelInput
{
    //sementic
    float4 position : SV_POSITION;
    float2 uv : UV0;
//�ȼ��� ��ġ
};

SamplerState Sampler : register(s0);
Texture2D Map : register(t0);
Texture2D Map2 : register(t1);
Texture2D Map3 : register(r2);

//���������� ����(�ϳ��ϳ�)
//�ȼ��� ��ġ��ȯ
//�Լ��� ���������� wvp��ȯ �Ͼ
PixelInput VS(VertexInput input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.uv = input.uv;
    
    return output; // ��ȯ���� �ȼ��� ��ġ
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 t = Map.Sample(Sampler, input.uv);
    float4 t2 = Map2.Sample(Sampler, input.uv);
    float4 alpha = Map3.Sample(Sampler, input.uv);


    return (1 - alpha.r) * t + alpha.r*t2;
}