#pragma once
#include "Framework.h"

class ColliderRay : public ColliderElement
{
public:
	ColliderRay(D3DXVECTOR3& position, D3DXVECTOR3& direction, Effect*effect = NULL)
		:ColliderElement(effect)
	{
		localPosition = position;
		localDirection = direction;

		ray = new Ray(position, direction);

		Type((UINT)ColliderElement::EType::ERay);
	}
	ColliderRay(wstring name, D3DXVECTOR3& position, D3DXVECTOR3& direction, Effect*effect = NULL)
		:ColliderElement(effect)
	{
		localPosition = position;
		localDirection = direction;

		ray = new Ray(position, direction);

		Name(name);
		Type((UINT)ColliderElement::EType::ERay);
	}
	ColliderRay(ColliderRay&copy)
	{
		localPosition = copy.localPosition;
		localDirection = copy.localDirection;

		ray = new Ray(localPosition, localDirection);

		Name(copy.name);
		Type((UINT)ColliderElement::EType::ERay);
	}

	virtual ~ColliderRay()
	{
		SAFE_DELETE(ray);
	}

	Ray * GetRay() { return ray; }
	D3DXVECTOR3 LocalPosition() { return localPosition; }
	D3DXVECTOR3 LocalDirection() { return localDirection; }
	D3DXVECTOR3 WorldPosition() { return ray->Position; }
	D3DXVECTOR3 WorldDirection() { return ray->Direction; }

	void Transform(D3DXMATRIX& world)
	{
		D3DXMATRIX result = transform*world;

		D3DXVec3TransformCoord(&ray->Position, &localPosition, &result);
		D3DXVec3TransformNormal(&ray->Direction, &localDirection, &result);
		D3DXVec3Normalize(&ray->Direction, &ray->Direction);

		if (isShowLine)
		{
			D3DXMATRIX initWorld;
			D3DXMatrixIdentity(&initWorld);
			line->Draw(initWorld, ray);
		}
	}

	virtual void ImguiRender()
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[StartPos]");
		ImGui::Text("StartPos(%.1f,%.1f,%.1f)", ray->Position.x, ray->Position.y, ray->Position.z);

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
		ImGui::DragFloat3("##T", pos, 0.05f, -128, 128);
		Position(pos);
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetDirection]");
		ImGui::DragFloat3("##D", localDirection, 0.05f, -1, 1);
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetDistance]");
		ImGui::DragFloat("##dis", &ray->Distance, 0.05f, 0, 100);
	}
private:
	D3DXVECTOR3 localPosition;
	D3DXVECTOR3 localDirection;

	Ray * ray;
};