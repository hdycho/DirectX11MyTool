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
    float2 uv : UV0;
};

struct PixelInput
{
    //sementic
    float4 position : SV_POSITION;
    float2 uv : UV0;
//픽셀의 위치
};

SamplerState Sampler : register(s0);
Texture2D Map : register(t0);
Texture2D Map2 : register(t1);
Texture2D Map3 : register(r2);

//정점데이터 전달(하나하나)
//픽셀의 위치반환
//함수에 들어온정점은 wvp변환 일어남
PixelInput VS(VertexInput input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.uv = input.uv;
    
    return output; // 반환값이 픽셀의 위치
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 t = Map.Sample(Sampler, input.uv);
    float4 t2 = Map2.Sample(Sampler, input.uv);
    float4 alpha = Map3.Sample(Sampler, input.uv);


    return (1 - alpha.r) * t + alpha.r*t2;
}