#include "stdafx.h"
#include "ColliderBox.h"
#include "./Model/ModelMeshPart.h"
#include "./Model/Model.h"
#include "./Objects/GameModel.h"
#include "./Utilities/Xml.h"


ColliderBox::ColliderBox(D3DXMATRIX modelMat, COLLIDERSTATE cState, bool isShowCollider, wstring name)
	:Collider(modelMat, isShowCollider, name)
{
	this->cState = cState;
	//ColliderLineInit();

	setBox.resize(8);
	boxInfo.resize(2);
	resultBox.resize(8);
}

ColliderBox::~ColliderBox()
{
	for (int i = 0; i < 12; i++)
		SAFE_DELETE_ARRAY(ColliderLine);
}

void ColliderBox::RenewBox(D3DXMATRIX modelWorld)
{
	D3DXVECTOR3 minpos, maxpos;

	minpos = { 0,0,0 };
	maxpos = { 0,0,0 };

	D3DXMATRIX result = modelWorld;

	D3DXVec3TransformCoord(&minpos, &minpos, &result);
	D3DXVec3TransformCoord(&maxpos, &maxpos, &result);

	minMaxPos.clear();
	for (int i = 0; i < 8; i++)
	{
		D3DXVec3TransformCoord(&resultBox[i], &setBox[i], &result);
		//최대최소점 바꿔야함
		if (minpos.x > resultBox[i].x)
			minpos.x = resultBox[i].x;
		if (maxpos.x < resultBox[i].x)
			maxpos.x = resultBox[i].x;

		if (minpos.y > resultBox[i].y)
			minpos.y = resultBox[i].y;
		if (maxpos.y < resultBox[i].y)
			maxpos.y = resultBox[i].y;

		if (minpos.z > resultBox[i].z)
			minpos.z = resultBox[i].z;
		if (maxpos.y < resultBox[i].z)
			maxpos.y = resultBox[i].z;
	}

	minMaxPos.resize(2);
	minMaxPos[0] = boxInfo[0].pos=minpos;
	minMaxPos[1] = boxInfo[1].pos=maxpos;


}

void ColliderBox::ColliderLineInit()
{
	ColliderLine = new LineMaker[12];

	for (int i = 0; i < 12; i++)
	{
		ColliderLine[i].SizeInit(0);
	}


	ColliderLine[0].UpdateLine((*boundingBox)[0], D3DXVECTOR3(1, 0, 0), distance.x);
	ColliderLine[1].UpdateLine((*boundingBox)[0], D3DXVECTOR3(0, 1, 0), distance.y);
	ColliderLine[2].UpdateLine((*boundingBox)[0], D3DXVECTOR3(0, 0, 1), distance.z);

	ColliderLine[3].UpdateLine(D3DXVECTOR3((*boundingBox)[0].x, (*boundingBox)[0].y + distance.y, (*boundingBox)[0].z), D3DXVECTOR3(1, 0, 0), distance.x);
	ColliderLine[4].UpdateLine(D3DXVECTOR3((*boundingBox)[0].x + distance.x, (*boundingBox)[0].y, (*boundingBox)[0].z), D3DXVECTOR3(0, 1, 0), distance.y);
	ColliderLine[5].UpdateLine(D3DXVECTOR3((*boundingBox)[0].x + distance.x, (*boundingBox)[0].y, (*boundingBox)[0].z), D3DXVECTOR3(0, 0, 1), distance.z);

	ColliderLine[6].UpdateLine((*boundingBox)[1], D3DXVECTOR3(-1, 0, 0), distance.x);
	ColliderLine[7].UpdateLine((*boundingBox)[1], D3DXVECTOR3(0, -1, 0), distance.y);
	ColliderLine[8].UpdateLine((*boundingBox)[1], D3DXVECTOR3(0, 0, -1), distance.z);

	ColliderLine[9].UpdateLine(D3DXVECTOR3((*boundingBox)[0].x, (*boundingBox)[0].y, (*boundingBox)[0].z + distance.z), D3DXVECTOR3(1, 0, 0), distance.x);
	ColliderLine[10].UpdateLine(D3DXVECTOR3((*boundingBox)[0].x, (*boundingBox)[0].y, (*boundingBox)[0].z + distance.z), D3DXVECTOR3(0, 1, 0), distance.y);
	ColliderLine[11].UpdateLine(D3DXVECTOR3((*boundingBox)[0].x, (*boundingBox)[0].y + distance.y, (*boundingBox)[0].z + distance.z), D3DXVECTOR3(0, 0, -1), distance.z);
}

