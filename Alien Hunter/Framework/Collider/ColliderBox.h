#pragma once
#include "Framework.h"

class ColliderBox : public ColliderElement
{
public:
	ColliderBox(wstring name, D3DXVECTOR3& min, D3DXVECTOR3& max, Effect*effect = NULL)
		: ColliderElement(effect)
	{
		localMin = min;
		localMax = max;

		boundingBox = new BBox(min, max);
		boundingBox->GetLine(transform, posVec);

		resultBox.resize(posVec.size());

		Name(name);
		Type((UINT)ColliderElement::EType::EBox);

	}

	ColliderBox(ColliderBox&copy)
		:ColliderElement(copy)
	{
		localMin = copy.localMin;
		localMax = copy.localMax;

		boundingBox = new BBox(localMin, localMax);
		boundingBox->GetLine(transform, posVec);

		resultBox.resize(posVec.size());

		Name(copy.name);
		Type((UINT)ColliderElement::EType::EBox);
	}

	virtual ~ColliderBox()
	{
		SAFE_DELETE(boundingBox);
	}

	D3DXVECTOR3 LocalMin() { return localMin; }
	D3DXVECTOR3 LocalMax() { return localMax; }
	D3DXVECTOR3 WorldMin() { return boundingBox->Min; }
	D3DXVECTOR3 WorldMax() { return boundingBox->Max; }

	void ResetMinMax(D3DXVECTOR3 min, D3DXVECTOR3 max)
	{
		localMin = min;
		localMax = max;

		boundingBox->ResetMinMax(min, max);
		boundingBox->GetLine(transform, posVec);
	}

	void Transform(D3DXMATRIX& world)
	{
		D3DXVECTOR3 minpos, maxpos;

		minpos = { 0,0,0 };
		maxpos = { 0,0,0 };

		D3DXMATRIX result = transform*world;

		D3DXVec3TransformCoord(&minpos, &minpos, &result);
		D3DXVec3TransformCoord(&maxpos, &maxpos, &result);

		for (UINT i = 0; i < resultBox.size(); i++)
		{
			D3DXVec3TransformCoord(&resultBox[i], &posVec[i], &result);
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
			if (maxpos.z < resultBox[i].z)
				maxpos.z = resultBox[i].z;
		}

		boundingBox->Min = minpos;
		boundingBox->Max = maxpos;

		if (isShowLine)
		{
			D3DXMATRIX initWorld;
			D3DXMatrixIdentity(&initWorld);
			line->Draw(initWorld, boundingBox);
		}
	}

	BBox * BoundingBox() { return boundingBox; }

	virtual void ImguiRender()
	{
		/*ImGui::TextColored(ImVec4(1, 0, 0, 1), "[ParentObject]");
		ImGui::Text("%s", String::ToString(parentObjName).c_str());
		ImGui::Separator();*/

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[MinMaxPos]");
		ImGui::Text("MinPos(%.1f,%.1f,%.1f)", boundingBox->Min.x, boundingBox->Min.y, boundingBox->Min.z);
		ImGui::Text("MaxPos(%.1f,%.1f,%.1f)", boundingBox->Max.x, boundingBox->Max.y, boundingBox->Max.z);

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

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[SetScale]");
		wstring textScale = L"[" + Name() + L"]scale";
		ImGui::Text(String::ToString(textScale).c_str());
		D3DXVECTOR3 scale;
		Scale(&scale);
		ImGui::DragFloat3("##S", scale, 0.05f, -10, 10);
		Scale(scale);
	}
private:
	D3DXVECTOR3 localMin;
	D3DXVECTOR3 localMax;

	BBox * boundingBox;

	vector<D3DXVECTOR3> posVec;
	vector<D3DXVECTOR3> resultBox;
};