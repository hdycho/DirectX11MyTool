#include "stdafx.h"
#include "MakeAnimation.h"
#include "../Utilities/Xml.h"
#include "../Objects/GameModel.h"

MakeAnimation::MakeAnimation(GameModel*model, int boneIdx, D3DXVECTOR3*t, D3DXVECTOR3*s, D3DXVECTOR3*r)
	:trans(t),scale(s),rot(r)
{
	modelBone = model->GetModel()->BoneByIndex(boneIdx);
	boneMatrix = modelBone->Local();
	rotate = { 0,0,0,0 };

	index = 0;
	maxIndex = 60;

	time = 0;
	elapsedTime = 0;
	count = 0;

	vTrans.push_back(*trans);
	vScale.push_back(*scale);
	vRotate.push_back(rotate);
	vIndex.push_back(index);

	modelName = modelBone->Name();
	animState = CONTROL;
	animSpeed = 10.0f;
}

MakeAnimation::~MakeAnimation()
{
}

void MakeAnimation::Render()
{
	ImGui::Begin("AnimationBar");
	{
		if (ImGui::Button("Play"))
		{
			animState = PLAY;
			*trans = vTrans[0];
			*scale = vScale[0];
			rotate = vRotate[0];

			count = 0;
			index = 0;
			time = 0;
			elapsedTime = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			vTrans.push_back(*trans);
			vScale.push_back(*scale);
			vRotate.push_back(rotate);
			vIndex.push_back(index);
		}
		ImGui::SameLine();
		if (ImGui::Button("LoadData"))
		{
			Path::OpenFileDialog(L"ÆÄÀÏ", L".ani", Models, bind(&MakeAnimation::LoadAnimData, this, placeholders::_1));
		}
		ImGui::SameLine();
		if (ImGui::Button("SaveData"))
		{
			SaveAnimData();
		}
		ImGui::SliderInt("Frame", &index, 0, maxIndex);


		if (ImGui::InputText("AnimSpeed", speed, sizeof(speed)))
		{
			animSpeed = atoi(speed);
		}
	}
	ImGui::End();
}

void MakeAnimation::Update()
{
	UpdateTurret();
}


