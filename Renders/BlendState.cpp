#include "stdafx.h"
#include "BlendState.h"

BlendState::BlendState()
	:state(NULL)
{
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

	//섞을때 외곽선 어떻게처리할지
	desc.AlphaToCoverageEnable = false;		

	//멀티랜더타켓을 사용할지 안할지
	//랜더타켓 여러개를 한번에 셋팅하고 한번에 같이 보여줌
	//총 8개를 사용할수 있음 
	//Blend를 독립적으로 사용가능
	desc.IndependentBlendEnable = false;	

	desc.RenderTarget[0].BlendEnable = false;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;

	//blend 연산 옵션
	//Dest(바닥색,먼저그려진(랜더된)위치의 색상) op(연산) Src(그릴색,나중에그려짐)
	//DestColor*DestBlend(1-SRC알파값) + SourceColor*SourceBlend(SRC알파값)
	//Dc*(1-a) + Sc*(a)
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Changed();
}

BlendState::~BlendState()
{
	SAFE_RELEASE(state);
}

void BlendState::AlphaToCoverageEnable(bool val)
{
	desc.AlphaToCoverageEnable = val;
	Changed();
}

void BlendState::BlendEnable(bool val)
{
	desc.RenderTarget[0].BlendEnable = val;
	Changed();
}

void BlendState::OMSetBlendState()
{
	D3D::GetDC()->OMSetBlendState(state, NULL, 0xFF);
}

void BlendState::BlendOp(D3D11_BLEND_OP val)
{
	desc.RenderTarget[0].BlendOp = val;
	Changed();
}

void BlendState::DestBlend(D3D11_BLEND val)
{
	desc.RenderTarget[0].DestBlend = val;
	Changed();
}

void BlendState::SrcBlend(D3D11_BLEND val)
{
	desc.RenderTarget[0].SrcBlend = val;
	Changed();
}

void BlendState::Changed()
{
	SAFE_RELEASE(state);

	HRESULT hr = D3D::GetDevice()->CreateBlendState(&desc, &state);
	assert(SUCCEEDED(hr));
}
