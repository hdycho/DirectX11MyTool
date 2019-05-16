struct Data
{
    float3 V1;
    float2 V2;
};

StructuredBuffer<Data> Input1;
StructuredBuffer<Data> Input2;

//RW => ReadWrite전부가능
RWStructuredBuffer<Data> Output;

[numthreads(64,1,1)]
void CS(int3 id:SV_GroupThreadId)
{
    Output[id.x].V1 = Input1[id.x].V1 + Input2[id.x].V1;
    Output[id.x].V2 = Input1[id.x].V2 + Input2[id.x].V2;
    
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}