void ColliderBox::Update(D3DXMATRIX modelWorld)
{
	if (model != NULL)
	{
		D3DXMATRIX result = model->GetBoneMatrix(parentObjName);
		RenewBox(colliderWorld*result);
		switch (cState)
		{
			case BOX:
			{
				for (int i = 0; i < 12; i++)
				{
					ColliderLine[i].World(colliderWorld*result);
					ColliderLine[i].SetColor(boundingBoxColor);
				}
			}
			break;
		}
	}
	else
	{
		RenewBox(colliderWorld*modelWorld);
		switch (cState)
		{
			case BOX:
			{
				for (int i = 0; i < 12; i++)
				{
					ColliderLine[i].World(colliderWorld*modelWorld);
					ColliderLine[i].SetColor(boundingBoxColor);
				}
			}
			break;
		}
	}
}

void ColliderBox::Render()
{
	if (isShowCollider)
	{
		for (int i = 0; i < 12; i++)
			ColliderLine[i].DrawLine();
	}
}

void ColliderBox::ImguiRender()
{
	switch (cState)
	{
		case BOX:
		{
			__super::Render();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[ParentObject]");
			ImGui::Text("%s", String::ToString(parentObjName).c_str());
			ImGui::Separator();

			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[MinMaxPos]");
			ImGui::Text("MinPos(%.1f,%.1f,%.1f)", boxInfo[0].pos.x, boxInfo[0].pos.y, boxInfo[0].pos.z);
			ImGui::Text("MaxPos(%.1f,%.1f,%.1f)", boxInfo[1].pos.x, boxInfo[1].pos.y, boxInfo[1].pos.z);
			
			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Collider]");
			if (ImGui::Button("Show Collider"))
			{
				if (!isShowCollider)isShowCollider = true;
				else isShowCollider = false;
			}
			ImGui::Text("Collider Size");
			ImGui::DragFloat3("CS", colliderSize, 0.05f, -50, 50);

			{
				D3DXMATRIX boxScale;
				D3DXMatrixIdentity(&boxScale);
				D3DXMatrixScaling(&boxScale, colliderSize.x, colliderSize.y, colliderSize.z);
				for (int i = 0; i < setBox.size(); i++)
					D3DXVec3TransformCoord(&setBox[i], &(*initPos)[i], &boxScale);

				ColliderLine[0].UpdateLine(setBox[0], setBox[1]);
				ColliderLine[1].UpdateLine(setBox[1], setBox[3]);
				ColliderLine[2].UpdateLine(setBox[2], setBox[3]);
				ColliderLine[3].UpdateLine(setBox[2], setBox[0]);

				ColliderLine[4].UpdateLine(setBox[4], setBox[5]);
				ColliderLine[5].UpdateLine(setBox[5], setBox[7]);
				ColliderLine[6].UpdateLine(setBox[6], setBox[7]);
				ColliderLine[7].UpdateLine(setBox[6], setBox[4]);

				ColliderLine[8].UpdateLine(setBox[0], setBox[4]);
				ColliderLine[9].UpdateLine(setBox[1], setBox[5]);
				ColliderLine[10].UpdateLine(setBox[2], setBox[6]);
				ColliderLine[11].UpdateLine(setBox[3], setBox[7]);
			}

			ImGui::Separator();
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetSRT]");
			wstring textTrans = L"[" +GetColliderName() + L"]translate";
			ImGui::Text(String::ToString(textTrans).c_str());
			ImGui::DragFloat3((String::ToString(GetColliderName()) + "T").c_str(), (float*)&cPosition, 0.05f, -128, 128);
			UpdateColliderWorld();

			wstring textScale = L"[" + GetColliderName() + L"]scale";
			ImGui::Text(String::ToString(textScale).c_str());
			ImGui::DragFloat3((String::ToString(GetColliderName()) + "S").c_str(), (float*)&cScale, 0.05f, -10, 10);
			UpdateColliderWorld();

			wstring textRotate = L"[" + GetColliderName() + L"]rotate";
			ImGui::Text(String::ToString(textRotate).c_str());
			ImGui::DragFloat3((String::ToString(GetColliderName()) + "R").c_str(), (float*)&cRotate, 0.05f, -3.14f, 3.14f);
			UpdateColliderWorld();

		}
		break;
	}
}




