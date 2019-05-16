#include "Framework.h"
#include "BoneSphere.h"

#include "Model/Model.h"
#include "Model/ModelClip.h"
#include "Model/ModelMesh.h"
#include "Model/ModelMeshPart.h"

BoneSphere::BoneSphere(Material* material)
	:MeshSphere(material, 1)
{
	Pass(2);

	colorVariable = this->GetMaterial()->GetEffect()->AsVector("BoneColor");

	color = { 1,0,0,1 };
	colorVariable->SetFloatVector(color);

	setPosition = { 0,0,0 };
	setRotate = { 0,0,0 };
	setScale = { 1,1,1 };

	D3DXMatrixIdentity(&setMat);
}

BoneSphere::~BoneSphere()
{
}

void BoneSphere::BoneRender()
{
	if (vertexBuffer == NULL && indexBuffer == NULL)
	{
		CreateBuffer();
	}

	UINT stride = sizeof(VertexTextureNormalTangent);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->WorldVariable()->SetMatrix(world);

	if (IsPicked())
	{
		color = { 0,1,0,1 };
		colorVariable->SetFloatVector(color);
	}
	else
	{
		color = { 1,0,0,1 };
		colorVariable->SetFloatVector(color);
	}
	material->GetEffect()->DrawIndexed(0, 2, indexCount);
}

void BoneSphere::PostBoneRender()
{
	if (!IsPicked())return;
	D3DXMATRIX V, P;
	Context::Get()->GetMainCamera()->Matrix(&V);
	Context::Get()->GetPerspective()->GetMatrix(&P);

	D3DXMATRIX matrix;
	D3DXMatrixMultiply(&matrix, &V, &P);

	D3DXMATRIX world;
	Matrix(&world);

	D3DXMATRIX WVP = world*matrix;

	D3DXVECTOR4 worldToScreen(0, 0, 0, 1);
	D3DXVec4Transform(&worldToScreen, &worldToScreen, &WVP);

	//NDC 공간으로 변환
	float wScreenX = worldToScreen.x / worldToScreen.w;
	float wScreenY = worldToScreen.y / worldToScreen.w;
	float wScreenZ = worldToScreen.z / worldToScreen.w;

	//-1~1 구간을 0~1구간으로 변환
	float nScreenX = (wScreenX + 1)*0.5f;
	float nScreenY = 1.0f - (wScreenY + 1)*0.5f;

	//최종화면의 좌표
	float resultX = nScreenX*Context::Get()->GetViewport()->GetWidth();
	float resultY = nScreenY*Context::Get()->GetViewport()->GetHeight();

	//RECT rect = { resultX,resultY,resultX + 100,resultY + 100 };

	//DirectWrite::Get()->RenderText(Name(), rect, 15, L"돋음체", D3DXCOLOR(1, 1, 0, 1));
}

void BoneSphere::BoneImGuiRender()
{
	ImGui::Separator();
	ImGui::Separator();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "===========[%s]===========", String::ToString(Name()).c_str());

	ImGui::Separator();
	ImGui::Separator();
	wstring textTrans = L"[" + Name() + L"]translate";
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set SRT]");
	ImGui::Text(String::ToString(textTrans).c_str());

	D3DXVECTOR3 t, preT;
	t = setPosition;
	preT = t;
	ImGui::DragFloat3((String::ToString(Name()) + "T").c_str(), (float*)&t, 0.05f, -128, 128);

	if (t != preT)
	{
		setPosition = t;
		SetMatrixUpdate();
	}

	wstring textScale = L"[" + Name() + L"]scale";
	ImGui::Text(String::ToString(textScale).c_str());

	D3DXVECTOR3 s, preS;
	s = setScale;
	preS = s;

	ImGui::DragFloat3((String::ToString(Name()) + "S").c_str(), (float*)&s, 0.05f, -10, 10);
	if (s != preS)
	{
		setScale = s;
		SetMatrixUpdate();
	}


	wstring textRotate = L"[" + Name() + L"]rotate";
	ImGui::Text(String::ToString(textRotate).c_str());

	D3DXVECTOR3 r, preR;
	r = setRotate;
	preR = r;
	ImGui::DragFloat3((String::ToString(Name()) + "R").c_str(), (float*)&r, 0.05f, -3.14f, 3.14f);

	if (r != preR)
	{
		setRotate = r;
		SetMatrixUpdate();
	}

}

void BoneSphere::SetMatrixUpdate()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, setScale.x, setScale.y, setScale.z);
	D3DXMatrixRotationYawPitchRoll(&R, setRotate.y, setRotate.x, setRotate.z);
	D3DXMatrixTranslation(&T, setPosition.x, setPosition.y, setPosition.z);

	setMat = S*R*T;
}





