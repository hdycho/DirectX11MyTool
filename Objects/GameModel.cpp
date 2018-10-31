#include "stdafx.h"
#include "GameModel.h"
#include "./Model/ModelMeshPart.h"
#include "../Model/Model.h"
#include "../Renders/Texture.h"
#include "../Physics/ColliderCapsule.h"
#include "../Physics/ColliderBox.h"
#include "../Physics/Trail.h"

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder ,wstring meshFile, ExecuteValues*values)
	:shader(NULL),velocity(0,0,0),values(values)
{
	isPick = false;
	isOneClick = false;
	isColliderOpen = false;
	isTrailOpen = false;

	myPtr = this;
	modelType = L"DefaultModel";
	className= L"DefaultModel";

	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	renderBuffer = new RenderBuffer();
	normalBuffer = new NormalBuffer();
	
	///////////////////초기에 한번돌아야함///////////////////
	CalcPosition();
	D3DXMATRIX t = Transformed();
	model->CopyGlobalBoneTo(boneTransforms, t);

	for (UINT i = 0; i < model->Bones().size(); i++)
	{
		initBonesTransforms.push_back(model->Bones()[0]->Local());
	}
	/////////////////////////////////////////////////////

	PickPosInit();

	isVisible = true;
	isActive = true;
}

GameModel::GameModel(GameModel & gameModel)
{
	shader = NULL;
	velocity = { 0, 0, 0 };
	
	values = gameModel.values;
	isPick = false;
	myPtr = this;
	
	Scale() = gameModel.Scale();
	Rotation() = gameModel.Rotation();
	Position() = gameModel.Position();

	modelType = gameModel.modelType;
	model = new Model(*gameModel.model);
	
	renderBuffer = new RenderBuffer();
	normalBuffer = new NormalBuffer();

	CalcPosition();
	D3DXMATRIX t = Transformed();
	model->CopyGlobalBoneTo(boneTransforms, t);

	for (UINT i = 0; i < model->Bones().size(); i++)
	{
		initBonesTransforms.push_back(model->Bones()[0]->Local());
	}

	PickPosInit();
	isVisible = true;
	isActive = true;

	for (int i = 0; i < gameModel.colliders.size(); i++)
	{
		Collider*collider;
		if (gameModel.colliders[i]->ColliderState() == COLLIDERSTATE::BOX)
		{
			collider = new ColliderBox(World(), COLLIDERSTATE::BOX);
			*collider = *gameModel.colliders[i];
		}
		else if (gameModel.colliders[i]->ColliderState() == COLLIDERSTATE::CAPSULE)
		{
			collider = new ColliderCapsule(World(), COLLIDERSTATE::CAPSULE);
			*collider = *gameModel.colliders[i];
		}
		collider->InputColliderInfo(&initBoxPos, &curBoxPos, &minmaxPos);
		collider->ColliderLineInit();
		colliders.push_back(collider);
	}
}

GameModel::~GameModel()
{
	SAFE_DELETE(normalBuffer);
	SAFE_DELETE(renderBuffer);
	SAFE_DELETE(shader);
	SAFE_DELETE(model);
}

void GameModel::Velocity(D3DXVECTOR3 & vec)
{
	velocity = vec;
}

D3DXVECTOR3 GameModel::Velocity()
{
	return velocity;
}

void GameModel::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

void GameModel::Scale(D3DXVECTOR3 vec)
{
	/*if (model->Bones().size() > 1)
		model->Buffer()->Scale(vec);
	else*/
	GameRender::Scale(vec);
}

D3DXVECTOR3&GameModel::Scale()
{
	//return model->Buffer()->Scale();
	return GameRender::Scale();
}

void GameModel::Update()
{
	if (isActive)
	{
		if (Mouse::Get()->Down(0) && IsPicked())
		{
			isOneClick = true;
			if (isPick)
				isPick = false;
			else
				isPick = true;
		}
		else
			isOneClick = false;

		UpdatePickPos();
		CalcPosition();

		D3DXMATRIX t;
		t = Transformed();
		model->CopyGlobalBoneTo(boneTransforms, t);

		for (int i = 0; i < colliders.size(); i++)
			colliders[i]->Update(World());

		for (int i = 0; i < trails.size(); i++)
			trails[i]->Update();
	}
}

