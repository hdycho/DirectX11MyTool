#include "000_header.hlsl"

cbuffer VS_Ocean : register(b10)
{
    float WaveFrequency;    //주파수주기(파동)
    float WaveAmplitude;    //진폭(높낮이)

    float2 TextureScale;    //실제텍스쳐의 비율크기 

    float2 BumpSpeed;      
    float BumpHeight;
    float RunningTime;      //흐르는시간
}

cbuffer PS_Ocean : register(b10)
{
    float4 ShallowColor;    //얕은색
    float4 DeepColor;       //깊은색

    float FresnelBias; //편향
    float FresnelPower; //강도
    float FresnelAmount; //보정값
    float ShoreBlend; //알파값 변화정도

    float2 OceanSize; //버텍스의 전체크기
    float HeightRatio; //높이 보정값
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 oPosition : POSITION0;
    float4 wPosition : POSITION1;
    float2 TexScale : TEXSCALE0;
    float3 Normal : NORMAL0;
    float3 View : VIEW0;
    
    float2 Bump[3] : BUMP0;
    float3 Tangent[3] : TANGENT0;
};

struct Wave
{
    float Frequency;    //파동(주기)
    float Amplitude;    //진폭(높낮이)
    float Phase;        //위상(시작위치)
    float2 Direction;   //방향
};

//진폭을 만드는 공식
float EvaluateWave(Wave wave,float2 position,float time)
{
    float s = sin(dot(wave.Direction, position) * wave.Frequency + time) + wave.Phase;

    return wave.Amplitude * s;
}

//노말을 계산하기 위해 필요
//축값의 차(sin^2+cos^2=1의 공식을 보면 sin값이증가하면 cos값은 줄어든다)
float EvaluateDifferent(Wave wave, float2 position, float time)
{
    float s = cos(dot(wave.Direction, position) * wave.Frequency + time) + wave.Phase;

    return wave.Amplitude *wave.Frequency* s;
}

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    
    Wave wave[3] =
    {
        0.0f, 0.0f, 0.50f, float2(-1.0f, 0.0f),
        0.0f, 0.0f, 1.30f, float2(-0.7f, -0.7f),
        0.0f, 0.0f, 0.25f, float2(0.2f, 0.1f)
    };

    wave[0].Frequency = WaveFrequency;
    wave[0].Amplitude = WaveAmplitude;
    wave[1].Frequency = WaveFrequency * 2.0f;
    wave[1].Amplitude = WaveAmplitude * 0.5f;
    wave[2].Frequency = WaveFrequency * 3.0f;
    wave[2].Amplitude = WaveAmplitude * 1.0f;

    //노멀벡터를 만들값
    float ddx = 0, ddy = 0;

    for (int i = 0; i < 3; i++)
    {
        input.Position.y += EvaluateWave(wave[i], input.Position.xz, RunningTime);

        float diff = EvaluateDifferent(wave[i], input.Position.xz, RunningTime);
        ddx += diff * wave[i].Direction.x; 
        ddy += diff * wave[i].Direction.y;
    }

    float3 T = float3(1, ddx, 0);
    float3 B = float3(-ddx, 1, -ddy);
    float3 N = float3(0, ddy, 1);

    //탄젠트 공간생성
    float3x3 matTangent = float3x3
    (
        normalize(T)*BumpHeight,
        normalize(B)*BumpHeight,
        normalize(N)
    );

    //탄젠트 공간을 월드공간으로 변환
    output.Tangent[0] = mul(World[0].xyz, matTangent); 
    output.Tangent[1] = mul(World[1].xyz, matTangent); 
    output.Tangent[2] = mul(World[2].xyz, matTangent);

    output.TexScale = input.Uv * TextureScale;

    float tempTime = fmod(RunningTime, 100); // 100으로 나눈나머지값

    //범프값은 노멀맵을 매핑하는 값이된다.
    output.Bump[0] = output.TexScale + tempTime * BumpSpeed;
    output.Bump[1] = output.TexScale*2.0f + tempTime * BumpSpeed*4.0f;
    output.Bump[2] = output.TexScale*4.0f + tempTime * BumpSpeed*8.0f;

    output.oPosition = input.Position;

    output.Position = mul(input.Position, World); //  월드 공간
    output.wPosition = output.Position;
    output.Position = mul(output.Position, View); //  눈의 위치와 회전 각
    output.Position = mul(output.Position, Projection); //  투영

    //뷰행렬의 역행렬은 뷰의 위치(카메라를 바라보는 방향)
    //output.View = ViewInverse._41_42_43 - output.wPosition.xyz;
    output.View = WorldViewDirection(output.wPosition);

    return output;
}

Texture2D HeightMap : register(t5);
SamplerState HeightSampler : register(s5);

//버텍스 포지션이 바다의 크기안에 들어오는지 판별
bool WithInBound(float3 position)
{
    return 
    (
    position.x > 0.0f && 
    position.z > 0.0f && 
    position.x < OceanSize.x 
    && position.z < OceanSize.y
    );
}

//깊이차를 계산하는 함수
//color는 높이값이 들어가있음
//높이에 대한 비율을 반환=>투명도가 됨
float EvaluateShoreBlend(float3 position)
{
    float2 temp = float2(position.x / OceanSize.x, position.z / OceanSize.y);
    float color = HeightMap.Sample(HeightSampler, temp).r/HeightRatio;

    return 1.0f - color * ShoreBlend;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //색상값을 축으로 변환
    float4 t0 = NormalMap.Sample(NormalSampler, input.Bump[0]) * 2.0f - 1.0f;
    float4 t1 = NormalMap.Sample(NormalSampler, input.Bump[1]) * 2.0f - 1.0f;
    float4 t2 = NormalMap.Sample(NormalSampler, input.Bump[2]) * 2.0f - 1.0f;

    //float3 normal = t0.xyz;
    float3 normal = t0.xyz + t1.xyz + t2.xyz;

    float3x3 matTangent;
    matTangent[0] = input.Tangent[0];
    matTangent[1] = input.Tangent[1];
    matTangent[2] = input.Tangent[2];

    normal = mul(normal, matTangent);
    normal = normalize(normal);

    float4 color = 0;

    //플레넬방정식=>면에대한 반사
    float facing = 1.0f - saturate(dot(input.View, normal));
    float fresnel = FresnelBias + (1.0f - FresnelBias) * pow(facing, FresnelPower);

    float alpha = 0;
    color = lerp(DeepColor, ShallowColor, facing);

    if(ShoreBlend>0&&WithInBound(input.oPosition.xyz))
    {
        alpha = EvaluateShoreBlend(input.oPosition.xyz);
        color.rgb = lerp(2, color.rgb, color.a);
    }
    color.rgb = color.rgb * Ambient.rgb /** fresnel*/;

    DiffuseLighting(color, Diffuse, normal);
    SpecularLighting(color, normal, input.View);

    return float4(color.rgb, alpha);
}
