#include "000_Header.hlsl"

//  상수는 static const로 선언
static const float PI = 3.14159265f;
static const float InnerRadius = 6356.7523142; //  지구 반지름
static const float OuterRadius = 6356.7523142 * 1.0157313; //  대기 반지름

static const float ESun = 20.0f; //  태양의 밝기
static const float Kr = 0.0025f; //  Rayleigh
static const float Km = 0.0010f; //  Mie

static const float KrESun = Kr * ESun;
static const float KmESun = Km * ESun;
static const float Kr4PI = Kr * 4.0f * PI;
static const float Km4PI = Km * 4.0f * PI;

static const float Scale = 1.0f / (OuterRadius - InnerRadius); //   비율 
static const float2 RayleighMieScaleHeight = { 0.25, 0.1 };

static const float g = -0.990f;
static const float g2 = -0.990f * -0.990f;
static const float Exposure = -2.0f; //  밝은 빛을 강조 시키기 위한 상수

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 oPosition : POSITION0;
};

PixelInput VS_Scattering(VertexTexture input)
{
    PixelInput output;
    
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = -input.Position.xyz;
    output.Uv = input.Uv;

    return output;
}

Texture2D RayleighMap : register(t10);
Texture2D MieMap : register(t11);
Texture2D StarMap : register(t12);

SamplerState RayleighSampler : register(s10);
SamplerState MieSampler : register(s11);
SamplerState StarSampler : register(s12);

// Phase 위상, 단계
float GetRayleighPhase(float c)
{
    return 0.75f * (1.0f + c);
}

float GetMiePhase(float c1, float c2)
{
    float3 result = 0;

    result.x = 1.5f * ((1.0f - g2) / (2.0f + g2));
    result.y = 1.0f + g2;
    result.z = 2.0f * g;
    return result.x * (1.0 + c2) / pow(result.y - result.z * c1, 1.5);
}

float3 HDR(float3 LDR)
{
    return 1.0f - exp(Exposure * LDR);
}

cbuffer PS_Scatter : register(b10)
{
    //int Select;
    float StarIntensity;
    float MoonAlpha;
    
    float4 SunColor;
}

float4 PS_Scattering(PixelInput input) : SV_TARGET
{
    float3 sunDirection = -normalize(Direction);

    float temp = dot(sunDirection, input.oPosition) / length(input.oPosition);
    float temp2 = temp * temp;

    float3 rSamples = RayleighMap.Sample(RayleighSampler, input.Uv);
    float3 mSamples = MieMap.Sample(MieSampler, input.Uv);

    float3 color = 0;
   
    color = GetRayleighPhase(temp2) * rSamples + GetMiePhase(temp, temp2) * mSamples;

    //if (Select == 0)
    //    color = GetRayleighPhase(temp2) * rSamples + GetMiePhase(temp, temp2) * mSamples;
    //else if (Select == 1)
    //    color = GetRayleighPhase(temp2) * rSamples;
    //else if (Select == 2)
    //    color = GetMiePhase(temp, temp2) * mSamples;

    color = HDR(color);

    color += max(0, (1 - color)) * float3(0.05f, 0.05f, 0.1f);

    float intensity = saturate(StarIntensity);
    return float4(color, 1) + StarMap.Sample(StarSampler, input.Uv) * intensity;
}

///////////////////////////////////////////////////////////////////////////////

cbuffer PS_Target : register(b11)
{
    float3 WaveLength;
    int SampleCount;

    float3 InvWaveLength;
    float PS_Target_Padding2;

    float3 WaveLengthMie;
    float PS_Target_Padding3;
}

struct PixelTargetInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};


PixelTargetInput VS_Target(VertexTexture input)
{
    PixelTargetInput output;
    
    //  NDC 공간
    output.Position = input.Position;
    output.Uv = input.Uv;
    
    return output;
}

struct PixelTargetOutput
{
    float4 RColor : SV_TARGET0;
    float4 MColor : SV_TARGET1;
};

float HitOuterSphere(float3 position, float3 direction)
{
    float3 light = -position;
    
    float b = dot(light, direction);
    float c = dot(light, light);

    float d = c - b * b;
    float q = sqrt(OuterRadius * OuterRadius - d);

    float t = b;
    t += q;

    return t;
}

float2 GetDensityRatio(float height)
{
    float altitude = (height - InnerRadius) * Scale;

    return exp(-altitude / RayleighMieScaleHeight.xy);
}

float2 GetDistance(float3 p1, float3 p2)
{
    float2 opticalDepth = 0;

    float3 temp = p2 - p1;
    float far = length(temp);
    float3 direction = temp / far;

    float sampleLength = far / SampleCount;
    float scaledLength = sampleLength * Scale;

    float3 sampleRay = direction * sampleLength;
    p1 += sampleRay * 0.5f;

    for (int i = 0; i < SampleCount; i++)
    {
        float height = length(p1);
        opticalDepth += GetDensityRatio(height);

        p1 += sampleRay;
    }
    
    return opticalDepth * scaledLength;
}