void GameModel::Render()
{
	model->Buffer()->Bones(&boneTransforms[0], boneTransforms.size());
	model->Buffer()->SetVSBuffer(2);

	for (int i = 0; i < colliders.size(); i++)
		colliders[i]->Render();

	for (int i = 0; i < trails.size(); i++)
		trails[i]->Render();

	for (ModelMesh*mesh : model->Meshes())
	{
		int index = mesh->ParentBoneIndex();

		renderBuffer->Data.Index = index;
		renderBuffer->SetVSBuffer(3);
		if(isVisible)
			mesh->Render();
	}
}

void GameModel::TrailSetRender()
{
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Trail]");
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Trail[%d]", trails.size());
	ImGui::SameLine();
	if (ImGui::ArrowButton("Trails", ImGuiDir_Down))
	{
		if (!isTrailOpen)isTrailOpen = true;
		else isTrailOpen = false;
	}

	if (isTrailOpen)
	{
		for (int i = 0; i < trails.size(); i++)
		{
			if (ImGui::TreeNode((String::ToString(trails[i]->GetTrailName())).c_str()))
			{
				trails[i]->ImGuiRender();

				ImGui::TreePop();
			}
			else if (ImGui::BeginDragDropSource())//하이라키모델 다르쪽에 넣을때 옵션
			{
				ImGui::SetDragDropPayload("Trail", trails[i], sizeof(Trail));
				ImGui::Text(String::ToString(trails[i]->GetTrailName()).c_str());
				ImGui::EndDragDropSource();
			}
		}
	}
}

void GameModel::ColliderSetRender()
{
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Colliders]");
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Colliders[%d]", colliders.size());
	ImGui::SameLine();
	if (ImGui::ArrowButton("Colliders", ImGuiDir_Down))
	{
		if (!isColliderOpen)isColliderOpen = true;
		else isColliderOpen = false;
	}

	if (isColliderOpen)
	{
		for (int i = 0; i < colliders.size(); i++)
		{
			if (ImGui::TreeNode((String::ToString(colliders[i]->GetColliderName())).c_str()))
			{
				colliders[i]->ImguiRender();

				ImGui::TreePop();
			}
			else if (ImGui::BeginDragDropSource())//하이라키모델 다르쪽에 넣을때 옵션
			{
				ImGui::SetDragDropPayload("Collider", colliders[i], sizeof(Collider));
				ImGui::Text(String::ToString(colliders[i]->GetColliderName()).c_str());
				ImGui::EndDragDropSource();
			}
		}
	}

	ImGui::Separator();
	ImGui::Separator();
}

D3DXMATRIX GameModel::GetBoneMatrix(wstring boneName)
{
	ModelBone* bone = model->BoneByName(boneName);
	int index = bone->Index();

	D3DXMATRIX matrix = bone->Global() * boneTransforms[index];

	return matrix;
}

D3DXMATRIX GameModel::GetBoneMatrix(int idx)
{
	ModelBone* bone = model->BoneByIndex(idx);
	int index = idx;

	D3DXMATRIX matrix = bone->Global() * boneTransforms[index];

	return matrix;
}

