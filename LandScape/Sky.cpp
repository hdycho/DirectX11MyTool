#include "stdafx.h"
#include "Sky.h"

Sky::Sky(ExecuteValues * values)
	: values(values)
	, realTime(false), theta(0), phi(0)
	, radius(10), slices(32), stacks(16), timeFactor(0)
{
	mieTarget = new RenderTarget(values, 128, 64);
	rayLeighTarget = new RenderTarget(values, 128, 64);

	scatterShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Scattering", "PS_Scattering");
	targetShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Target", "PS_Target");
	moonShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Moon", "PS_Moon");
	cloudShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Cloud", "PS_Cloud");

	worldBuffer = new WorldBuffer();
	targetBuffer = new TargetBuffer();
	scatterBuffer = new PS_ScatterBuffer();
	cloudBuffer = new VS_CloudBuffer();

	GenerateSphere();
	GenerateQuad();
	GenerateMoon();
	GenerateCloud();

	depthState[0] = new DepthStencilState();
	depthState[1] = new DepthStencilState();
	depthState[1]->DepthEnable(false);
	depthState[1]->DepthWriteEnable(D3D11_DEPTH_WRITE_MASK_ZERO);

	rayleigh2D = new Render2D(values);
	rayleigh2D->Position(0, 100);
	rayleigh2D->Scale(200, 100);

	mie2D = new Render2D(values);
	mie2D->Position(0, 0);
	mie2D->Scale(200, 100);

	starField = new Texture(Textures + L"starfield.png");
	moonTex = new Texture(Textures + L"moon.tga");
	glowTex = new Texture(Textures + L"moonglow.tga");

	rayleighSampler = new SamplerState();
	rayleighSampler->AddressU(D3D11_TEXTURE_ADDRESS_CLAMP);
	rayleighSampler->AddressV(D3D11_TEXTURE_ADDRESS_CLAMP);
	rayleighSampler->AddressW(D3D11_TEXTURE_ADDRESS_CLAMP);
	rayleighSampler->Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	mieSampler = new SamplerState();
	mieSampler->AddressU(D3D11_TEXTURE_ADDRESS_CLAMP);
	mieSampler->AddressV(D3D11_TEXTURE_ADDRESS_CLAMP);
	mieSampler->AddressW(D3D11_TEXTURE_ADDRESS_CLAMP);
	mieSampler->Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);


	starSampler = new SamplerState();
	starSampler->AddressU(D3D11_TEXTURE_ADDRESS_CLAMP);
	starSampler->AddressV(D3D11_TEXTURE_ADDRESS_CLAMP);
	starSampler->AddressW(D3D11_TEXTURE_ADDRESS_CLAMP);
	starSampler->Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);


	moonSampler = new SamplerState();
	moonSampler->AddressU(D3D11_TEXTURE_ADDRESS_WRAP);
	moonSampler->AddressV(D3D11_TEXTURE_ADDRESS_WRAP);
	moonSampler->Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	cloudSampler = new SamplerState();
	cloudSampler->AddressU(D3D11_TEXTURE_ADDRESS_WRAP);
	cloudSampler->AddressV(D3D11_TEXTURE_ADDRESS_WRAP);
	cloudSampler->Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	moonBlend[0] = new BlendState();
	moonBlend[1] = new BlendState();
	moonBlend[1]->BlendEnable(true);
	moonBlend[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	moonBlend[1]->DestBlend(D3D11_BLEND_INV_SRC_ALPHA);


	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->CullMode(D3D11_CULL_NONE);
}