PixelTargetOutput PS_Target(PixelTargetInput input) : SV_TARGET
{
    PixelTargetOutput output;
    
    float2 uv = input.Uv;

    float3 pointPv = float3(0, InnerRadius + 1e-3f, 0.0f);
    float angleY = 100.0f * uv.x * PI / 180.0f;
    float angleXZ = PI * uv.y;

    float3 direction;
    direction.x = sin(angleY) * cos(angleXZ);
    direction.y = cos(angleY);
    direction.z = sin(angleY) * sin(angleXZ);
    direction = normalize(direction);

    float farPvPa = HitOuterSphere(pointPv, direction);
    float3 ray = direction;

    float3 pointP = pointPv;
    float sampleLength = farPvPa / SampleCount;
    float scaledLength = sampleLength * Scale;
    float3 sampleRay = ray * sampleLength;
    pointP += sampleRay * 0.5f;

    float3 sunDir = -normalize(Direction);

    float3 rayleighSum = 0;
    float3 mieSum = 0;

    for (int i = 0; i < SampleCount; i++)
    {
        float pHeight = length(pointP);
        
        float2 densityRatio = GetDensityRatio(pHeight);
        densityRatio *= scaledLength;

        float2 viewerOpticalDepth = GetDistance(pointP, pointPv);

        float farPPc = HitOuterSphere(pointP, sunDir);
        float2 sunOpticalDepth = GetDistance(pointP, pointP + sunDir * farPPc);

        float2 opticalDepthP = sunOpticalDepth.xy + viewerOpticalDepth.xy;
        float3 attenuation = exp(-Kr4PI * InvWaveLength * opticalDepthP.x - Km4PI * opticalDepthP.y);

        rayleighSum += densityRatio.x * attenuation;
        mieSum += densityRatio.y * attenuation;

        pointP += sampleRay;
    }

    float3 rayleigh = rayleighSum * KrESun;
    float3 mie = mieSum * KmESun;

    rayleigh *= InvWaveLength;
    mie *= WaveLengthMie;

    output.RColor = float4(rayleigh, 1);
    output.MColor = float4(mie, 1);

    return output;
}

///////////////////////////////////////////////////////////////////////////////

struct PixelMoonInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

PixelMoonInput VS_Moon(VertexTexture input)
{
    PixelMoonInput output;
    
    //  NDC 공간
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    
    return output;
}

Texture2D MoonMap : register(t13);
SamplerState MoonSampler : register(s13);

float4 PS_Moon(PixelMoonInput input) : SV_TARGET
{
    float4 color = MoonMap.Sample(MoonSampler, input.Uv)*StarIntensity;

    return color;
}

///////////////////////////////////////////////////////////////////////////////

static const float ONE = 0.00390625f;
static const float ONEHALF = 0.001953125f;

static const float CloudCover = -0.1;
static const float CloudSharpness = 0.125;

Texture2D CloudMap : register(t14);
SamplerState CloudSampler : register(s14);

cbuffer VS_CloudBuffer : register(b10)
{
    float NumTiles;
}

struct PixelCloudInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float2 oUv : UV1;
};

PixelCloudInput VS_Cloud(VertexTexture input)
{
    PixelCloudInput output;
    
    //  NDC 공간
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Uv = (input.Uv * NumTiles);
    output.oUv = input.Uv;
    
    return output;
}

float fade(float t)
{
  // return t*t*(3.0-2.0*t);
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float noise(float2 P)
{
    float2 Pi = ONE * floor(P) + ONEHALF;
    float2 Pf = frac(P);

    float2 grad00 = CloudMap.Sample(CloudSampler, Pi).rg * 4.0 - 1.0;
    float n00 = dot(grad00, Pf);

    float2 grad10 = CloudMap.Sample(CloudSampler, Pi + float2(ONE, 0.0)).rg * 4.0 - 1.0;
    float n10 = dot(grad10, Pf - float2(1.0, 0.0));

    float2 grad01 = CloudMap.Sample(CloudSampler, Pi + float2(0.0, ONE)).rg * 4.0 - 1.0;
    float n01 = dot(grad01, Pf - float2(0.0, 1.0));

    float2 grad11 = CloudMap.Sample(CloudSampler, Pi + float2(ONE, ONE)).rg * 4.0 - 1.0;
    float n11 = dot(grad11, Pf - float2(1.0, 1.0));

    float2 n_x = lerp(float2(n00, n01), float2(n10, n11), fade(Pf.x));

    float n_xy = lerp(n_x.x, n_x.y, fade(Pf.y));

    return n_xy;
}

float4 PS_Cloud(PixelCloudInput input) : SV_TARGET
{
    float n = noise(input.Uv + MoonAlpha);
    float n2 = noise(input.Uv * 2 + MoonAlpha);
    float n3 = noise(input.Uv * 4 + MoonAlpha);
    float n4 = noise(input.Uv * 8 + MoonAlpha);
	
    float nFinal = n + (n2 / 2) + (n3 / 4) + (n4 / 8);
	
    float c = CloudCover - nFinal;
    if (c < 0) 
        c = 0;
 
    float CloudDensity = 1.0 - pow(CloudSharpness, c);
    
    float4 retColor = CloudDensity;
    retColor *= SunColor;
    
    float uvX = abs(0.5f - input.oUv.x) + 0.5f;
    float uvY = abs(0.5f - input.oUv.y) + 0.5f;
    
    retColor.a = uvX > 0.8 ? lerp(0.0f, retColor.a * 0.85f, (1 - uvX) / 0.2f) : retColor.a;
    retColor.a = uvY > 0.8 ? lerp(0.0f, retColor.a * 0.85f, (1 - uvY) / 0.2f) : retColor.a;
    
    return retColor;
}