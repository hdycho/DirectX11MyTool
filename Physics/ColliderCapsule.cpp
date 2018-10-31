#include "ColliderCapsule.h"
#include "ColliderBox.h"
#include "./Model/ModelMeshPart.h"
#include "./Model/Model.h"
#include "./Objects/GameModel.h"
#include "./Utilities/Xml.h"

ColliderCapsule::ColliderCapsule(D3DXMATRIX modelMat, COLLIDERSTATE cState, bool isShowCollider, wstring name)
	:Collider(modelMat, isShowCollider, name)
{
	Pos.heightPos.resize(2);

	CircleCount = 36;
	totalCount = 152;
	this->cState = cState;
	D3DXMatrixIdentity(&curMat);

}

ColliderCapsule::~ColliderCapsule()
{
}

void ColliderCapsule::Update(D3DXMATRIX modelWorld)
{
	if (model != NULL)
	{
		D3DXMATRIX result = model->GetBoneMatrix(parentObjName);
		RenewCapsule(colliderWorld*result);

		//월드행렬의 분해가 필요하다. 크기만 빼고넣기위하여
		D3DXVECTOR3 scale, trans;
		D3DXQUATERNION rotate;
		D3DXMatrixDecompose(&scale, &rotate, &trans, &modelWorld);

		D3DXMATRIX T, R, decompose;
		D3DXMatrixIdentity(&T);
		D3DXMatrixIdentity(&R);

		D3DXMatrixTranslation(&T, trans.x, trans.y, trans.z);
		D3DXMatrixRotationQuaternion(&R, &rotate);

		decompose = R*T;
		for (int i = 0; i < totalCount; i++)
		{
			ColliderLine[i].World(colliderWorld*result);
			ColliderLine[i].SetColor(boundingBoxColor);
		}
	}
	else
	{
		RenewCapsule(colliderWorld*modelWorld);

		//월드행렬의 분해가 필요하다. 크기만 빼고넣기위하여
		D3DXVECTOR3 scale, trans;
		D3DXQUATERNION rotate;
		D3DXMatrixDecompose(&scale, &rotate, &trans, &modelWorld);

		D3DXMATRIX T, R, result;
		D3DXMatrixIdentity(&T);
		D3DXMatrixIdentity(&R);

		D3DXMatrixTranslation(&T, trans.x, trans.y, trans.z);
		D3DXMatrixRotationQuaternion(&R, &rotate);

		result = R*T;
		for (int i = 0; i < totalCount; i++)
		{
			ColliderLine[i].World(colliderWorld*result);
			ColliderLine[i].SetColor(boundingBoxColor);
		}
	}
}

void ColliderCapsule::Render()
{
	if (isShowCollider)
	{
		for (int i = 0; i < totalCount; i++)
			ColliderLine[i].DrawLine();
	}
}

void ColliderCapsule::ImguiRender()
{
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[ParentObject]");
	ImGui::Text("%s", String::ToString(parentObjName).c_str());
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[HeightPos]");
	ImGui::Text("Up(%.2f,%.2f,%.2f)", capsuleInfo[0].heightPos[0].x, capsuleInfo[0].heightPos[0].y, capsuleInfo[0].heightPos[0].z);
	ImGui::Text("Down(%.2f,%.2f,%.2f)", capsuleInfo[0].heightPos[1].x, capsuleInfo[0].heightPos[1].y, capsuleInfo[0].heightPos[1].z);
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Height]");
	ImGui::Text("Height(%d)", capsuleInfo[0].height);
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Radius]");
	ImGui::Text("Radius(%.2f)", capsuleInfo[0].radius);
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Collider]");
	if (ImGui::Button("Show Collider"))
	{
		if (!isShowCollider)isShowCollider = true;
		else isShowCollider = false;
	}

	ImGui::DragInt("height", &Pos.height, 0.1f, 0, 200);
	ImGui::DragFloat("radius", &Pos.radius, 0.1f, 0, 50);
	{
		vector<D3DXVECTOR3> circle;
		verticalLines.clear();
		DrawCircle(circle, CircleCount, 0, CircleCount, 0);
		DrawCircle(circle, CircleCount, CircleCount, CircleCount * 2, 3);

		DrawCircle(circle, CircleCount / 2 + 1, CircleCount * 2, CircleCount * 2 + 19, 1);
		DrawCircle(circle, CircleCount / 2 + 1, CircleCount * 2 + 19, CircleCount * 2 + 38, 2);
		DrawCircle(circle, CircleCount / 2 + 1, CircleCount * 2 + 38, CircleCount * 2 + 57, 4);
		DrawCircle(circle, CircleCount / 2 + 1, CircleCount * 2 + 57, CircleCount * 2 + 76, 5);

		//반직선4개
		ColliderLine[CircleCount * 2 + 76].UpdateLine(verticalLines[0], verticalLines[4]);
		ColliderLine[CircleCount * 2 + 77].UpdateLine(verticalLines[1], verticalLines[5]);
		ColliderLine[CircleCount * 2 + 78].UpdateLine(verticalLines[2], verticalLines[6]);
		ColliderLine[CircleCount * 2 + 79].UpdateLine(verticalLines[3], verticalLines[7]);
	}

	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetSRT]");
	wstring textTrans = L"[" + GetColliderName() + L"]translate";
	ImGui::Text(String::ToString(textTrans).c_str());
	ImGui::DragFloat3((String::ToString(GetColliderName()) + "T").c_str(), (float*)&cPosition, 0.05f, -128, 128);
	UpdateColliderWorld();

	wstring textRotate = L"[" + GetColliderName() + L"]rotate";
	ImGui::Text(String::ToString(textRotate).c_str());
	ImGui::DragFloat3((String::ToString(GetColliderName()) + "R").c_str(), (float*)&cRotate, 0.05f, -3.14f, 3.14f);
	UpdateColliderWorld();
}

