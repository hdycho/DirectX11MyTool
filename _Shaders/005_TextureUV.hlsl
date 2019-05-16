cbuffer VS_ViewProjection : register(b0)
{
	//�ӽ� ���������ġ : �������� (�뷮���۴�)
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
    float4 position : SV_POSITION;
    float2 uv : UV0;
//�ȼ��� ��ġ
};

SamplerState Sampler : register(s0);
Texture2D Map : register(t0);
Texture2D Map2 : register(t1);

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
	//���� float4 = > r,g,b,a
    //return Color;
    //�̹����� ȭ�鿡 ����Ҷ� �����ϴ� �ܰ� Sample
    //���̴����� ���� floató����
    //float�� ���������� ������ �߻��ϴ� ������?
    //int a = 10;
    float4 color = Map.Sample(Sampler, input.uv);
    float x = input.uv.x;
    if(x<0.5f)
        color = Map2.Sample(Sampler, input.uv);


    return color;
}