Sky::~Sky()
{
	SAFE_DELETE(depthState[0]);
	SAFE_DELETE(depthState[1]);
	SAFE_DELETE(moonBlend[0]);
	SAFE_DELETE(moonBlend[1]);

	SAFE_DELETE_ARRAY(quadVertices);
	SAFE_RELEASE(quadBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(targetBuffer);
	SAFE_DELETE(scatterBuffer);
	SAFE_DELETE(cloudBuffer);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_RELEASE(moonVBuffer);
	SAFE_RELEASE(moonIBuffer);

	SAFE_DELETE(scatterShader);
	SAFE_DELETE(targetShader);
	SAFE_DELETE(moonShader);

	SAFE_DELETE(mieTarget);
	SAFE_DELETE(rayLeighTarget);

	SAFE_DELETE(rayleighSampler);
	SAFE_DELETE(mieSampler);
	SAFE_DELETE(starSampler);
	SAFE_DELETE(moonSampler);
	SAFE_DELETE(cloudSampler);
}

void Sky::Update()
{
	D3DXMATRIX V;
	D3DXVECTOR3 pos;
	values->MainCamera->Position(&pos);
	D3DXMatrixTranslation(&V, pos.x, pos.y, pos.z);

	theta += Time::Delta() * timeFactor;

	if (theta > Math::PI * 2.0f) theta -= Math::PI * 2.0f;


	//theta에 따라 빛의 방향이 변하는데 쉐이더에서 처리
	values->GlobalLight->Data.Direction = GetDirection();

	scatterBuffer->Data.SunColor = GetSunColor(-theta, 2);

	worldBuffer->SetMatrix(V);
}

void Sky::PreRender()
{
	if (prevTheta == theta && prevPhi == phi) return;

	//멀티렌더타겟 사용
	mieTarget->Set();
	rayLeighTarget->Set();

	ID3D11RenderTargetView * rtvs[2];
	rtvs[0] = rayLeighTarget->GetRTV();
	rtvs[1] = mieTarget->GetRTV();

	//멀티 안됨, 통일 시켜야함
	ID3D11DepthStencilView * dsv;
	dsv = rayLeighTarget->GetDSV();

	D3D::Get()->SetRenderTargets(2, rtvs, dsv);

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	targetBuffer->SetPSBuffer(11);
	targetShader->Render();

	D3D::GetDC()->Draw(6, 0);
}

void Sky::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	scatterShader->Render();
	worldBuffer->SetVSBuffer(1);

	//==위에서 레일리 미 계산된 쉐이더리소스뷰 쉐이더에 연결==//
	ID3D11ShaderResourceView * rSRV = rayLeighTarget->GetSRV();
	D3D::GetDC()->PSSetShaderResources(10, 1, &rSRV);

	ID3D11ShaderResourceView * mSRV = mieTarget->GetSRV();
	D3D::GetDC()->PSSetShaderResources(11, 1, &mSRV);
	////////////////////////////////////////////////////////
	starField->SetShaderResource(12);


	rayleighSampler->PSSetSamplers(10);
	mieSampler->PSSetSamplers(11);
	starSampler->PSSetSamplers(12);

	scatterBuffer->Data.StarIntensity = GetStarIntensity();
	scatterBuffer->SetPSBuffer(10);


	depthState[1]->OMSetDepthStencilState();
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);

	moonBlend[1]->OMSetBlendState();

	DrawMoon();
	DrawGlow();
	DrawCloud();

	moonBlend[0]->OMSetBlendState();
	depthState[0]->OMSetDepthStencilState();

	rayleigh2D->SRV(rayLeighTarget->GetSRV());
	rayleigh2D->Update();
	rayleigh2D->Render();

	mie2D->SRV(mieTarget->GetSRV());
	mie2D->Update();
	mie2D->Render();

	ImGui::SliderFloat("Theta", &theta, 0.0f, Math::PI * 2.0f, "%.5f");
}

void Sky::GenerateSphere()
{
	UINT domeCount = 32;
	UINT latitude = domeCount / 2; // 위도
	UINT longitude = domeCount; // 경도

	vertexCount = longitude * latitude * 2;
	indexCount = (longitude - 1) * (latitude - 1) * 2 * 8;

	VertexTexture* vertices = new VertexTexture[vertexCount];

	UINT index = 0;
	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * Math::PI / 180.0f;

		for (UINT j = 0; j < latitude; j++)
		{
			float y = Math::PI * j / (latitude - 1);

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);
			vertices[index].Position *= 10.0f; // 크기를 키우려고 임의의 값 곱한거

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		} // for(j)
	}  // for(i)

	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * Math::PI / 180.0f;

		for (UINT j = 0; j < latitude; j++)
		{
			float y = (Math::PI * 2.0f) - (Math::PI * j / (latitude - 1));

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);
			vertices[index].Position *= 10.0f; // 크기를 키우려고 임의의 값 곱한거

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		} // for(j)
	}  // for(i)


	index = 0;
	UINT* indices = new UINT[indexCount * 3];

	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = i * latitude + j;
			indices[index++] = (i + 1) * latitude + j;
			indices[index++] = (i + 1) * latitude + (j + 1);

			indices[index++] = (i + 1) * latitude + (j + 1);
			indices[index++] = i * latitude + (j + 1);
			indices[index++] = i * latitude + j;
		}
	}

	UINT offset = latitude * longitude;
	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = offset + i * latitude + j;
			indices[index++] = offset + (i + 1) * latitude + (j + 1);
			indices[index++] = offset + (i + 1) * latitude + j;

			indices[index++] = offset + i * latitude + (j + 1);
			indices[index++] = offset + (i + 1) * latitude + (j + 1);
			indices[index++] = offset + i * latitude + j;
		}
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);
}

