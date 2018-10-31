#include "stdafx.h"
#include "SamplerState.h"

SamplerState::SamplerState()
	: state(NULL)
{
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));

	//이미지크기와 버텍스크기 사이에서 사이즈를 맞추기위해
	//확대할것인지 축소할것인지 거리에따라 바꿀때
	//LINEAR(선형) 필터를 사용하겠다는 뜻
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	//UV가 1보다클때 나머지공간을 채우는방법
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	//비등방성 필터링 
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	Changed();
}

SamplerState::~SamplerState()
{
	SAFE_RELEASE(state);
}

void SamplerState::PSSetSamplers(UINT slot)
{
	D3D::GetDC()->PSSetSamplers(slot, 1, &state);
}

void SamplerState::Filter(D3D11_FILTER val)
{
	desc.Filter = val;

	Changed();
}

void SamplerState::AddressU(D3D11_TEXTURE_ADDRESS_MODE val)
{
	desc.AddressU = val;

	Changed();
}

void SamplerState::AddressV(D3D11_TEXTURE_ADDRESS_MODE val)
{
	desc.AddressV = val;

	Changed();
}

void SamplerState::AddressW(D3D11_TEXTURE_ADDRESS_MODE val)
{
	desc.AddressW = val;

	Changed();
}

void SamplerState::ComparisonFunc(D3D11_COMPARISON_FUNC val)
{
	desc.ComparisonFunc = val;

	Changed();
}

void SamplerState::Changed()
{
	SAFE_RELEASE(state);

	HRESULT hr;

	hr = D3D::GetDevice()->CreateSamplerState(&desc, &state);
	assert(SUCCEEDED(hr));
}