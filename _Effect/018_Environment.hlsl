#include "000_EffectHeader.hlsl"


//***********************************************************************//
//*****************************TerrainEffect*****************************//
//***********************************************************************//
// --------------------------------------------------------------------- //
//  Global Variables
// --------------------------------------------------------------------- //
cbuffer CB_Terrain
{
    float4 FogColor;
    float FogStart;
    float FogRange;

    float MinDistance;
    float MaxDistance;
    float MinTessellation;
    float MaxTessellation;

    float TexelCellSpaceU;
    float TexelCellSpaceV;
    float WorldCellSpace;

    int TexScale;
    float CB_Terrain_Padding;

    float4 WorldFrustumPlanes[6];
};

float2 HeightMapSize; // HeightMap 가로/세로 크기
float2 TerrainSize; // 지형맵 버텍스 실제 위치의 크기

float MaxHeight;
float4 BlendPositionRate;
float4 ActiveBlend;

//브러쉬 타입
int Type;
float3 Location;
int Range;
float3 Color;

//터레인디테일 타입
float DetailValue = 0.999;
float DetailIntensity = 1;

//터레인라인 타입
int LineType;
float3 LineColor;
int Spacing; // 한칸의 넓이
float Thickness;

float3 BrushColor(float3 location)
{
    //location은 버텍스위치
    //Location은 지형의 피킹 위치
    if (Type == 0)
        return float3(0, 0, 0);
    //사각형일때
    if (Type == 1)
    {
        if ((location.x >= (Location.x - Range)) &&
            (location.x <= (Location.x + Range)) &&
            (location.z >= (Location.z - Range)) &&
            (location.z <= (Location.z + Range)))
        {
            return Color;
        }
    }
    //원일때
    if (Type == 2)
    {
        float dx = location.x - Location.x;
        float dy = location.z - Location.z;

        float dist = sqrt(dx * dx + dy * dy);

        if (dist <= Range)
            return Color;
    }

    return float3(0, 0, 0);
}

float3 Line(float3 oPosition)
{
    //Line Drawing
    [branch] // 둘다실행하고 하나만 선택함 if else if사용가능
    if (LineType == 1)
    {
        float2 grid = float2(0, 0);
        grid.x = frac(oPosition.x / (float) Spacing);
        grid.y = frac(oPosition.z / (float) Spacing);

        [flatten]
        if (grid.x < Thickness || grid.y < Thickness)
            return LineColor;
    }
    else if (LineType == 2)
    {
        float2 grid = oPosition.xz / (float) Spacing;

        //끝지점을 맞추는작업(한칸의 범위)
        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        
        //편미분(fwidth) 
        //월드상에선 xz가 스크린상에선 xy로 나타내지는것에
        //기울기를 구하기위해 편미분 사용
        float2 speed = fwidth(range);

        float2 pixel = range / speed;
        float weights = saturate(min(pixel.x, pixel.y) - Thickness);

        return lerp(LineColor, float3(0, 0, 0), weights);
    }

    return float3(0, 0, 0);
}
// --------------------------------------------------------------------- //
//  Vertex Shader
// --------------------------------------------------------------------- //

Texture2D HeightMap;
SamplerState HeightMapSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct TerrainVertexInput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float2 BoundsY : BoundsY0;
};

struct TerrainVertexOutput
{
    float4 Position : POSITION0;
    float2 Uv : Uv0;
    float2 BoundsY : BoundsY0;
};

TerrainVertexOutput VS_Terrain(TerrainVertexInput input)
{
    TerrainVertexOutput output;
    output.Position = input.Position;
    output.Position.y = HeightMap.SampleLevel(HeightMapSampler, input.Uv, 0).r;

    output.Uv = input.Uv;
    output.BoundsY = input.BoundsY;

    //  프랙탈 알고리즘

    return output;
}

// --------------------------------------------------------------------- //
//  Hull Shader
// --------------------------------------------------------------------- //
float CalcTessFactor(float3 position)
{
    float d = distance(position, ViewPosition);
    float s = saturate((d - MinDistance) / (MaxDistance - MinDistance));

    return pow(2, lerp(MaxTessellation, MinTessellation, s));
}

bool AabbOutsideFrustumTest(float3 center, float3 extents)
{
    [unroll]
    for (int i = 0; i < 6; ++i)
    {
      // If the box is completely behind any of the frustum planes
      // then it is outside the frustum.
        [flatten]
        if (AabbBehindPlaneTest(center, extents, WorldFrustumPlanes[i]))
        {
            return true;
        }
    }
    return false;
}

struct ConstantOutput
{
    float Edges[4] : SV_TessFactor;
    float Inside[2] : SV_InsidetessFactor;
};

