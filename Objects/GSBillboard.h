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

	//y축을 따로 설정하는 함수도 만들면 좋을듯
	//=>터레인의 높이에맞춰서 설정하게만들기
	//빌보드이미지 셋팅할수 있게만들기
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