void ColliderCapsule::ColliderLineInit()
{
	ColliderLine = new LineMaker[totalCount];

	for (int i = 0; i < totalCount; i++)
	{
		ColliderLine[i].SizeInit(0);
	}


	float centerX = ((*colliderInfo)[0].pos.x + (*colliderInfo)[1].pos.x) / 2.0f;
	float centerY = ((*colliderInfo)[0].pos.y + (*colliderInfo)[1].pos.y) / 2.0f;
	float centerZ = ((*colliderInfo)[0].pos.z + (*colliderInfo)[1].pos.z) / 2.0f;
	
	Pos.pos = { centerX,centerY,centerZ };
	Pos.radius = (*colliderInfo)[1].pos.x - (*colliderInfo)[0].pos.x;
	Pos.height = ((*colliderInfo)[1].pos.x - (*colliderInfo)[0].pos.x) *4;
	//윗점
	Pos.heightPos[0] = { centerX,centerY+(float)Pos.height/2.0f,centerZ };
	//아랫점
	Pos.heightPos[1] = { centerX,centerY - (float)Pos.height / 2.0f,centerZ };

	vector<D3DXVECTOR3> circle;
	DrawCircle(circle, CircleCount,0,CircleCount,0); // 위쪽 XZ원
	DrawCircle(circle, CircleCount, CircleCount, CircleCount*2, 3); // 아래쪽 XZ원

	DrawCircle(circle, CircleCount/2+1, CircleCount * 2, CircleCount * 2 +19,1); // 위쪽 XY원
	DrawCircle(circle, CircleCount/2+1, CircleCount * 2 + 19, CircleCount * 2 + 38, 2); // 위쪽 ZY원
	DrawCircle(circle, CircleCount / 2 + 1, CircleCount * 2 + 38 ,CircleCount * 2 + 57, 4); //아래쪽XY
	DrawCircle(circle, CircleCount / 2 + 1, CircleCount * 2 + 57, CircleCount * 2 + 76, 5); // 아래쪽ZY

	//반직선4개
	ColliderLine[CircleCount * 2 + 76].UpdateLine(verticalLines[0], verticalLines[4]);
	ColliderLine[CircleCount * 2 + 77].UpdateLine(verticalLines[1], verticalLines[5]);
	ColliderLine[CircleCount * 2 + 78].UpdateLine(verticalLines[2], verticalLines[6]);
	ColliderLine[CircleCount * 2 + 79].UpdateLine(verticalLines[3], verticalLines[7]);
	
}

void ColliderCapsule::RenewCapsule(D3DXMATRIX modelWorld)
{
	ColliderInfo centerPos;
	capsuleInfo.clear();
	
	centerPos.heightPos.resize(2);

	//윗점
	Pos.heightPos[0] = { Pos.pos.x,Pos.pos.y + (float)Pos.height / 2.0f,Pos.pos.z };
	//아랫점
	Pos.heightPos[1] = { Pos.pos.x,Pos.pos.y - (float)Pos.height / 2.0f,Pos.pos.z };

	D3DXVec3TransformCoord(&centerPos.pos, &Pos.pos, &modelWorld);
	D3DXVec3TransformCoord(&centerPos.pos, &Pos.pos, &modelWorld);


	D3DXVECTOR3 scale, trans;
	D3DXQUATERNION rotate;
	D3DXMatrixDecompose(&scale, &rotate, &trans, &modelWorld);

	D3DXMATRIX T, R, result;
	D3DXMatrixIdentity(&T);
	D3DXMatrixIdentity(&R);

	D3DXMatrixTranslation(&T, trans.x, trans.y, trans.z);
	D3DXMatrixRotationQuaternion(&R, &rotate);
	
	if (model != NULL)
	{
		D3DXMATRIX res = model->GetBoneMatrix(parentObjName);
		result = colliderWorld*res*R*T;
	}
	else
		result = colliderWorld*R*T;

	D3DXVec3TransformCoord(&centerPos.heightPos[0], &Pos.heightPos[0], &result);
	D3DXVec3TransformCoord(&centerPos.heightPos[1], &Pos.heightPos[1], &result);

	//높이 = (ymax-ymin)-2*pos.radius
	centerPos.height = Pos.height;
	centerPos.radius = Pos.radius;

	capsuleInfo.push_back(centerPos);
}