ConstantOutput HS_Constant_Terrain(InputPatch<TerrainVertexOutput, 4> input, uint patchId : SV_PrimitiveId)
{
    ConstantOutput output;

    float minY = input[0].BoundsY.x;
    float maxY = input[0].BoundsY.y;

    float3 vMin = float3(input[2].Position.x, minY, input[2].Position.z);
    float3 vMax = float3(input[1].Position.x, maxY, input[1].Position.z);

    float3 boxCenter = (vMin + vMax) * 0.5f;
    float3 boxExtents = (vMax - vMin) * 0.5f;

    if (AabbOutsideFrustumTest(boxCenter, boxExtents))
    {
        output.Edges[0] = 0.0f;
        output.Edges[1] = 0.0f;
        output.Edges[2] = 0.0f;
        output.Edges[3] = 0.0f;

        output.Inside[0] = 0.0f;
        output.Inside[1] = 0.0f;
        
        return output;
    }

    float3 e0 = (input[0].Position + input[2].Position).xyz * 0.5f;
    float3 e1 = (input[0].Position + input[1].Position).xyz * 0.5f;
    float3 e2 = (input[1].Position + input[3].Position).xyz * 0.5f;
    float3 e3 = (input[2].Position + input[3].Position).xyz * 0.5f;
    float3 c = (input[0].Position + input[1].Position + input[2].Position + input[3].Position).xyz * 0.25f;

    output.Edges[0] = CalcTessFactor(e0);
    output.Edges[1] = CalcTessFactor(e1);
    output.Edges[2] = CalcTessFactor(e2);
    output.Edges[3] = CalcTessFactor(e3);

    output.Inside[0] = CalcTessFactor(c);
    output.Inside[1] = output.Inside[0];

    return output;
}

ConstantOutput HS_Constant_Depth_Terrain(InputPatch<TerrainVertexOutput, 4> input, uint patchId : SV_PrimitiveId)
{
    ConstantOutput output;

    float minY = input[0].BoundsY.x;
    float maxY = input[0].BoundsY.y;

    float3 vMin = float3(input[2].Position.x, minY, input[2].Position.z);
    float3 vMax = float3(input[1].Position.x, maxY, input[1].Position.z);

    float3 boxCenter = (vMin + vMax) * 0.5f;
    float3 boxExtents = (vMax - vMin) * 0.5f;
    

    float3 e0 = (input[0].Position + input[2].Position).xyz * 0.5f;
    float3 e1 = (input[0].Position + input[1].Position).xyz * 0.5f;
    float3 e2 = (input[1].Position + input[3].Position).xyz * 0.5f;
    float3 e3 = (input[2].Position + input[3].Position).xyz * 0.5f;
    float3 c = (input[0].Position + input[1].Position + input[2].Position + input[3].Position).xyz * 0.25f;

    output.Edges[0] = CalcTessFactor(e0);
    output.Edges[1] = CalcTessFactor(e1);
    output.Edges[2] = CalcTessFactor(e2);
    output.Edges[3] = CalcTessFactor(e3);

    output.Inside[0] = CalcTessFactor(c);
    output.Inside[1] = output.Inside[0];

    return output;
}

struct HullOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
};

//  line
//  line_adj
//  tri
//  tri_adj
[domain("quad")]
//  integer : 무조건 올림 변환
//  fractional_odd : 홀수 일 때 선 분할
//  fractional_even : 짝수 일 때 선 분할
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant_Terrain")]
[maxtessfactor(64.0f)] //  사용해도되고 안해도 되지만 사용하면 속도가 조금 빨라짐
HullOutput HS_Terrain(InputPatch<TerrainVertexOutput, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveId)
{
    HullOutput output;
    output.Position = input[pointID].Position;
    output.Uv = input[pointID].Uv;

    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("HS_Constant_Depth_Terrain")]
[maxtessfactor(64.0f)] //  사용해도되고 안해도 되지만 사용하면 속도가 조금 빨라짐
HullOutput HS_Depth_Terrain(InputPatch<TerrainVertexOutput, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveId)
{
    HullOutput output;
    output.Position = input[pointID].Position;
    output.Uv = input[pointID].Uv;

    return output;
}
// --------------------------------------------------------------------- //
//  Domain Shader
// --------------------------------------------------------------------- //
struct DomainOutput
{
    float4 Position : SV_Position0;
    float3 wPosition : Position1;
    float4 pPosition : Position2;
    float2 Uv : Uv0;
    float2 TiledUv : Uv1;
    float3 BrushColor : BrushColor0;
    float4 ShadowPos : Uv2;
};

