#pragma once

//�ִ� ����� �� �߰��Ͽ� ����
//���ϰ� ����ϴ� Ŭ����
//����Ŭ����

//�����Ͷ������ �������� ��ȯ���� ������ VSó�� ������ 
//PSó�� �������� ó��
class RasterizerState
{
public:
	RasterizerState();
	~RasterizerState();

	void RSSetState();

	//��Ƽ��������� ���� ������
	void AntialiasedLineEnable(bool val);
	
	//�ø��� ���� �ø��� �ð�������� ���� �ݽð�������� ����
	//�ø��� ���ؽ����̴� ó���ٷ� ������ �Ͼ(���ؽ�)
	void CullMode(D3D11_CULL_MODE val);
	
	//Ŭ������ �ȼ����̴� ó���ٷ� ������ �Ͼ(�ȼ�)

	void DepthClipEnable(bool val);
	
	void FillMode(D3D11_FILL_MODE val);
	
	//�׸� ������ ���ϴ°�=>�ð� ����,�ݽð� ����
	void FrontCounterClockwise(bool val);
	
	//ȭ�鿡 ���̴� ����Ƽ�� ���ϼ������� �ӵ�������
	void MultisampleEnable(bool val);
private:
	void Changed();

private:
	D3D11_RASTERIZER_DESC desc;
	ID3D11RasterizerState* state;
};
