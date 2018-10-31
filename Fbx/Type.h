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


	//한 프레임이 가지는 크기,회전,이동 정보
	struct FbxKeyframeData
	{
		float Time;				//해당시간

		D3DXVECTOR3 Scale;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Translation;
	};

	//한 본(뼈)이 가지는 에니메이션 정보
	struct FbxKeyFrame
	{
		string BoneName;
		int index;
		int parent;
		vector<FbxKeyframeData> Transforms;
	};

	//한 에니메이션의 정보
	struct FbxClip
	{
		string Name;

		int FrameCount;		//프레임 갯수
		float FrameRate;	//속도	
		float Duration;		//시작과 끝의 간격

		//한 에니메이션의 본 정보들
		vector<FbxKeyFrame*> KeyFrames; 
	};


	//한점이 영향을 받는 가중치 값
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

	//한점이 가지는 가중치값
	struct FbxBoneWeights
	{
	private:
		//   pair<본 인덱스, 가중치 값>
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
				//가중치 값이 기존가중치값보다 크면 새로삽입
				if (boneWeights > it->second)
				{
					BoneWeights.insert(it, Pair(boneIndex, boneWeights));
					bAdd = true;

					break;
				}

				it++;
			}//while(it)

			//가중치값이 하나도없을때
			if (bAdd == false)
				BoneWeights.push_back(Pair(boneIndex, boneWeights));
		}

		//가중치 복사
		void GetBlendWeights(FbxBlendWeight& blendWeights)
		{
			for (UINT i = 0; i < BoneWeights.size(); i++)
			{
				if (i >= 4) return;

				//가중치값을 보낸다
				blendWeights.Set(i, BoneWeights[i].first, BoneWeights[i].second);
			}
		}

		//가중치값들의 평균을 낸다.
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