[domain("quad")]
DomainOutput DS_Terrain(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;
    
    float3 p1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, uv.x).xyz;
    float3 p2 = lerp(patch[2].Position.xyz, patch[3].Position.xyz, uv.x).xyz;
    
    float2 uv1 = lerp(patch[0].Uv.xy, patch[1].Uv.xy, uv.x);
    float2 uv2 = lerp(patch[2].Uv.xy, patch[3].Uv.xy, uv.x);
    output.Uv = lerp(uv1, uv2, uv.y);

    output.wPosition = lerp(p1, p2, uv.y);
    output.wPosition.y = HeightMap.SampleLevel(HeightMapSampler, output.Uv, 0).r;

    output.wPosition = mul(float4(output.wPosition, 1), World);
    output.ShadowPos = mul(float4(output.wPosition, 1), ShadowTransform);
    output.BrushColor = BrushColor(output.wPosition);

    output.Position = mul(float4(output.wPosition, 1), View);
    output.Position = mul(output.Position, Projection);
    
    output.pPosition = output.Position;
    // Tile layer textures over terrain.
    output.TiledUv = output.Uv * TexScale;

    return output;
}

[domain("quad")]
DomainOutput DS_Depth_Terrain(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;
    
    float3 p1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, uv.x).xyz;
    float3 p2 = lerp(patch[2].Position.xyz, patch[3].Position.xyz, uv.x).xyz;
    
    float2 uv1 = lerp(patch[0].Uv.xy, patch[1].Uv.xy, uv.x);
    float2 uv2 = lerp(patch[2].Uv.xy, patch[3].Uv.xy, uv.x);
    output.Uv = lerp(uv1, uv2, uv.y);

    output.wPosition = lerp(p1, p2, uv.y);
    output.wPosition.y = HeightMap.SampleLevel(HeightMapSampler, output.Uv, 0).r;

    output.wPosition = mul(float4(output.wPosition, 1), World);
    output.ShadowPos = mul(float4(output.wPosition, 1), ShadowTransform);
    output.BrushColor = BrushColor(output.wPosition);

    output.Position = mul(float4(output.wPosition, 1), LightView);
    output.Position = mul(output.Position, LightProjection);
    
    output.pPosition = output.Position;
    // Tile layer textures over terrain.
    output.TiledUv = output.Uv * TexScale;

    return output;
}

// --------------------------------------------------------------------- //
//  Pixel Shader
// --------------------------------------------------------------------- //
Texture2DArray LayerMapArray;
Texture2D BlendMap;

bool IsHovering = false;
float2 PickPosition = float2(0, 0);
float2 BrushRate = float2(50, 50);

SamplerState TrilinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

SamplerState TerrainSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct PixelTargetOutput
{
    float4 tColor : SV_TARGET0;
    float4 pColor : SV_TARGET1;
};

float4 BlendLerpHelper(float4 fColor, float4 sColor, float wPosRate, float4 BPR, float minusRate, float plusRate)
{
    float4 rColor = fColor;
    
    rColor = (BPR < 0.01f && BPR > -0.001f) ? lerp(fColor, sColor, saturate((0.01f - wPosRate) / (0.01f + plusRate - minusRate))) : lerp(fColor, sColor, saturate((wPosRate - (BPR - minusRate)) / ((BPR + plusRate) - (BPR - minusRate))));

    return rColor;
}

float4 BlendLerpHelper2(float4 fColor, float4 sColor, float wPosRate, float4 BPR, float minusRate, float plusRate)
{
    float4 rColor = 0;

    rColor = lerp(fColor, sColor, saturate((wPosRate - (BPR - minusRate)) / ((BPR + plusRate) - (BPR - minusRate))));

    rColor = (BPR < 0.01f && wPosRate < 0.01f) ? sColor : rColor;

    return rColor;
}

float4 BrushHelper(float4 color, float2 uv, Texture2D brushTexture)
{
    float4 rColor = color;

    float2 mousePos = PickPosition / HeightMapSize;
    mousePos.y = 1.0f - mousePos.y; // 픽포지션 변환작업으로 반전되어있어서 다시 되돌려줘야함.

    float2 pix1uv = 1.0f / TerrainSize;
    float2 brushAreaMinUv = mousePos - pix1uv * (BrushRate * 0.5f);
    float2 brushAreaMaxUv = mousePos + pix1uv * (BrushRate * 0.5f);

    if (
        (uv.x > brushAreaMinUv.x) && (uv.x < brushAreaMaxUv.x)
        && (uv.y > brushAreaMinUv.y) && (uv.y < brushAreaMaxUv.y)
        )
    {
        float2 brushUv = (uv - brushAreaMinUv) / (brushAreaMaxUv - brushAreaMinUv);
        float4 brushColor = brushTexture.Sample(TrilinearSampler, brushUv);

#if 0
        rColor.xyz = brushColor.a > 0.2 ? brushColor.xyz : rColor.xyz;
#else
        brushColor.a = (brushColor.r > 0.99 && brushColor.g < 0.01f && brushColor.b > 0.99) ? 0 : brushColor.a;
        rColor.xyz = brushColor.a > 0.01f ? float3(0, 1, 0) : rColor.xyz;
#endif

    }

    return rColor;
}

