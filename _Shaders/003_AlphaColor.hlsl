cbuffer VS_ViewProjection : register(b0)
{
	//임시 고속저장장치 : 레지스터 (용량이작다)
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_AlphaColor : register(b10)
{
    float4 Color;
    float PickState;
}

struct VertexInput
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
//픽셀의 위치
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.color = input.color;
    return output; // 반환값이 픽셀의 위치
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Color;

    if(PickState==1)
        color.a = 1;
    else if(PickState==0)
        color.a = 1;
    return Color;
    //return input.color;
}