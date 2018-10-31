#include "stdafx.h"
#include "TestAlphaBlend.h"
#include "../Objects/MeshQuad.h"

//   1. IA : Input Assembly( 기초 데이터 세팅 )
//   2. VS : Vertex Shader( Culling:vertex단위 )
//   3. HS : Hul Shading
//   4. DS : Domain Shading
//   5. GS : Geometry Shader
//      - RS : Rasterizer State( 3D 에서 2D 로 변환, Backface Culling )
//      - PS : Pixel Shader( Clipping:pixel단위 )
//      - MR : Multi Rendertarget
//      - OM : Ouput Merger( 출력을 통합, blendState 이 단계에서 이루어짐 )
//   6. SO : Stream Output

//IA -> VS -> HS,DS,GS->RS(3D를 2D로변환),PS,MR,OM
//IA -> VS 사이에 컬링붙음(정점)
//RS -> PS 사이에 클리핑일어남(픽셀)
//RS변환은 뷰포트와 붙어서 변환 끝나고 2D로 변환한다음 PS로 넘어감
//MR(MultiRenderTarget) 
//OM(Output Merge) => 최종적으로 랜더타겟을 섞은다음 백버퍼로출력

TestAlphaBlend::TestAlphaBlend(ExecuteValues * values)
	: Execute(values)
{
	//알파블랜드
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

	//덧셈합성
	blendState[3] = new BlendState();
	blendState[3]->BlendEnable(true);
	blendState[3]->DestBlend(D3D11_BLEND_ONE);
	blendState[3]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendState[3]->BlendOp(D3D11_BLEND_OP_ADD);

	//뺄셈합성
	//TODO: 실제 순서 Sc*Sb+Dc*Db여서 반대
	//D3D11_BLEND_ONE 은 1로취급
	//Sc*Sb-Dc*Db
	blendState[4] = new BlendState();
	blendState[4]->BlendEnable(true);
	blendState[4]->DestBlend(D3D11_BLEND_ONE);
	blendState[4]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendState[4]->BlendOp(D3D11_BLEND_OP_REV_SUBTRACT);

	//곱셈합성
	//Dc*Db(Sc)+Sc*Sb(0)
	//=>Dc*Sc
	blendState[5] = new BlendState();
	blendState[5]->BlendEnable(true);
	blendState[5]->DestBlend(D3D11_BLEND_SRC_COLOR);
	blendState[5]->SrcBlend(D3D11_BLEND_ZERO);
	blendState[5]->BlendOp(D3D11_BLEND_OP_ADD);

	//제곱합성
	//Dc*Db(Dc)+Sc*Sb(0)
	//Dc^2
	blendState[6] = new BlendState();
	blendState[6]->BlendEnable(true);
	blendState[6]->DestBlend(D3D11_BLEND_DEST_COLOR);
	blendState[6]->SrcBlend(D3D11_BLEND_ZERO);
	blendState[6]->BlendOp(D3D11_BLEND_OP_ADD);

	//네거티네이션 포지티브 합성
	blendState[7] = new BlendState();
	blendState[7]->BlendEnable(true);
	blendState[7]->DestBlend(D3D11_BLEND_ZERO);
	blendState[7]->SrcBlend(D3D11_BLEND_INV_DEST_COLOR);
	blendState[7]->BlendOp(D3D11_BLEND_OP_ADD);

	//겹치는 상황에서 사용할것
	//알파블랜드는 멀티텍스쳐와 달리 이미그려진 상황이므로
	//그 상황을 제어하기 위해 사용
	//텍스처 1
	blendState[8] = new BlendState();
	blendState[8]->BlendEnable(true);
	blendState[8]->DestBlend(D3D11_BLEND_ONE);
	blendState[8]->SrcBlend(D3D11_BLEND_ZERO);
	blendState[8]->BlendOp(D3D11_BLEND_OP_ADD);

	//텍스처 2
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