PixelTargetOutput PS_Terrain(DomainOutput input, uniform bool fogEnabled) : SV_TARGET
{
    PixelTargetOutput output;

    float2 left = input.Uv + float2(-TexelCellSpaceU, 0.0f);
    float2 right = input.Uv + float2(TexelCellSpaceU, 0.0f);
    float2 top = input.Uv + float2(0.0f, -TexelCellSpaceV);
    float2 bottom = input.Uv + float2(0.0f, TexelCellSpaceV);

    float leftY = HeightMap.SampleLevel(HeightMapSampler, left, 0).r;
    float rightY = HeightMap.SampleLevel(HeightMapSampler, right, 0).r;
    float topY = HeightMap.SampleLevel(HeightMapSampler, top, 0).r;
    float bottomY = HeightMap.SampleLevel(HeightMapSampler, bottom, 0).r;

    float3 tangent = normalize(float3(WorldCellSpace * 2.0f, rightY - leftY, 0.0f));
    float3 biTangent = normalize(float3(0.0f, bottomY - topY, WorldCellSpace * -2.0f));
    float3 normalW = cross(tangent, biTangent);
    
    float3 eye = ViewPosition - input.wPosition;
    float distanceToEye = length(eye);
    eye /= distanceToEye;

    float depth = input.pPosition.z / input.pPosition.w;
    float2 zValue = float2(1, 1);

    [branch]
    if (depth > DetailValue) //0.999
    {
        zValue = input.TiledUv * 0.25f;
    }
    else
    {
        zValue = input.TiledUv;
    }

    //======================터레인자체==========================//
    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);
    
    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { LightAmbient, LightDiffuse, LightSpecular, LightDirection };

    float4 A, D, S;

    float4 diffuseColor = DiffuseMap.Sample(TerrainSampler, zValue);
    float4 color = diffuseColor;

    float4 specularColor = SpecularMap.Sample(TerrainSampler, zValue);

    float4 detailMap = DetailMap.Sample(TerrainSampler, zValue);

    float4 normalMap = NormalMap.Sample(TerrainSampler, zValue);
    float3 normalValue;


    if (depth <= DetailValue)
    {
        if (normalMap.r != 0)
        {
            normalValue = NormalSampleToWorldSpace(normalMap.xyz, normalW, tangent);
            NormalMapping(color, normalMap, normalValue, tangent);
        }
        else
            normalValue = normalW;
    }
 
    if (specularColor.r != 0)
    {
        SpecularLighting(color, specularColor, normalValue, -ViewDirection);
    }

    if (detailMap.r != 0)
    {
        color = color * detailMap * DetailIntensity;
    }
    
    
    // Sample layers in texture array.
    
    float4 c0 = LayerMapArray.Sample(TerrainSampler, float3(zValue, 0));
    float4 c1 = LayerMapArray.Sample(TerrainSampler, float3(zValue, 1));
    float4 c2 = LayerMapArray.Sample(TerrainSampler, float3(zValue, 2));
    
    // Sample the blend map.
    float4 t = BlendMap.Sample(TerrainSampler, input.Uv);
    
    float pRate = input.wPosition.y / MaxHeight;
    
    if (ActiveBlend.x == 1)
        color = BlendLerpHelper(color, c0, pRate, BlendPositionRate[0], 0.05f, 0.05f);
    if (ActiveBlend.y == 1)
        color = BlendLerpHelper(color, c1, pRate, BlendPositionRate[1], 0.15f, 0.15f);
    if (ActiveBlend.z == 1)
        color = BlendLerpHelper(color, c2, pRate, BlendPositionRate[2], 0.01f, 0.01f);
    
    float shadow = 1.0f;
    shadow = CalcShadowFactor(samShadow, ShadowMap, input.ShadowPos);
   
    ambient = float3(0, 0, 0);
    diffuse = float3(0, 0, 0);
    specular = float3(0, 0, 0);
    
    Material m2 = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l2 = { LightAmbient, LightDiffuse, LightSpecular, LightDirection };
    
    ComputeDirectionalLight(m2, l2, normalW, eye, A, D, S);

    ambient += A * color;
    diffuse += D * color * shadow;
    specular += S * color * shadow;

    float4 result2 = float4(saturate(ambient + diffuse + specular), 1.0f);

    [flatten]
    if (fogEnabled == true)
    {
        float fogFactor = saturate((distanceToEye - FogStart) / FogRange);

        result2 = lerp(result2, FogColor, fogFactor);
    }


    output.tColor = result2 + float4(input.BrushColor, 1);
    output.tColor = output.tColor + float4(Line(input.wPosition), 0);
    

    output.pColor = float4(abs(input.Uv.x), abs(1 - input.Uv.y), 0, 1);

    return output;
}

