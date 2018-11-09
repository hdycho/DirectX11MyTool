#pragma once

class GSBillboard
{
public:
	GSBillboard(ExecuteValues* values,
		int billBoardCount,
		D3DXVECTOR2 scale,
		D3DXVECTOR2 position);
	~GSBillboard();

	void SetTexture(wstring fileName)
	{
		texture = new Texture(fileName);
	}
	wstring&GetName() { return name; }

	void Update();
	void Render();
	void ImGuiRender();

	//y���� ���� �����ϴ� �Լ��� ����� ������
	//=>�ͷ����� ���̿����缭 �����ϰԸ����
	//�������̹��� �����Ҽ� �ְԸ����
private:
	ExecuteValues* values;
	Shader*shader;
	WorldBuffer*worldBuffer;
	VertexSize*vertices;
	
	ID3D11Buffer*vertexBuffer;
	RasterizerState*rasterizerState[2];

	Texture*texture;
	int vertexCount;
	int beforeCount;

	wstring name;

	D3DXVECTOR2 setPos;
	D3DXVECTOR2 beforePos;

	D3DXVECTOR2 setScale;
	D3DXVECTOR2 beforeScale;
};