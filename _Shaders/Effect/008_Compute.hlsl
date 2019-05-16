struct Data
{
    float3 V1;
    float2 V2;
};

StructuredBuffer<Data> Input1;
StructuredBuffer<Data> Input2;

//RW => ReadWrite전부가능
RWStructuredBuffer<Data> Output;

// numthreads 에 x*y*z이 768을 넘으면안됨
[numthreads(128,1,1)]
void CS(int3 id : SV_GroupThreadId,int uid : SV_DispatchThreadId)
{
    Output[uid].V1 = Input1[uid].V1 + Input2[uid].V1;
    Output[uid].V2 = Input1[uid].V2 + Input2[uid].V2;
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