void PS_Depth_Alpha_Terrain(DomainOutput input)
{
    float4 diffuse = DiffuseMap.Sample(samLinear, input.Uv);

    // Don't write transparent pixels to the shadow map.
    clip(diffuse.a - 0.15f);
}
//***********************************************************************//
//***********************************************************************//
//***********************************************************************//



//***********************************************************************//
//******************************billboard********************************//
//***********************************************************************//
// --------------------------------------------------------------------- //
//  Vertex Shader
// --------------------------------------------------------------------- //
struct BillboardVertexInput
{
    float4 Position : POSITION0;
    
    float3 TransPosition : INSTANCE0;
    matrix DeTransWorld : INSTANCE1;
    matrix RotateWorld : INSTANCE5;
    float2 Size : INSTANCE9;
    uint TextureNumber : INSTANCE10;

    uint InstanceId : SV_INSTANCEID0;
};

struct BillboardVertexOutput
{
    float4 Center : POSITION0;
    float2 Size : SIZE0;
    uint TextureNumber : GRASS0;
    matrix RotateWorld : MATRIX0;
    matrix DeTransWorld : MATRIX4;
    uint InstanceId : INSTANCEID0;
};

BillboardVertexOutput VS_Billboard(BillboardVertexInput input)
{
    BillboardVertexOutput output;

    output.Center = float4(input.TransPosition, 1);

    output.Size = input.Size;
    output.TextureNumber = input.TextureNumber;
    output.RotateWorld = input.RotateWorld;
    output.DeTransWorld = input.DeTransWorld;
    output.InstanceId = input.InstanceId;
    
    return output;
}

// --------------------------------------------------------------------- //
//  Geometry Shader
// --------------------------------------------------------------------- //
struct GeometryOutput
{
    float4 Position : SV_POSITION;
    float3 wPosition : Position1;
    float2 Uv : UV0;
    uint TextureNumber : GRASS0;
    float4 ShadowTransform : UV1;
    uint InstanceId : INSTANCEID0;
};

static const float2 TexCoord[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};


[maxvertexcount(4)] //  최대 넘겨 보낼 버텍스 개수
void GS_Billboard(point BillboardVertexOutput input[1], uint PrimitiveID : SV_PrimitiveID, inout TriangleStream<GeometryOutput> stream)
{
    float4 root = input[0].Center;

    float cameraDistance = length(ViewPosition.xz - root.xz);

    // Properties of the grass blade
    float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;
    
	// Animation
    float toTheLeft = sin(Time);


    // Generating vertices
    float3 up = float3(0, 1, 0);
    float3 look = ViewPosition - input[0].Center.xyz;
    look.y = 0.0f;
    look = normalize(look);

    float3 right = cross(up, look);

    
    float3 boxExtents = (halfWidth + halfHeight) * 0.5f;
    if (AabbOutsideFrustumTest(input[0].Center.xyz, boxExtents))
    {
        return;
    }
    //  4 정점 계산
    float4 v[4];

    v[0] = float4(input[0].Center.xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center.xyz + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center.xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center.xyz - halfWidth * right + halfHeight * up, 1.0f);

    GeometryOutput output;

    [unroll]
    for (int i = 0; i < 4; ++i) //  ++i 가 i++ 보다 빠름
    {
        output.wPosition = input[0].Center.xyz;

        output.Position = mul(v[i], input[0].RotateWorld);
        output.Position = mul(output.Position, input[0].DeTransWorld);
        output.Position = mul(output.Position, View);
        output.Position = mul(output.Position, Projection);

        output.ShadowTransform = mul(float4(output.wPosition, 1), ShadowTransform);
  
        output.Uv = TexCoord[i];
        output.TextureNumber = input[0].TextureNumber;
        output.InstanceId = input[0].InstanceId;

        stream.Append(output);
    }

}

