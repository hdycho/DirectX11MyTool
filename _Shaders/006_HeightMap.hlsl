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
//픽셀의 위치
};

SamplerState Sampler : register(s0);
Texture2D Map : register(t0);
Texture2D Map2 : register(t1);

//정점데이터 전달(하나하나)
//픽셀의 위치반환
//함수에 들어온정점은 wvp변환 일어남
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

    return output; // 반환값이 픽셀의 위치
}

float4 PS(PixelInput input) : SV_TARGET
{
	//색은 float4 = > r,g,b,a
    //return Color;
    //이미지를 화면에 출력할때 조정하는 단계 Sample
    //쉐이더에선 전부 float처리됨
    //float의 오차범위와 오차가 발생하는 이유는?
    //int a = 10;

    return input.color;
}