void ColliderCapsule::DrawCircle(vector<D3DXVECTOR3>&circlePos, int count, int startIdx, int endIdx,int type)
{
	float radiusRatio = 2.0f * Math::PI / CircleCount;
	float radius = 0;

	//반구중 윗부분
	//XZ평면의 원그리기 
	for (int i = 1; i <= count; i++)
	{
		switch (type)
		{
			case 0: // 위쪽 XZ원
			{
				D3DXVECTOR3 targetPos = { 0,Pos.pos.y+ Pos.height/2,0 };
				targetPos.x = cosf(radius)*Pos.radius+Pos.pos.x;
				targetPos.z = sinf(radius)*Pos.radius+Pos.pos.z;
				circlePos.push_back(targetPos);
				radius = i*radiusRatio;
			}
			break;
			case 1: // 위쪽 XY원
			{
				D3DXVECTOR3 targetPos = { 0,0,Pos.pos.z };
				targetPos.x = cosf(radius)*Pos.radius+Pos.pos.x;
				targetPos.y = sinf(radius)*Pos.radius+Pos.pos.y+ Pos.height/2;
				circlePos.push_back(targetPos);
				if (i == 1 || i == count)
					verticalLines.push_back(targetPos);
				radius = i*radiusRatio;
			}
			break;
			case 2: // 위쪽 ZY원
			{
				D3DXVECTOR3 targetPos = { Pos.pos.x,0,0 };
				targetPos.z = cosf(radius)*Pos.radius+ Pos.pos.z;
				targetPos.y = sinf(radius)*Pos.radius+ Pos.pos.y+ Pos.height/2;
				circlePos.push_back(targetPos);
				if (i == 1 || i == count)
					verticalLines.push_back(targetPos);
				radius = i*radiusRatio;
			}
			break;
			case 3: // 아래쪽 XZ
			{
				D3DXVECTOR3 targetPos = { 0,Pos.pos.y- Pos.height/2,0 };
				targetPos.x = cosf(radius)*Pos.radius + Pos.pos.x;
				targetPos.z = sinf(radius)*Pos.radius + Pos.pos.z;
				circlePos.push_back(targetPos);
				radius = i*radiusRatio;
			}
			break;
			case 4: // 아래쪽 XY
			{
				D3DXVECTOR3 targetPos = { 0,0,Pos.pos.z };
				targetPos.x = cosf(radius)*Pos.radius + Pos.pos.x;
				targetPos.y = sinf(radius)*Pos.radius + Pos.pos.y- Pos.height/2;
				circlePos.push_back(targetPos);
				if (i == 1 || i == count)
					verticalLines.push_back(targetPos);
				radius = -i*radiusRatio;
			}
			break;
			case 5: // 아래쪽 ZY
			{
				D3DXVECTOR3 targetPos = { Pos.pos.x,0,0 };
				targetPos.z = cosf(radius)*Pos.radius + Pos.pos.z;
				targetPos.y = sinf(radius)*Pos.radius + Pos.pos.y- Pos.height/2;
				circlePos.push_back(targetPos);
				if (i == 1 || i == count)
					verticalLines.push_back(targetPos);
				radius = -i*radiusRatio;
			}
			break;
		}
	}
	
	if (type == 0)
	{
		for (int i = startIdx; i < endIdx; i++)
		{
			if (i < endIdx - 1)
				ColliderLine[i].UpdateLine(circlePos[i], circlePos[i + 1]);
			else
				ColliderLine[i].UpdateLine(circlePos[i], circlePos[startIdx]);
		}
	}
	else
	{
		for (int i = startIdx; i < endIdx; i++)
		{
			if (i < endIdx-1)
				ColliderLine[i].UpdateLine(circlePos[i], circlePos[i + 1]);
			else
				ColliderLine[i].UpdateLine(circlePos[i], circlePos[startIdx]);
		}
	}
}

