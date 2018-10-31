#include "stdafx.h"
#include "TestAlphaBlend.h"
#include "../Objects/MeshQuad.h"

//   1. IA : Input Assembly( ���� ������ ���� )
//   2. VS : Vertex Shader( Culling:vertex���� )
//   3. HS : Hul Shading
//   4. DS : Domain Shading
//   5. GS : Geometry Shader
//      - RS : Rasterizer State( 3D ���� 2D �� ��ȯ, Backface Culling )
//      - PS : Pixel Shader( Clipping:pixel���� )
//      - MR : Multi Rendertarget
//      - OM : Ouput Merger( ����� ����, blendState �� �ܰ迡�� �̷���� )
//   6. SO : Stream Output

//IA -> VS -> HS,DS,GS->RS(3D�� 2D�κ�ȯ),PS,MR,OM
//IA -> VS ���̿� �ø�����(����)
//RS -> PS ���̿� Ŭ�����Ͼ(�ȼ�)
//RS��ȯ�� ����Ʈ�� �پ ��ȯ ������ 2D�� ��ȯ�Ѵ��� PS�� �Ѿ
//MR(MultiRenderTarget) 
//OM(Output Merge) => ���������� ����Ÿ���� �������� ����۷����

TestAlphaBlend::TestAlphaBlend(ExecuteValues * values)
	: Execute(values)
{
	//���ĺ���
	quad1 = new MeshQuad(values);
	quad2 = new MeshQuad(values);

	shader = new Shader(Shaders + L"041_BaseTexture.hlsl");
	quad1->SetShader(shader);
	quad1->SetDiffuseMap(Textures + L"Floor.png");

	quad2->SetShader(shader);
	quad2->SetDiffuseMap(Textures + L"Tree.png");
	quad2->SetDiffuseMap(Textures + L"BlendTest.png");
	quad2->Position(0, 0, -1);

	blendState[0] = new BlendState();

	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);

	blendState[2] = new BlendState();
	blendState[2]->BlendEnable(true);
	blendState[2]->AlphaToCoverageEnable(true);

	//�����ռ�
	blendState[3] = new BlendState();
	blendState[3]->BlendEnable(true);
	blendState[3]->DestBlend(D3D11_BLEND_ONE);
	blendState[3]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendState[3]->BlendOp(D3D11_BLEND_OP_ADD);

	//�����ռ�
	//TODO: ���� ���� Sc*Sb+Dc*Db���� �ݴ�
	//D3D11_BLEND_ONE �� 1�����
	//Sc*Sb-Dc*Db
	blendState[4] = new BlendState();
	blendState[4]->BlendEnable(true);
	blendState[4]->DestBlend(D3D11_BLEND_ONE);
	blendState[4]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendState[4]->BlendOp(D3D11_BLEND_OP_REV_SUBTRACT);

	//�����ռ�
	//Dc*Db(Sc)+Sc*Sb(0)
	//=>Dc*Sc
	blendState[5] = new BlendState();
	blendState[5]->BlendEnable(true);
	blendState[5]->DestBlend(D3D11_BLEND_SRC_COLOR);
	blendState[5]->SrcBlend(D3D11_BLEND_ZERO);
	blendState[5]->BlendOp(D3D11_BLEND_OP_ADD);

	//�����ռ�
	//Dc*Db(Dc)+Sc*Sb(0)
	//Dc^2
	blendState[6] = new BlendState();
	blendState[6]->BlendEnable(true);
	blendState[6]->DestBlend(D3D11_BLEND_DEST_COLOR);
	blendState[6]->SrcBlend(D3D11_BLEND_ZERO);
	blendState[6]->BlendOp(D3D11_BLEND_OP_ADD);

	//�װ�Ƽ���̼� ����Ƽ�� �ռ�
	blendState[7] = new BlendState();
	blendState[7]->BlendEnable(true);
	blendState[7]->DestBlend(D3D11_BLEND_ZERO);
	blendState[7]->SrcBlend(D3D11_BLEND_INV_DEST_COLOR);
	blendState[7]->BlendOp(D3D11_BLEND_OP_ADD);

	//��ġ�� ��Ȳ���� ����Ұ�
	//���ĺ���� ��Ƽ�ؽ��Ŀ� �޸� �̹̱׷��� ��Ȳ�̹Ƿ�
	//�� ��Ȳ�� �����ϱ� ���� ���
	//�ؽ�ó 1
	blendState[8] = new BlendState();
	blendState[8]->BlendEnable(true);
	blendState[8]->DestBlend(D3D11_BLEND_ONE);
	blendState[8]->SrcBlend(D3D11_BLEND_ZERO);
	blendState[8]->BlendOp(D3D11_BLEND_OP_ADD);

	//�ؽ�ó 2
	blendState[9] = new BlendState();
	blendState[9]->BlendEnable(true);
	blendState[9]->DestBlend(D3D11_BLEND_ZERO);
	blendState[9]->SrcBlend(D3D11_BLEND_ONE);
	blendState[9]->BlendOp(D3D11_BLEND_OP_ADD);

	buffer = new Buffer();
}

TestAlphaBlend::~TestAlphaBlend()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(quad1);
}

void TestAlphaBlend::Update()
{
	quad1->Update();
	quad2->Update();
}

void TestAlphaBlend::PreRender()
{

}

void TestAlphaBlend::Render()
{
	ImGui::SliderInt("AlphaBlend", &buffer->Data.Selected, 0, 2);

	quad1->Render();

	blendState[buffer->Data.Selected]->OMSetBlendState();
	quad2->Render();


	blendState[0]->OMSetBlendState();
}

void TestAlphaBlend::PostRender()
{

}

void TestAlphaBlend::ResizeScreen()
{
}