[maxvertexcount(4)] //  최대 넘겨 보낼 버텍스 개수
void GS_Depth_Billboard(point BillboardVertexOutput input[1], uint PrimitiveID : SV_PrimitiveID, inout TriangleStream<GeometryOutput> stream)
{
    float4 root = input[0].Center;

    float cameraDistance = length(ViewPosition.xz - root.xz);

    // Properties of the grass blade
    float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;
    
	// Animation
    float toTheLeft = sin(Time);


    // Generating vertices
    float3 up = float3(0, 1, 0);
    float3 lightPos = -LightDirection * 500.0f;
    float3 look = lightPos - input[0].Center.xyz;
    look.y = 0.0f;
    look = normalize(look);

    float3 right = cross(up, look);

    //  4 정점 계산
    float4 v[4];

    v[0] = float4(input[0].Center.xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center.xyz + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center.xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center.xyz - halfWidth * right + halfHeight * up, 1.0f);


    GeometryOutput output;
    [unroll]
    for (int i = 0; i < 4; ++i) //  ++i 가 i++ 보다 빠름
    {
        //output.Position = mul(v[i], LightView);
        //output.Position = mul(output.Position, LightProjection);
     
        //output.Uv = TexCoord[i];
        //output.BillboardNum = input[0].BillboardNum;

        //stream.Append(output);

        output.wPosition = v[i].xyz;
        output.Position = mul(v[i], LightView);
        output.Position = mul(output.Position, LightProjection);

        output.ShadowTransform = mul(v[i], ShadowTransform);
        
        output.Uv = TexCoord[i];
        output.TextureNumber = input[0].TextureNumber;
        output.InstanceId = input[0].InstanceId;

        stream.Append(output);
    }

}

// --------------------------------------------------------------------- //
//  Pixel Shader
// --------------------------------------------------------------------- //
Texture2DArray BillboardMap;
sampler BillboardSampler
{
    AddressU = WRAP;
    AddressV = WRAP;
    Filter = COMPARISON_ANISOTROPIC;
    MaxAnisotropy = 16;
    MaxLOD = 2.0f;
};

float4 PS_Billboard(GeometryOutput input) : SV_TARGET
{
    //번호 0번 0번그림 = > TextureNumber
    //번호 1번 1번그림

    float3 uvw;
    if (input.TextureNumber == 0)
        uvw = float3(input.Uv, 0);
    else if (input.TextureNumber == 1)
        uvw = float3(input.Uv, 1);
    else
        uvw = float3(input.Uv, 2);

    float4 flower = BillboardMap.Sample(BillboardSampler, uvw);

    float3 toEye = ViewPosition - input.wPosition;
    float distanceToEye = length(toEye);
    
    toEye = normalize(toEye);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    float shadow = float3(1.0f, 1.0f, 1.0f);
    shadow = CalcShadowFactor(samShadow, ShadowMap, input.ShadowTransform);

    shadow = min(0.5f, shadow);

    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { LightAmbient, LightDiffuse, LightSpecular, LightDirection };


    float4 A, D, S;
    float3 normal = float3(0, 1, 0);
    ComputeDirectionalLight(m, l, normal, toEye, A, D, S);
    ambient += A;
    diffuse += D * shadow;
    specular += S * shadow;
    
    float4 color = float4(ambient + diffuse + specular, 1.0f);
    color *= flower;

    clip(color.a - 0.5f);
    float fogFactor = saturate((distanceToEye - FogStart) / FogRange);

    color = lerp(color, SunColor, fogFactor);

    return color;
}

void PS_Depth_Billboard(GeometryOutput input)
{
    float3 uvw;
    if (input.TextureNumber == 0)
        uvw = float3(input.Uv, 0);
    else if (input.TextureNumber == 1)
        uvw = float3(input.Uv, 1);
    else
        uvw = float3(input.Uv, 2);

    float4 color = BillboardMap.Sample(BillboardSampler, uvw);

    clip(color.a - 0.5f);
}
//***********************************************************************//
//***********************************************************************//
//***********************************************************************//


//***********************************************************************//
//****************************Sky****************************************//
//***********************************************************************//

//  상수는 static const로 선언
static const float PI = 3.14159265f;
static const float InnerRadius = 6356.7523142; //  지구 반지름
static const float OuterRadius = 6356.7523142 * 1.0157313; //  대기 반지름


float ESun = 20.0f; //  태양의 밝기
float Kr = 0.0025f; //  Rayleigh
float Km = 0.0010f; //  Mie

float KrESun = 0.0025f * 20.0f;
float KmESun = 0.0010f * 20.0f;
float Kr4PI = 0.0025f * 4.0f * PI;
float Km4PI = 0.0010f * 4.0f * PI;

float Scale = 1.0f / (OuterRadius - InnerRadius); //   비율 
float2 RayleighMieScaleHeight = { 0.25, 0.1 };

float g = -0.990f;
float g2 = -0.990f * -0.990f;
float Exposure = -2.0f; //  밝은 빛을 강조 시키기 위한 상수

float3 WaveLength;
int SampleCount;
float3 InvWaveLength;
float3 WaveLengthMie;
//float4 SunColor;

float MoonAlpha = 0.0f;

float NumTiles = 16.25f;
float NoiseTime = 0.0f;

RasterizerState CullModeOff
{
    CullMode = None;
};

DepthStencilState DepthOff
{
    DepthEnable = false;
};

