#pragma once
#include "stdafx.h"

namespace Fbx
{
	struct FbxMaterial
	{
		string Name;
		//Diffuse 
		D3DXCOLOR Diffuse;
		string DiffuseFile;

		//Specular
		string SpecularFile;

		//Normalmap
		string NormalMapFile;
	};

	struct FbxBoneData
	{
		int Index;
		string Name;

		int Parent;
		
		D3DXMATRIX LocalTransform; 
		D3DXMATRIX GlobalTransform;
	};

	struct FbxVertex
	{
		int ControlPoint;
		string MaterialName;
		
		ModelVertexType Vertex;
	};

	struct FbxMeshPartData
	{
		string MaterialName;

		vector<ModelVertexType> Vertices;
		vector<UINT> Indices;
	};

	struct FbxMeshData
	{
		string Name;
		int ParentBone;

		FbxMesh*Mesh;

		vector<FbxVertex*> Vertices;
		vector<FbxMeshPartData*> MeshParts;
	};


	//�� �������� ������ ũ��,ȸ��,�̵� ����
	struct FbxKeyframeData
	{
		float Time;				//�ش�ð�

		D3DXVECTOR3 Scale;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Translation;
	};

	//�� ��(��)�� ������ ���ϸ��̼� ����
	struct FbxKeyFrame
	{
		string BoneName;
		int index;
		int parent;
		vector<FbxKeyframeData> Transforms;
	};

	//�� ���ϸ��̼��� ����
	struct FbxClip
	{
		string Name;

		int FrameCount;		//������ ����
		float FrameRate;	//�ӵ�	
		float Duration;		//���۰� ���� ����

		//�� ���ϸ��̼��� �� ������
		vector<FbxKeyFrame*> KeyFrames; 
	};


	//������ ������ �޴� ����ġ ��
	struct FbxBlendWeight
	{
		D3DXVECTOR4 Indices = D3DXVECTOR4(0, 0, 0, 0);
		D3DXVECTOR4 Weights = D3DXVECTOR4(0, 0, 0, 0);

		void Set(UINT index, UINT boneIndex, float weight)
		{
			float i = (float)boneIndex;
			float w = weight;

			switch (index)
			{
			case 0: Indices.x = i; Weights.x = w; break;
			case 1: Indices.y = i; Weights.y = w; break;
			case 2: Indices.z = i; Weights.z = w; break;
			case 3: Indices.w = i; Weights.w = w; break;
			}
		}
	};

	//������ ������ ����ġ��
	struct FbxBoneWeights
	{
	private:
		//   pair<�� �ε���, ����ġ ��>
		typedef pair<int, float> Pair;
		vector<Pair> BoneWeights;

	public:
		void AddWeights(UINT boneIndex, float boneWeights)
		{
			if (boneWeights <= 0.0f) return;

			bool bAdd = false;
			vector<Pair>::iterator it = BoneWeights.begin();
			
			while (it != BoneWeights.end())
			{
				//����ġ ���� ��������ġ������ ũ�� ���λ���
				if (boneWeights > it->second)
				{
					BoneWeights.insert(it, Pair(boneIndex, boneWeights));
					bAdd = true;

					break;
				}

				it++;
			}//while(it)

			//����ġ���� �ϳ���������
			if (bAdd == false)
				BoneWeights.push_back(Pair(boneIndex, boneWeights));
		}

		//����ġ ����
		void GetBlendWeights(FbxBlendWeight& blendWeights)
		{
			for (UINT i = 0; i < BoneWeights.size(); i++)
			{
				if (i >= 4) return;

				//����ġ���� ������
				blendWeights.Set(i, BoneWeights[i].first, BoneWeights[i].second);
			}
		}

		//����ġ������ ����� ����.
		void Normalize()
		{
			float totalWeight = 0.0f;

			int i = 0;
			vector<Pair>::iterator it = BoneWeights.begin();
			while (it != BoneWeights.end())
			{
				if (i < 4)
				{
					totalWeight += it->second;
					i++, it++;
				}
				else
					it = BoneWeights.erase(it);
			}

			float scale = 1.0f / totalWeight;

			it = BoneWeights.begin();
			while (it != BoneWeights.end())
			{
				it->second *= scale;
				it++;
			}
		}
	};
}