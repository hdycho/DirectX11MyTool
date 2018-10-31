#pragma once

class ModelBone;
class ModelMesh;
class Model
{
public:
	Model();
	Model(Model&model);
	~Model();

	UINT MaterialCount() { return materials.size(); }
	vector<Material*>& Materials() { return materials; }
	Material*MaterialByIndex(UINT index) { return materials[index]; }
	Material*MaterialByName(wstring name);

	UINT BoneCount() { return bones.size(); }
	vector<ModelBone*>& Bones() { return bones; }
	ModelBone*BoneByIndex(UINT index) { return bones[index]; }
	ModelBone*BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh*>& Meshes() { return meshes; }
	ModelMesh*MeshByIndex(UINT index) { return meshes[index]; }
	ModelMesh*MeshByName(wstring name);


	void ReadMaterial(wstring folder,wstring file);
	void ReadMesh(wstring folder, wstring file);

	void CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms);
	void CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms,D3DXMATRIX& w);
	

private:
	void BindingBone();
	void BindingMesh();
	void SetBoundingPos();
private:
	class ModelBone*root;

	vector<Material*> materials;
	vector<class ModelMesh*> meshes;
	vector<class ModelBone*> bones;

private:
	class BoneBuffer :public ShaderBuffer
	{
	public:
		BoneBuffer()
			:ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 128; i++)
				D3DXMatrixIdentity(&Data.Bones[i]);
			
			D3DXMatrixIdentity(&Data.Scale);
			D3DXMatrixTranspose(&Data.Scale, &Data.Scale);
			Data.UseBlend = 0;
		}

		void Bones(D3DXMATRIX*m, UINT count)
		{
			memcpy(Data.Bones, m, sizeof(D3DXMATRIX)*count);
			
			//전치행렬로 바꾸는이유
			//쉐이더 원래 9까지는 행우선이었는데
			//10 이후로는 열우선으로 바뀌어서
			//전치행렬로 행렬뒤집어줘야한다
			for (UINT i = 0; i < count; i++)
				D3DXMatrixTranspose(&Data.Bones[i],&Data.Bones[i]);
		}

		void Scale(float x, float y, float z)
		{
			Scale(D3DXVECTOR3(x, y, z));
		}

		void Scale(D3DXVECTOR3 vec)
		{
			D3DXMatrixScaling(&Data.Scale, vec.x, vec.y, vec.z);
			D3DXMatrixTranspose(&Data.Scale, &Data.Scale);
		}

		D3DXVECTOR3 Scale()
		{
			D3DXVECTOR3 scale = { Data.Scale._11,Data.Scale._22 ,Data.Scale._33 };
			return scale;
		}

		D3DXMATRIX ScaleMatrix()
		{
			return Data.Scale;
		}

		void UseBlend(bool val)
		{
			Data.UseBlend = (int)val;
		}

	public:
		struct Struct
		{
			D3DXMATRIX Bones[128];
			D3DXMATRIX Scale;

			int UseBlend;
			float Padding[3];
		}Data;
	};

	BoneBuffer*buffer;
public:
	BoneBuffer*Buffer() { return buffer; }

	D3DXVECTOR2 test;
};

class Models
{
public:
	friend class Model;

public:
	static void Create();
	static void Delete();
private:
	static void LoadMaterial(wstring file, vector<Material*>*materials);
	static void ReadMaterialData(wstring file);
	static void LoadMesh(wstring file, vector<class ModelBone *>* bones, vector<class ModelMesh *>* meshes);
	static void ReadMeshData(wstring file);
private:
	static map<wstring, vector<Material*>>materialMap;
	
	struct MeshData
	{
		vector<class ModelBone *> Bones;
		vector<class ModelMesh *> Meshes;
	};
	static map<wstring, MeshData> meshDataMap;
};