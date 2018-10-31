#include "stdafx.h"
#include "TestMultiTextures.h"
#include "../Objects/MeshQuad.h"

TestMultiTextures::TestMultiTextures(ExecuteValues * values)
	: Execute(values)
{
	quad = new MeshQuad(values);
	shader = new Shader(Shaders + L"041_MultiTexture.hlsl");
	quad->SetShader(shader);

	//멀티텍스쳐 : 두장의 이미지가 동시에 들어감
	quad->SetDiffuseMap(Textures + L"Floor.png");
	quad->SetNormalMap(Textures + L"Dirt.png");

	buffer = new Buffer();
}

TestMultiTextures::~TestMultiTextures()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(quad);
	SAFE_DELETE(buffer);
}

void TestMultiTextures::Update()
{
	quad->Update();
}

void TestMultiTextures::PreRender()
{

}

void TestMultiTextures::Render()
{
	buffer->SetPSBuffer(10);
	ImGui::SliderInt("MultiTexture", &buffer->Data.Selected, -1, 11);
	quad->Render();
}

void TestMultiTextures::PostRender()
{

}

void TestMultiTextures::ResizeScreen()
{
}