void Sky::GenerateQuad()
{
	quadVertices = new VertexTexture[6];

	quadVertices[0].Position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);   //   0
	quadVertices[1].Position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);   //   1
	quadVertices[2].Position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);   //   2
	quadVertices[3].Position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);   //   2
	quadVertices[4].Position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);   //   1
	quadVertices[5].Position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);   //   3

	quadVertices[0].Uv = D3DXVECTOR2(0, 1);   //   0
	quadVertices[1].Uv = D3DXVECTOR2(0, 0);   //   1
	quadVertices[2].Uv = D3DXVECTOR2(1, 1);   //   2
	quadVertices[3].Uv = D3DXVECTOR2(1, 1);   //   2
	quadVertices[4].Uv = D3DXVECTOR2(0, 0);   //   1
	quadVertices[5].Uv = D3DXVECTOR2(1, 0);   //   3

											  //   CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = quadVertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Sky::GenerateMoon()
{
	VertexTexture * moonVert = new VertexTexture[4];

	moonVert[0].Position = D3DXVECTOR3(1, -1, 0);
	moonVert[1].Position = D3DXVECTOR3(-1, -1, 0);
	moonVert[2].Position = D3DXVECTOR3(-1, 1, 0);
	moonVert[3].Position = D3DXVECTOR3(1, 1, 0);

	moonVert[0].Uv = D3DXVECTOR2(1, 1);
	moonVert[1].Uv = D3DXVECTOR2(0, 1);
	moonVert[2].Uv = D3DXVECTOR2(0, 0);
	moonVert[3].Uv = D3DXVECTOR2(1, 0);

	UINT * moonIndices = new UINT[6]{ 0, 1, 2, 2, 3, 0 };

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = moonVert;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &moonVBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = moonIndices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &moonIBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Sky::GenerateCloud()
{
	int perm[] = { 151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	int gradValues[] = {
		1,1,0,
		-1,1,0, 1,-1,0,
		-1,-1,0, 1,0,1,
		-1,0,1, 1,0,-1,
		-1,0,-1, 0,1,1,
		0,-1,1, 0,1,-1,
		0,-1,-1, 1,1,0,
		0,-1,1, -1,1,0,
		0,-1,-1
	};
	permTex = new Texture(Textures + L"CloudMap.png");

	vector<D3DXCOLOR> pixels;
	for (int i = 0; i<256; i++)
	{
		for (int j = 0; j<256; j++)
		{
			//int offset = (i * 256 + j);
			int value = perm[(j + perm[i]) & 0xFF];
			D3DXCOLOR color;
			color.r = (float)(gradValues[value & 0x0F] * 64 + 64);
			color.g = (float)(gradValues[value & 0x0F + 1] * 64 + 64);
			color.b = (float)(gradValues[value & 0x0F + 2] * 64 + 64);
			color.a = (float)value;
			pixels.push_back(color);
		}
	}
	permTex->WritePixels(DXGI_FORMAT_R8G8B8A8_UNORM, pixels, true);
}

float Sky::GetStarIntensity()
{
	return values->GlobalLight->Data.Direction.y;
}

void Sky::DrawMoon()
{
	D3DXMATRIX R;
	values->MainCamera->Matrix(&R);
	D3DXMatrixInverse(&R, NULL, &R);

	float x = asin(-R._32);
	float y = atan2(R._31, R._33);
	float z = atan2(R._12, R._22);

	D3DXVECTOR3 rotation(x, y, z);

	D3DXVECTOR3 position;
	position.x = values->GlobalLight->Data.Direction.x * 150;
	position.y = values->GlobalLight->Data.Direction.y * 150;
	position.z = values->GlobalLight->Data.Direction.z * 150;

	D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);

	D3DXMATRIX S, RX, RY, RZ, T1, T2;
	D3DXMatrixScaling(&S, 20, 20, 20);
	D3DXMatrixRotationX(&RX, rotation.x);
	D3DXMatrixRotationY(&RY, rotation.y);
	D3DXMatrixRotationZ(&RZ, rotation.z);
	D3DXMatrixTranslation(&T1, position.x, position.y, position.z);
	D3DXMatrixTranslation(&T2, camPos.x, camPos.y, camPos.z);

	D3DXMATRIX world = S*RX * RY * RZ * T1 * T2;

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &moonVBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(moonIBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	moonShader->Render();

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	moonTex->SetShaderResource(13);
	moonSampler->PSSetSamplers(13);

	D3D::GetDC()->DrawIndexed(6, 0, 0);
}

void Sky::DrawGlow()
{
	D3DXMATRIX R;
	values->MainCamera->Matrix(&R);
	D3DXMatrixInverse(&R, NULL, &R);

	float x = asin(-R._32);
	float y = atan2(R._31, R._33);
	float z = atan2(R._12, R._22);

	D3DXVECTOR3 rotation(x, y, z);

	D3DXVECTOR3 position;
	position.x = values->GlobalLight->Data.Direction.x * 50;
	position.y = values->GlobalLight->Data.Direction.y * 50;
	position.z = values->GlobalLight->Data.Direction.z * 50;

	D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);

	D3DXMATRIX S, RX, RY, RZ, T1, T2;
	D3DXMatrixScaling(&S, 20, 20, 20);
	D3DXMatrixRotationX(&RX, rotation.x);
	D3DXMatrixRotationY(&RY, rotation.y);
	D3DXMatrixRotationZ(&RZ, rotation.z);
	D3DXMatrixTranslation(&T1, position.x, position.y, position.z);
	D3DXMatrixTranslation(&T2, camPos.x, camPos.y, camPos.z);

	D3DXMATRIX world = S*RX * RY * RZ * T1 * T2;

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &moonVBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(moonIBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	moonShader->Render();

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	glowTex->SetShaderResource(13);
	moonSampler->PSSetSamplers(13);

	D3D::GetDC()->DrawIndexed(6, 0, 0);
}

void Sky::DrawCloud()
{
	rasterizerState[1]->RSSetState();
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXVECTOR3 camPos;
	D3DXMATRIX sc, rX, rY, dist, cam, matF;

	values->MainCamera->Position(&camPos);

	D3DXMatrixScaling(&sc, 900, 900, 1);
	D3DXMatrixTranslation(&dist, 0, 0, -200.0f);
	D3DXMatrixRotationX(&rX, Math::PI * 0.5f);
	D3DXMatrixTranslation(&cam, camPos.x, camPos.y, camPos.z);
	matF = sc * dist * rX * cam;

	worldBuffer->SetMatrix(matF);
	worldBuffer->SetVSBuffer(1);

	cloudBuffer->Data.NumTiles = 16.25f;
	cloudBuffer->SetVSBuffer(10);

	scatterBuffer->Data.StarIntensity = 16.25f;
	scatterBuffer->Data.Running = Time::Get()->Running() / 8.0f;
	scatterBuffer->SetPSBuffer(10);

	permTex->SetShaderResource(14);
	moonSampler->PSSetSamplers(14);

	cloudShader->Render();
	D3D::GetDC()->Draw(6, 0);
	rasterizerState[0]->RSSetState();

	ImGui::DragFloat("Time", &timeFactor, 0.001f, -1.0f, 1.0f);
	ImGui::DragFloat("Sun Angle", &phi, 1, 0, 90);
}

D3DXVECTOR3 Sky::GetDirection()
{
	float y = (float)cosf(theta);
	float x = (float)(sinf(theta));
	float z = (float)(sinf(Math::ToRadian(phi)));

	return D3DXVECTOR3(x, y, z);
}

D3DXCOLOR Sky::GetSunColor(float fTheta, int nTurbidity)
{
	float fBeta = 0.04608365822050f * nTurbidity - 0.04586025928522f;
	float fTauR, fTauA;
	float*fTau = new float[3];

	float coseno = (float)cosf((double)fTheta + Math::PI);
	double factor = (double)fTheta / Math::PI * 180.0;
	double jarl = pow(93.885 - factor, -1.253);
	float potencia = (float)jarl;
	float m = 1.0f / (coseno + 0.15f * potencia);

	int i;
	float*fLambda = new float[3];
	fLambda[0] = targetBuffer->Data.WaveLength.x;
	fLambda[1] = targetBuffer->Data.WaveLength.y;
	fLambda[2] = targetBuffer->Data.WaveLength.z;


	for (i = 0; i < 3; i++)
	{
		potencia = (float)pow((double)fLambda[i], 4.0);
		fTauR = (float)expf((double)(-m * 0.008735f * potencia));

		const float fAlpha = 1.3f;
		potencia = (float)pow((double)fLambda[i], (double)-fAlpha);
		if (m < 0.0f)
			fTau[i] = 0.0f;
		else
		{
			fTauA = (float)expf((double)(-m * fBeta * potencia));
			fTau[i] = fTauR * fTauA;
		}

	}

	D3DXCOLOR vAttenuation = D3DXCOLOR(fTau[0], fTau[1], fTau[2], 1.0f);
	return vAttenuation;
}
