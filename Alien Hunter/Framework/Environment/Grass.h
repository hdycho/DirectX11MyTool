//#pragma once
//#include "Interface\IShadow.h"
//
//class Grass : public IShadow
//{
//	struct GrassDesc;
//	class Wind;
//
//public:
//	Grass(class Terrain * terrain);
//	~Grass();
//
//	void Initialize();
//	void Ready();
//	void Update();
//	void Render();
//	void RenderShadow(UINT tech, UINT pass);
//
//	void CreateVector();
//	void GenerateGrass(D3DXVECTOR3 position, UINT terrainSize = 1024, UINT numberOfPatchRows = 50, UINT numberOfRootsInPatch = 70);
//
//	vector<GrassDesc *>& Grasses() { return grasses; }
//	GrassDesc * GetDesc() { return grassDesc; }
//	UINT& GrassCount() { return grassCount; }
//	Wind * GetWind() { return windField; }
//
//private:
//	//	Wind
//	void InitWind();
//
//	//	Load
//	void LoadData();
//	void BindData();
//
//	//	Generate Grass & Flower
//	int GeneratePatch(GrassDesc * grass, D3DXVECTOR3 startPos, D3DXVECTOR3 patchSize, int currentVertex);
//	void GenerateFlower(GrassDesc * grass);
//
//	//	Draw Grass & Flower
//	void DrawGrass(UINT tech = 0, UINT pass = 0);
//	void DrawFlower(UINT tech = 1, UINT pass = 0);
//private:
//	class Terrain * terrain;
//
//	Material * material;
//
//	UINT grassCount;
//
//	// ====================================================== //
//	//	Grass
//	// ====================================================== //
//	vector<GrassDesc *> grasses;
//
//	vector<VertexTextureNormal> verticesG;
//	ID3D11Buffer * vertexBufferG;
//
//	UINT numOfRootsBefore;
//	UINT numOfRoots;
//	UINT numOfPatches;
//	// ====================================================== //
//	//	Flower
//	// ====================================================== //
//	vector<VertexTextureNormal> verticesF;
//	ID3D11Buffer * vertexBufferF;
//
//	UINT flowerRootsNum;
//	UINT flowerRootsNumBefore;
//private:
//	struct GrassDesc
//	{
//		Material * material;
//
//		wstring DiffuseMap;
//		wstring NormalMap;
//		wstring SpecularMap;
//		wstring DetailMap;
//
//		UINT size;
//		D3DXVECTOR3 position;
//		UINT numOfPatchRows;
//		UINT numOfRootsInPatch;
//		UINT numOfRoots;
//
//		bool isFlower;
//		UINT flowerNum;
//
//		GrassDesc() {}
//		GrassDesc(Material * material)
//		{
//			this->material = material;
//			this->material->SetDiffuseMap(Textures + L"grassBlade.png");
//			this->material->SetNormalMap(Textures + L"grassBladeAlpha.png");
//			this->material->SetSpecularMap(Textures + L"grassBladeAlpha2.png");
//			this->material->SetDetailMap(Textures + L"flower.png");
//
//			size = 0;
//			position = D3DXVECTOR3(0, 0, 0);
//			numOfPatchRows = 10;
//			numOfRootsInPatch = 10;
//			flowerNum = 10;
//			isFlower = false;
//		}
//
//		void ChangeTexture(wstring fileName, UINT type)
//		{
//
//			if (type == 0) this->material->SetDiffuseMap(fileName);
//			else if (type == 1) this->material->SetDetailMap(fileName);
//		}
//
//		void Delete()
//		{
//
//		}
//	} *grassDesc;
//
//private:
//	class Wind
//	{
//	public:
//		Wind() {}
//		Wind(D3DXVECTOR2 accel, D3DXVECTOR2 velocity)
//			: accel(accel), velocity(velocity)
//		{}
//		~Wind() {}
//
//		//void Accel(D3DXVECTOR2 val) { accel = val; }
//		D3DXVECTOR2& Accel() { return accel; }
//
//		//void Velocity(D3DXVECTOR2 val) { velocity = val; }
//		D3DXVECTOR2& Velocity() { return velocity; }
//	private:
//		D3DXVECTOR2 accel;
//		D3DXVECTOR2 velocity;
//	};
//
//	Wind * windField;
//
//	// IShadow을(를) 통해 상속됨
//	virtual void SetShader(D3DXMATRIX v, D3DXMATRIX p) override;
//	virtual void SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv) override;
//	virtual void ShadowUpdate() override;
//	virtual void NormalRender() override;
//	virtual void ShadowRender(UINT tech, UINT pass) override;
//};