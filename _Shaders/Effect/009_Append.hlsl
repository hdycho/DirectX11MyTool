struct Data
{
    int Id;
    int Data;
    int Sum;
};

ConsumeStructuredBuffer<int> Input;
AppendStructuredBuffer<Data> Output;


// numthreads �� x*y*z�� 768�� ������ȵ�
//����ġ ��������̵�
[numthreads(64,1,1)]
void CS(int3 id:SV_GroupThreadId,int uid:SV_DispatchThreadId)
{
    int data = Input.Consume();
    
    Data result;
    result.Id = id;
    result.Data = data;
    result.Sum = id + data;

    Output.Append(result);
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