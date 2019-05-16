#pragma once

class ModelBone;
class ModelMesh;

class Model
{
public:
	Model();
	~Model();

	UINT MaterialCount() { return materials.size(); }
	vector<Material *>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name);

	UINT BoneCount() { return bones.size(); }
	vector<ModelBone *>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh *>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT index) { return meshes[index]; }
	ModelMesh* MeshByName(wstring name);

	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);

	// �⺻���� ��ﶩ �̳༮ �ᵵ ��
	void CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms);
	// �̵���ų�� �̳༮ ȣ��
	void CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms, D3DXMATRIX& w);

	bool FirstNameModel() { return isFirstName; }
private:
	// �θ� �ڽ� ���� ���� ���� �Լ�
	void BindingBone();
	void BindingMesh();

private:
	class ModelBone* root;

	vector<Material *> materials;
	vector<class ModelMesh *> meshes;
	vector<class ModelBone *> bones;

	D3DXVECTOR3 min, max;
	vector<D3DXVECTOR3> pickCube;

	wstring modelName;	//	�������̸�(�ν��Ͻ̶��ʿ�)

	bool isFirstName; // ���߿� ������ ���������ʴ� ��

public:
	wstring GetModelName() { return modelName; } // �������̸�(�ν��Ͻ̶��ʿ�)
	D3DXVECTOR3&Min() { return min; }
	D3DXVECTOR3&Max() { return max; }
	vector<D3DXVECTOR3>&PickCube() { return pickCube; }
};

// �� �ѹ� �ҷ����� �ٽ� �Ⱥҷ����� �ϱ� ���� Ŭ����
class Models
{
public:
	friend class Model;

public:
	static void Create();
	static void Delete();

private:
	static void LoadMaterial(wstring file, vector<Material *>* materials);
	static void ReadMaterialData(wstring file);

	static bool LoadMesh(wstring file, vector<class ModelBone *>* bones, vector<class ModelMesh *>* meshes);
	static bool ReadMeshData(wstring file);

private:
	static map<wstring, vector<Material *>> materialMap;

	struct MeshData
	{
		vector<class ModelBone *> Bones;
		vector<class ModelMesh *> Meshes;
	};
	static map<wstring, MeshData> meshDataMap;
	static map<wstring, bool> firstNameMap;
};