void MakeAnimation::SaveAnimData()
{
	Xml::XMLDocument*document = new Xml::XMLDocument();

	Xml::XMLDeclaration *decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Animation");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	Xml::XMLElement*node = NULL;
	node = document->NewElement("MAXFPS");
	node->SetText(maxIndex);
	root->LinkEndChild(node);

	node = document->NewElement("AnimSpeed");
	node->SetText(animSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("Indices");
	for (int i = 0; i < vIndex.size(); i++)
	{
		Xml::XMLElement*index = document->NewElement("Index");
		index->SetText(vIndex[i]);
		node->LinkEndChild(index);
	}
	root->LinkEndChild(node);

	node = document->NewElement("Trans");
	for (int i = 0; i < vTrans.size(); i++)
	{
		Xml::XMLElement*trans = document->NewElement("X");
		trans->SetText(vTrans[i].x);
		node->LinkEndChild(trans);

		trans = document->NewElement("Y");
		trans->SetText(vTrans[i].y);
		node->LinkEndChild(trans);

		trans = document->NewElement("Z");
		trans->SetText(vTrans[i].z);
		node->LinkEndChild(trans);
	}
	root->LinkEndChild(node);

	node = document->NewElement("Scale");
	for (int i = 0; i < vScale.size(); i++)
	{
		Xml::XMLElement*scale = document->NewElement("X");
		scale->SetText(vScale[i].x);
		node->LinkEndChild(scale);

		scale = document->NewElement("Y");
		scale->SetText(vScale[i].y);
		node->LinkEndChild(scale);

		scale = document->NewElement("Z");
		scale->SetText(vScale[i].z);
		node->LinkEndChild(scale);
	}
	root->LinkEndChild(node);

	node = document->NewElement("Rotate");
	for (int i = 0; i < vRotate.size(); i++)
	{
		Xml::XMLElement*rotate = document->NewElement("X");
		rotate->SetText(vRotate[i].x);
		node->LinkEndChild(rotate);

		rotate = document->NewElement("Y");
		rotate->SetText(vRotate[i].y);
		node->LinkEndChild(rotate);

		rotate = document->NewElement("Z");
		rotate->SetText(vRotate[i].z);
		node->LinkEndChild(rotate);

		rotate = document->NewElement("W");
		rotate->SetText(vRotate[i].w);
		node->LinkEndChild(rotate);
	}
	root->LinkEndChild(node);

	string file = String::ToString(Models + L"Tank/"+modelName+L"TankAnim.ani");
	document->SaveFile(file.c_str());
}

void MakeAnimation::LoadAnimData(wstring fName)
{
	vTrans.clear();
	vScale.clear();
	vRotate.clear();
	vIndex.clear();

	*trans = { 0,0,0 };
	*scale = { 1,1,1 };
	*rot = { 0, 0, 0 };
	rotate = { 0,0,0,0 };

	index = 0;
	maxIndex = 60;

	time = 0;
	elapsedTime = 0;
	count = 0;
	animState = CONTROL;
	animSpeed = 0;


	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = fName;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*node = root->FirstChildElement();
	maxIndex = node->IntText();

	node = node->NextSiblingElement();
	animSpeed = node->FloatText();

	node = node->NextSiblingElement();
	Xml::XMLElement*index = node->FirstChildElement();
	do {
		vIndex.push_back(index->IntText());
		index = index->NextSiblingElement();
	} while (index != NULL);

	node = node->NextSiblingElement();
	Xml::XMLElement*trans = node->FirstChildElement();
	do {
		float x = trans->FloatText();
		trans = trans->NextSiblingElement();
		float y = trans->FloatText();
		trans = trans->NextSiblingElement();
		float z = trans->FloatText();
		trans = trans->NextSiblingElement();

		vTrans.push_back(D3DXVECTOR3(x, y, z));
	} while (trans != NULL);

	node = node->NextSiblingElement();
	Xml::XMLElement*scale = node->FirstChildElement();
	do {
		float x = scale->FloatText();
		scale = scale->NextSiblingElement();
		float y = scale->FloatText();
		scale = scale->NextSiblingElement();
		float z = scale->FloatText();
		scale = scale->NextSiblingElement();

		vScale.push_back(D3DXVECTOR3(x, y, z));
	} while (scale != NULL);

	node = node->NextSiblingElement();
	Xml::XMLElement*rotate = node->FirstChildElement();
	do {
		float x = rotate->FloatText();
		rotate = rotate->NextSiblingElement();
		float y = rotate->FloatText();
		rotate = rotate->NextSiblingElement();
		float z = rotate->FloatText();
		rotate = rotate->NextSiblingElement();
		float w = rotate->FloatText();
		rotate = rotate->NextSiblingElement();

		vRotate.push_back(D3DXQUATERNION(x, y, z, w));
	} while (rotate != NULL);

}


void MakeAnimation::UpdateTurret()
{
	D3DXMATRIX S, R, T;
	D3DXMATRIX result;
	D3DXMatrixIdentity(&S);
	D3DXMatrixIdentity(&R);
	D3DXMatrixIdentity(&T);

	float deltaTime = 0;

	switch (animState)
	{
	case MakeAnimation::CONTROL:
	{
		D3DXQuaternionRotationYawPitchRoll(&rotate, rot->y, rot->x, rot->z);
		D3DXMatrixRotationQuaternion(&R, &rotate);
	}
	break;
	case MakeAnimation::PLAY:
	{
		time += Time::Delta() * animSpeed;
		index = (int)time;

		if (index == vIndex[count])
		{
			if (count < vIndex.size() - 1)
			{
				count++;
				deltaTime = 0;
				elapsedTime = 0;
			}
		}

		D3DXVECTOR3 v1 = vTrans[count - 1];
		D3DXVECTOR3 v2 = vTrans[count];
		D3DXVECTOR3 s1 = vScale[count - 1];
		D3DXVECTOR3 s2 = vScale[count];
		D3DXQUATERNION q1 = vRotate[count - 1];
		D3DXQUATERNION q2 = vRotate[count];

		elapsedTime += Time::Delta() * animSpeed;
		deltaTime = elapsedTime / (float)(vIndex[count] - vIndex[count - 1]);

		if (deltaTime < 1)
		{
			D3DXVec3Lerp(trans, &v1, &v2, deltaTime);
			D3DXVec3Lerp(scale, &s1, &s2, deltaTime);
			D3DXQuaternionSlerp(&rotate, &q1, &q2, deltaTime);
		}
		else
		{
			*trans = v2;
			*scale = s2;
			rotate = q2;
		}

		if (index >= maxIndex)
		{
			count = 0;
			index = 0;
			time = 0;
			elapsedTime = 0;
			*trans = vTrans[0];
			*scale = vScale[0];
			*rot = { 0,0,0 };
			rotate = vRotate[0];
			animState = CONTROL;
			break;
		}
		D3DXMatrixRotationQuaternion(&R, &rotate);
	}
	break;
	}

	D3DXMatrixScaling(&S, scale->x, scale->y, scale->z);
	D3DXMatrixTranslation(&T, trans->x, trans->y, trans->z);

	result = S*R*T;

	modelBone->Local(boneMatrix*result);
}