BlendState BlendOn
{
    BlendEnable[0] = true;
    BlendOp[0] = Add;
    BlendOpAlpha[0] = Add;
    DestBlend[0] = Inv_Src_Alpha;
    DestBlendAlpha[0] = Zero;
    SrcBlend[0] = Src_Alpha;
    SrcBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 1 | 2 | 4 | 8; // Write all colors R | G | B | A
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float3 oPosition : POSITION1;
};


struct PixelCloudInput
{
    float4 Position : SV_POSITION;
    float4 wPosition : Position1;
    float2 Uv : UV0;
    float2 oUv : UV1;
};

float4x4 SkyWorld;
PixelInput VS_Scattering(VertexTexture input)
{
    PixelInput output;
    
    output.Position = mul(input.Position, SkyWorld);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = -input.Position.xyz;
    output.Uv = input.Uv;

    return output;
}

Texture2D RayleighMap;
SamplerState RayleighSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

Texture2D MieMap;
SamplerState MieSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
    AddressW = CLAMP;
};

Texture2D StarfieldMap;
SamplerState StarfieldSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
};

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

float4 PS_Scattering(PixelInput input) : SV_TARGET
{
    float3 sunDirection = -normalize(LightDirection);

    float temp = dot(sunDirection, input.oPosition) / length(input.oPosition);
    float temp2 = temp * temp;

    float3 rSamples = RayleighMap.Sample(RayleighSampler, input.Uv);
    float3 mSamples = MieMap.Sample(MieSampler, input.Uv);

    float3 color = 0;

    color = GetRayleighPhase(temp2) * rSamples + GetMiePhase(temp, temp2) * mSamples;
    //color = GetMiePhase(temp, temp2) * mSamples;
    //color = GetRayleighPhase(temp2) * rSamples;
    color = HDR(color);

    color += max(0, (1 - color)) * float3(0.05f, 0.05f, 0.1f);
    
    float4 starField = saturate(StarfieldMap.Sample(StarfieldSampler, input.Uv) * LightDirection.y);
    return float4(color, 1) + saturate(starField);
}

///////////////////////////////////////////////////////////////////////////////

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

struct SkyPixelTargetOutput
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