void GameModel::SetShader(Shader * shader)
{
	for (Material*material : model->Materials())
		material->SetShader(shader);
}
void GameModel::SetDiffuse(float r, float g, float b, float a)
{
	SetDiffuse(D3DXCOLOR(r, g, b, a));
}
void GameModel::SetDiffuse(D3DXCOLOR & color)
{
	for (Material*material : model->Materials())
		material->SetDiffuse(color);
}
void GameModel::SetDiffuseMap(wstring file)
{
	for (Material*material : model->Materials())
		material->SetDiffuseMap(file);
}
void GameModel::SetDiffuseMap(Texture * diffues)
{
	for (Material*material : model->Materials())
		material->GetDiffuseMap() = diffues;
}
void GameModel::SetDiffuseMap(Texture * diffuse, int idx)
{
	Texture*newTexture = new Texture(*diffuse);

	model->Materials()[idx]->GetDiffuseMap() = newTexture;
}
void GameModel::SetSpecular(float r, float g, float b, float a)
{
	SetSpecular(r, g, b, a);
}
void GameModel::SetSpecular(D3DXCOLOR & color)
{
	for (Material*material : model->Materials())
		material->SetSpecular(color);
}
void GameModel::SetSpecularMap(wstring file)
{
	for (Material*material : model->Materials())
		material->SetSpecularMap(file);
}
void GameModel::SetSpecularMap(Texture * diffues)
{
	for (Material*material : model->Materials())
		material->GetSpecularMap() = diffues;
}
void GameModel::SetSpecularMap(Texture * diffues, int idx)
{
	Texture*newTexture = new Texture(*diffues);

	model->Materials()[idx]->GetSpecularMap() = newTexture;
}
void GameModel::SetNormalMap(wstring file)
{
	for (Material*material : model->Materials())
		material->SetNormalMap(file);
}
void GameModel::SetNormalMap(Texture * diffues)
{
	for (Material*material : model->Materials())
		material->GetNormalMap() = diffues;
}
void GameModel::SetNormalMap(Texture * diffues, int idx)
{
	Texture*newTexture = new Texture(*diffues);

	model->Materials()[idx]->GetNormalMap() = newTexture;
}
void GameModel::SetDetailMap(wstring file)
{
	for (Material*material : model->Materials())
		material->SetDetailMap(file);
}
void GameModel::SetShininess(float val)
{
	for (Material*material : model->Materials())
		material->SetShininess(val);
}

D3DXVECTOR3 GameModel::GetCenterPos()
{
	float centetX = (minmaxPos[0].pos.x + minmaxPos[1].pos.x) / 2.0f;
	float centerY = (minmaxPos[0].pos.y + minmaxPos[1].pos.y) / 2.0f;
	float centetZ = (minmaxPos[0].pos.z + minmaxPos[1].pos.z) / 2.0f;
	return D3DXVECTOR3(centetX, centerY, centetZ);
}

D3DXVECTOR3 GameModel::GetSize()
{
	float centetX = abs(minmaxPos[0].pos.x - minmaxPos[1].pos.x);
	float centerY = abs(minmaxPos[0].pos.y - minmaxPos[1].pos.y);
	float centetZ = abs(minmaxPos[0].pos.z - minmaxPos[1].pos.z);
	return D3DXVECTOR3(centetX, centerY, centetZ);
}

void GameModel::PickPosInit()
{
	D3DXVECTOR3 minPoint = { 0,0,0 };
	D3DXVECTOR3 maxPoint = { 0,0,0 };

	D3DXMATRIX world;

	for (ModelMesh*mesh : model->Meshes())
	{
		ModelBone*bone = mesh->ParentBone();
		world = bone->Global();

		for (ModelMeshPart*meshPart : mesh->GetMeshParts())
		{
			vector<ModelVertexType> vertices = meshPart->GetVertices();

			for (ModelVertexType vertex : vertices)
			{
				D3DXVECTOR3 pos = vertex.Position;
				D3DXVec3TransformCoord(&pos, &pos, &world);

				if (minPoint.x > pos.x)minPoint.x = pos.x;
				if (minPoint.y > pos.y)minPoint.y = pos.y;
				if (minPoint.z > pos.z)minPoint.z = pos.z;

				if (maxPoint.x < pos.x)maxPoint.x = pos.x;
				if (maxPoint.y < pos.y)maxPoint.y = pos.y;
				if (maxPoint.z < pos.z)maxPoint.z = pos.z;
			}
		}
	}

	float disx = abs(maxPoint.x - minPoint.x);
	float disy = abs(maxPoint.y - minPoint.y);
	float disz = abs(maxPoint.z - minPoint.z);

	minmaxPos.resize(2);

	minmaxPos[0].pos = minPoint;
	minmaxPos[1].pos = maxPoint;

	initBoxPos.push_back(D3DXVECTOR3(minmaxPos[0].pos.x, minmaxPos[0].pos.y, minmaxPos[0].pos.z));
	initBoxPos.push_back(D3DXVECTOR3(minmaxPos[0].pos.x, minmaxPos[0].pos.y + disy, minmaxPos[0].pos.z));
	initBoxPos.push_back(D3DXVECTOR3(minmaxPos[0].pos.x + disx, minmaxPos[0].pos.y, minmaxPos[0].pos.z));
	initBoxPos.push_back(D3DXVECTOR3(minmaxPos[0].pos.x + disx, minmaxPos[0].pos.y + disy, minmaxPos[0].pos.z));
	initBoxPos.push_back(D3DXVECTOR3(minmaxPos[1].pos.x - disx, minmaxPos[1].pos.y - disy, minmaxPos[1].pos.z));
	initBoxPos.push_back(D3DXVECTOR3(minmaxPos[1].pos.x - disx, minmaxPos[1].pos.y, minmaxPos[1].pos.z));
	initBoxPos.push_back(D3DXVECTOR3(minmaxPos[1].pos.x, minmaxPos[1].pos.y - disy, minmaxPos[1].pos.z));
	initBoxPos.push_back(D3DXVECTOR3(minmaxPos[1].pos.x, minmaxPos[1].pos.y, minmaxPos[1].pos.z));

	curBoxPos.assign(8, D3DXVECTOR3(0,0,0));
}

