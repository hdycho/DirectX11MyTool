#include "000_EffectHeader.hlsl"


float2 WindVector;
float TextureCount;

// Our constants
static const float kHalfPi = 1.5707;
static const float kQuarterPi = 0.7853;
static const float kOscillateDelta = 0.5;
static const float kWindCoeff = 50.0f;

float3 HUEtoRGB(in float H)
{
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return saturate(float3(R, G, B));
}

float3 HSVtoRGB(in float3 HSV)
{
    float3 RGB = HUEtoRGB(HSV.x);
    return ((RGB - 1) * HSV.y + 1) * HSV.z;
}


// --------------------------------------------------------------------- //
//  Vertex Shader
// --------------------------------------------------------------------- //
struct VertexOutput
{
    float4 Center : POSITION0;
    float2 Size : SIZE0;
   // uint PrimitiveID : SV_PrimitiveID;
};

VertexOutput VS(VertexSize input)
{
    VertexOutput output;

    output.Center = input.Position;
    output.Size = input.Size;
    
    return output;
}

// --------------------------------------------------------------------- //
//  Geometry Shader
// --------------------------------------------------------------------- //
struct GeometryOutput
{
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION1;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    uint PrimitiveID : SV_PrimitiveID;
    float3 VertexToLight : POSITION2;
    float3 VertexToCamera : POSITION3;
    float Random : NORMAL1;
};

static const float2 TexCoord[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};

[maxvertexcount(4)] //  최대 넘겨 보낼 버텍스 개수
void GS(point VertexOutput input[1], uint PrimitiveID : SV_PrimitiveID, inout TriangleStream<GeometryOutput> stream)
{
    float4 root = input[0].Center;

    // Generate a random number between 0.0 to 1.0 by using the root position (which is randomized by the CPU)
    float random = sin(kHalfPi * frac(root.x) + kHalfPi * frac(root.z));
    float randomRotation = random;
    float cameraDistance = length(ViewPosition.xz - root.xz);

    // Properties of the grass blade
    float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;
    
	// Animation
    float toTheLeft = sin(Time);

    // Rotate in Z-axis
    float3x3 rotationMatrix =
    {
        cos(randomRotation), 0, sin(randomRotation),
		0, 1, 0,
		-sin(randomRotation), 0, cos(randomRotation)
    };


    // Generating vertices
    float3 up = float3(0, 1, 0);
    float3 look = ViewPosition - input[0].Center.xyz;
    look.y = 0.0f;
    look = normalize(look);

    float3 right = cross(up, look);
    //  4 정점 계산
    float4 v[4];
    v[0] = float4(input[0].Center.xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center.xyz + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center.xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center.xyz - halfWidth * right + halfHeight * up, 1.0f);

    // Wind
    float windCoEff = 0;
    float steepnessFactor = 1.75;


    GeometryOutput output;
    [unroll]
    for (int i = 0; i < 4; ++i) //  ++i 가 i++ 보다 빠름
    {
        //Wind 
        float2 windVec = WindVector;
        windVec.x += (sin(Time + root.x / 25) + sin((Time + root.x / 15) + 50)) * 0.5;
        windVec.y += cos(Time + root.z / 80);
        windVec *= lerp(0.7, 1.0, 1.0 - random);

        // Oscillate wind
        float sinSkewCoeff = random;
        float oscillationStrength = 2.5f;
        float lerpCoeff = (sin(oscillationStrength * Time + sinSkewCoeff) + 1.0) / 2;
        float2 leftWindBound = windVec * (1.0 - kOscillateDelta);
        float2 rightWindBound = windVec * (1.0 + kOscillateDelta);
        windVec = lerp(leftWindBound, rightWindBound, lerpCoeff);

        // Randomize wind by adding a random wind vector
        float randAngle = lerp(-3.14, 3.14, random);
        float randMagnitude = lerp(0, 1.0, random);
        float2 randWindDir = float2(sin(randAngle), cos(randAngle));
        windVec += randWindDir * randMagnitude;

        float windForce = length(windVec);

        float currentV = 1;
        float VOffset = 1;
        float currentNormalY = 0;
        float currentHeightOffset = sqrt(input[0].Size.y);
        float currentVertexHeight = 0;

        if (i % 2 != 0)
        {
			// General
            currentV -= VOffset;
            currentNormalY += VOffset * 2;

			// Height
            currentHeightOffset -= VOffset;
            float currentHeight = input[0].Size.y - (currentHeightOffset * currentHeightOffset);
            currentVertexHeight = currentHeight;

			// Wind
            windCoEff += VOffset; // TODO: Check these values
        }

        //Calculate final vertex position based on wind
        v[i].xz += windVec * windCoEff;
        //v[i].y -= windForce * windCoEff*0.8f;
        v[i].y += halfHeight * 1.3f;

        output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);
        output.wPosition = v[i].xyz;

        output.Normal = normalize(float4(0, pow(currentNormalY, steepnessFactor), 0, 1));
        output.Uv = TexCoord[i];
        output.PrimitiveID = PrimitiveID;
        output.VertexToLight = normalize(LightPosition - output.wPosition);
        output.VertexToCamera = normalize(ViewPosition - output.wPosition);
        output.Random = random;

        stream.Append(output);
    }

}

// --------------------------------------------------------------------- //
//  Pixel Shader
// --------------------------------------------------------------------- //
Texture2DArray Map;
SamplerState Sampler;

float4 PS(GeometryOutput input) : SV_TARGET
{
    float3 uvw = float3(input.Uv, input.PrimitiveID % TextureCount);
    float4 color = Map.Sample(Sampler, uvw);

    // Phong
    float3 r = normalize(reflect(input.VertexToLight, input.Normal));
    float shininess = 100;

    float ambientLight = 0.1;
    float diffuseLight = saturate(dot(input.VertexToLight, input.Normal));
    float specularLight = saturate(dot(-input.VertexToCamera, r));
    specularLight = saturate(pow(specularLight, shininess));
	
    float light = ambientLight + (diffuseLight * 1.55) + (specularLight * 0.5);
	
    float3 grassColorHSV = { 0.17 + (input.Random / 20), 1, 1 };
    float3 grassColorRGB = HSVtoRGB(grassColorHSV);

    clip(color.a - 0.5f);

    return /* light * */color * float4(grassColorRGB, 1);
    //return float4(sin(LightPosition.y), sin(LightPosition.y), sin(LightPosition.y), 1);
}

// --------------------------------------------------------------------- //
//  Techniques
// --------------------------------------------------------------------- //
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}