SkyPixelTargetOutput PS_Target(PixelTargetInput input) : SV_TARGET
{
    SkyPixelTargetOutput output;
    
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

    float3 sunDir = normalize(LightDirection);
    sunDir.y *= -1.0f;

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

Texture2D MoonMap;
Texture2D MoonGlowMap;

PixelInput VS_Moon(VertexTexture input)
{
    PixelInput output;
    
    output.Position = mul(input.Position, SkyWorld);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = -input.Position.xyz;
    output.Uv = input.Uv;

    return output;
}

float4 PS_Moon(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    color = MoonMap.Sample(TrilinearSampler, input.Uv);
    color.a *= (MoonAlpha * 1.5f);

    return color;
}

float4 PS_MoonGlow(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    color = MoonGlowMap.Sample(TrilinearSampler, input.Uv);
    color.a *= MoonAlpha;

    return color;
}

///////////////////////////////////////////////////////////////////////////////
//cbuffer PS_CloudBuffer : register(b10)
//{
//    float NoiseTime;
//}

float cloudscale = 10.1;
float speed = 0.03;
float clouddark = 0.5;
float cloudlight = 0.3;
float cloudcover = 0.2;
float cloudalpha = 8.0;
float skytint = 0.5;
float3 skycolour1 = float3(0.2, 0.4, 0.6);
float3 skycolour2 = float3(0.4, 0.7, 1.0);

static const float2x2 m = float2x2(1.6, 1.2, -1.2, 1.6);


static const float CloudCover = -0.1;
static const float CloudSharpness = 0.125;


Texture2D CloudMap;
SamplerState CloudSampler;


PixelCloudInput VS_Cloud(VertexTexture input)
{
    PixelCloudInput output;
    
    output.Position = mul(input.Position, SkyWorld);
    output.wPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Uv = (input.Uv * NumTiles);
    output.oUv = input.Uv;

    return output;
}

float2 Hash2(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

float Noise2(in float2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    float2 i = floor(p + (p.x + p.y) * K1);
    float2 a = p - i + (i.x + i.y) * K2;
    float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0); //vec2 of = 0.5 + 0.5*vec2(sign(a.x-a.y), sign(a.y-a.x));
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    float3 n = h * h * h * h * float3(dot(a, Hash2(i + 0.0)), dot(b, Hash2(i + o)), dot(c, Hash2(i + 1.0)));
    return dot(n, float3(70.0, 70.0, 70.0));
}

float Fbm(float2 n)
{
    float total = 0.0, amplitude = 0.1;
    for (int i = 0; i < 7; i++)
    {
        total += Noise2(n) * amplitude;
        n = mul(n, m);
        amplitude *= 0.4;
    }
    return total;
}

float2 iResolution = float2(900, 600);

float4 mainImage(PixelCloudInput input) : SV_Target
{
    float2 uv = input.oUv;
    float time = Time * speed;
    float q = Fbm(uv * cloudscale * 0.5);
    
    //ridged noise shape
    float r = 0.0;
    float f = 0.0;
    uv *= cloudscale;
    uv -= q - time;
    float weightR = 0.8;
    float weightF = 0.7;
    for (int i = 0; i < 7; i++)
    {
        r += abs(weightR * Noise2(uv));
        f += weightF * Noise2(uv);

        uv = mul(uv, m) + time;

        weightR *= 0.7;
        weightF *= 0.6;
    }

    f *= r + f;

    //noise colour
    float c = 0.0;
    float c1 = 0.0;
    time = Time * speed * 2.0;
    uv = input.oUv;
    uv *= cloudscale * 2.0;
    uv -= q - time;
    float weight = 0.4;
    for (int k = 0; k < 7; k++)
    {
        c += weight * Noise2(uv);
        c1 += abs(weight * Noise2(uv));

        uv = mul(uv, m) + time;

        weight *= 0.6;
    }
    c += c1;
    
    float4 skycolour = float4(1, 1, 1, 0);
    float4 cloudcolour = float4(float3(1.1, 1.1, 0.9) * clamp((clouddark + cloudlight * c), 0.0, 1.0), 1.0f);
   
    f = cloudcover + cloudalpha * f * r;
    
    float4 result = lerp(skycolour, clamp(skytint * skycolour + cloudcolour, 0.0, 1.0), clamp(f + c, 0.0, 1.0));
    
    float uvX = abs(0.5f - input.oUv.x) + 0.5f;
    float uvY = abs(0.5f - input.oUv.y) + 0.5f;
    
    float alpha1 = uvX > 0.8 ? lerp(0.0f, result.a, (1 - uvX) / 0.2f) : result.a;
    float alpha2 = uvY > 0.8 ? lerp(0.0f, result.a, (1 - uvY) / 0.2f) : result.a;

    float alpha = alpha1 * alpha2;

    result.a = result.a * alpha1 * alpha2;

    return result * SunColor;
}

static const float ONE = 0.00390625;
static const float ONEHALF = 0.001953125;
// The numbers above are 1/256 and 0.5/256, change accordingly
// if you change the code to use another texture size.

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


float4 PS_Cloud(PixelCloudInput input) : SV_Target
{
    float n = noise(input.Uv + NoiseTime);
    float n2 = noise(input.Uv * 2 + NoiseTime);
    float n3 = noise(input.Uv * 4 + NoiseTime);
    float n4 = noise(input.Uv * 8 + NoiseTime);
   
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


// --------------------------------------------------------------------- //
//  Technique
// --------------------------------------------------------------------- //
RasterizerState FillMode
{
    FillMode = Wireframe;
};

technique11 T0  //  Terrain
{
    pass P0     //  terrain NormalMode
    {
        SetRasterizerState(NULL);

        SetVertexShader(CompileShader(vs_5_0, VS_Terrain()));
        SetHullShader(CompileShader(hs_5_0, HS_Terrain()));
        SetDomainShader(CompileShader(ds_5_0, DS_Terrain()));
        SetPixelShader(CompileShader(ps_5_0, PS_Terrain(true)));
    }

    pass P1     //  terrain FillMode
    {
        SetRasterizerState(FillMode);

        SetVertexShader(CompileShader(vs_5_0, VS_Terrain()));
        SetHullShader(CompileShader(hs_5_0, HS_Terrain()));
        SetDomainShader(CompileShader(ds_5_0, DS_Terrain()));
        SetPixelShader(CompileShader(ps_5_0, PS_Terrain(true)));
    }

    pass P2     //  Billboard  
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Billboard()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Billboard()));
        SetPixelShader(CompileShader(ps_5_0, PS_Billboard()));
    }

    //=============================================================//
    //========================Shadow===============================//
    //=============================================================//

    pass P3     //  Terrain Shadow
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Terrain()));
        SetHullShader(CompileShader(hs_5_0, HS_Depth_Terrain()));
        SetDomainShader(CompileShader(ds_5_0, DS_Depth_Terrain()));
        //SetPixelShader(CompileShader(ps_5_0, PS_Depth_Alpha()));
        SetPixelShader(NULL);

        SetRasterizerState(ShadowDepth);
    }

    pass P4     //  Billboard Shadow
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Billboard()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Depth_Billboard()));
        SetPixelShader(CompileShader(ps_5_0, PS_Depth_Billboard()));
    }
}
