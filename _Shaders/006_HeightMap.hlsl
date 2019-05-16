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
};

struct PixelInput
{
    //sementic
    float4 position : SV_POSITION;
    float4 color : COLOR0;
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

    float4 color = float4(1, 1, 1, 1);
    if (input.position.y > 10)
        color = float4(1, 1, 0, 1);
    if (input.position.y > 20)
        color = float4(1, 0, 0, 1);
    if (input.position.y > 30)
        color = float4(0, 1, 0, 1);
    if(input.position.y>50)
        color = float4(0, 0, 1, 1);

    output.color = color;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

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

    return input.color;
}