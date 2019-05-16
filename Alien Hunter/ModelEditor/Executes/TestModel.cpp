#include "stdafx.h"
#include "TestModel.h"
#include "Fbx/FbxLoader.h"
#include "Model\Model.h"

void TestModel::Initialize()
{
	/*FbxLoader * loader = NULL;

	loader = new FbxLoader
	(
	Assets + L"Mutant/mutant.fbx",
	Models + L"Mutant/", L"Mutant"
	);
	loader->ExportMaterial();
	loader->ExportMesh();
	SAFE_DELETE(loader);*/

	for (int i = 0; i < 3; i++)
	{
		model[i] = new GameModel(i,
			Models + L"Player/Player.material",
			Models + L"Player/Player.mesh");
	}
}

void TestModel::Ready()
{
	for (int i = 0; i < 3; i++)
	{
		model[i]->Ready();
		//model[i]->CreateCollider(0);
	}
}

void TestModel::Destroy()
{
	for (int i = 0; i < 3; i++)
	{
		SAFE_DELETE(model[i]);
	}
}

void TestModel::Update()
{
	for (int i = 0; i < 3; i++)
	{
		model[i]->Update();
	}

	if (Keyboard::Get()->Press(VK_LEFT))
	{
		D3DXVECTOR3 vec;
		model[0]->Position(&vec);

		vec.x += 10 * Time::Delta();

		model[0]->Position(vec);
	}
}

void TestModel::PreRender()
{

}

void TestModel::Render()
{
	//for (int i = 0; i < 4; i++)
	//{
	//	model[i]->Render();
	//}
}

void TestModel::PostRender()
{
}

void TestModel::ResizeScreen()
{
}

void TestModel::TestImgui()
{
}