void GameModel::UpdatePickPos()
{
	D3DXVECTOR3 minpos,maxpos;

	minpos = { 0,0,0 };
	maxpos = { 0,0,0 };

	D3DXVec3TransformCoord(&minpos, &minpos, &World());
	D3DXVec3TransformCoord(&maxpos, &maxpos, &World());

	minmaxPos.clear();
	for (int i = 0; i < 8; i++)
	{
		D3DXVec3TransformCoord(&curBoxPos[i], &initBoxPos[i], &World());
		//최대최소점 바꿔야함
		if (minpos.x > curBoxPos[i].x)
			minpos.x = curBoxPos[i].x;
		if (maxpos.x < curBoxPos[i].x)
			maxpos.x = curBoxPos[i].x;

		if (minpos.y > curBoxPos[i].y)
			minpos.y = curBoxPos[i].y;
		if (maxpos.y < curBoxPos[i].y)
			maxpos.y = curBoxPos[i].y;

		if (minpos.z > curBoxPos[i].z)
			minpos.z = curBoxPos[i].z;
		if (maxpos.y < curBoxPos[i].z)
			maxpos.y = curBoxPos[i].z;
	}

	minmaxPos.resize(2);

	minmaxPos[0].pos = minpos;
	minmaxPos[1].pos = maxpos;
}

bool GameModel::IsPicked()
{
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);

	D3DXVECTOR3 direction = values->MainCamera->Direction
	(values->Viewport, values->Perspective);

	D3DXMATRIX inverse;
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);
	mWorld._41 = World()._41;
	mWorld._42 = World()._42;
	mWorld._43 = World()._43;

	D3DXMatrixInverse(&inverse, NULL, &mWorld);

	D3DXVec3TransformNormal(&direction, &direction, &inverse);

	//내가구한 최대최소점의 면과 선이 충돌했을때 트루반환

	vector<UINT> indices;
	indices.resize(36);
	indices = {
		0,1,2,2,1,3,
		2,3,6,6,3,7,
		6,7,4,4,7,5,
		0,1,4,4,1,5,
		1,5,3,3,5,7,
		0,4,2,2,4,6
	};

	for (int i = 0; i < 6; i++)
	{
		D3DXVECTOR3 p[6];
		for (int j = 0; j < 6; j++)
			p[j] = curBoxPos[indices[6 * i + j]];

		float u, v, distance;

		if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
		{
			return true;
		}
		else if (D3DXIntersectTri(&p[3], &p[4], &p[5], &start, &direction, &u, &v, &distance))
		{
			return true;
		}
	}
	return false;
}


void GameModel::CalcPosition()
{
	if (D3DXVec3Length(&velocity) <= 0.0f)
		return;

	D3DXVECTOR3 vec(0, 0, 0);

	if (velocity.z != 0.0f)
		vec += Direction() * velocity.z;

	if (velocity.y != 0.0f)
		vec += Up() * velocity.y;

	if (velocity.x != 0.0f)
		vec += Right() * velocity.x;

	D3DXVECTOR3 pos = Position() + vec*Time::Delta();
	Position(pos);
}




