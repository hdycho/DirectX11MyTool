#pragma once
#include "Framework.h"

class ColliderSphere : public ColliderElement
{
public:
	ColliderSphere(wstring name, D3DXVECTOR3 center, float radius, Effect*effect = NULL)
		: ColliderElement(effect), radius(radius)
	{
		localCenter = center;

		boundingSphere = new BSphere(center, this->radius);

		Name(name);
		Type((UINT)ColliderElement::EType::ESphere);
	}
	ColliderSphere(ColliderSphere&copy)
	{
		localCenter = copy.localCenter;
		radius = copy.radius;

		boundingSphere = new BSphere(localCenter, radius);

		Name(copy.name);
		Type((UINT)ColliderElement::EType::ESphere);
	}
	virtual ~ColliderSphere()
	{
		SAFE_DELETE(boundingSphere);
	}

	BSphere * BoundingSphere() { return boundingSphere; }

	D3DXVECTOR3 LocalCenter() { return localCenter; }
	D3DXVECTOR3 WorldCenter() { return boundingSphere->Center; }

	float&Radius() { return boundingSphere->Radius; }

	void Transform(D3DXMATRIX& world)
	{
		D3DXMATRIX result = transform*world;
		D3DXVec3TransformCoord(&boundingSphere->Center, &localCenter, &result);

		float x = world._11;
		float y = world._22;
		float z = world._33;

		float temp = max(x, max(y, z));
		//boundingSphere->Radius *= temp;

		if (isShowLine)
		{
			D3DXMATRIX initWorld;
			D3DXMatrixIdentity(&initWorld);
			line->Draw(initWorld, boundingSphere);
		}
	}

	virtual void ImguiRender()
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[CenterPos]");
		ImGui::Text("CenterPos(%.1f,%.1f,%.1f)", boundingSphere->Center.x, boundingSphere->Center.y, boundingSphere->Center.z);

		ImGui::Separator();
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Set Collider]");
		if (ImGui::Button("Show Collider"))
		{
			if (!isShowLine)isShowLine = true;
			else isShowLine = false;
		}

		ImGui::Separator();
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetPosition]");
		wstring textTrans = L"[" + Name() + L"]translate";
		ImGui::Text(String::ToString(textTrans).c_str());
		D3DXVECTOR3 pos;
		Position(&pos);
		ImGui::DragFloat3("##T", pos, 0.05f, -300, 300);
		Position(pos);
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetRadius]");
		ImGui::DragFloat("##dis", &boundingSphere->Radius, 0.05f, 0, 100);
		ImGui::Separator();
	}
private:
	D3DXVECTOR3 localCenter;
	BSphere * boundingSphere;

	float radius;
};