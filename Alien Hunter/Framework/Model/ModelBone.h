#pragma once

class ModelBone
{
public:
	// 반대로 해도됨 근데 보통은 열어줄 곳에다 쓰심
	friend class Model;
	friend class Models;

private:
	ModelBone();
	~ModelBone();

public:
	int Index() { return index; }

	int ParentIndex() { return parentIndex; }
	ModelBone* Parent() { return parent; }

	UINT ChildCount() { return childs.size(); }
	ModelBone* child(UINT index) { return childs[index]; }
	vector<ModelBone *>& GetChilds() { return childs; }

	wstring Name() { return name; }

	D3DXMATRIX& Local() { return local; }
	void Local(D3DXMATRIX& matrix) { this->local = matrix; }

	D3DXMATRIX& Global() { return global; }
	void Global(D3DXMATRIX& matrix) { this->global = matrix; }

public:
	void Clone(void **clone);

private:
	int index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	vector<ModelBone *> childs;

	D3DXMATRIX local;
	D3DXMATRIX global;
};