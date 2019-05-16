struct Data
{
    float3 V1;
    float2 V2;
};

StructuredBuffer<Data> Input1;
StructuredBuffer<Data> Input2;

//RW => ReadWrite���ΰ���
RWStructuredBuffer<Data> Output;

// numthreads �� x*y*z�� 768�� ������ȵ�
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