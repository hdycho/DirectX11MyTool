#pragma once

class CsResource
{
public:
	static void CreateRawBuffer(UINT size, void*initData, ID3D11Buffer**buffer);
	static void CreateStructuredBuffer(UINT size, UINT count, void*initData, ID3D11Buffer**buffer);
	static void CreateSRV(ID3D11Buffer*buffer, ID3D11ShaderResourceView**srv);
	static void CreateSRV(ID3D11Texture2D*resource, ID3D11ShaderResourceView**srv);
	static void CreateUAV(ID3D11Buffer*buffer, ID3D11UnorderedAccessView**uav);
	static ID3D11Buffer* CreateAndCopyBuffer(ID3D11Buffer*src);
	static void CreateStructuredBuffer(UINT size, UINT count, void * initData, ID3D11Buffer ** buffer, bool cpuWrite, bool gpuWrite);
	static void CreateTexture(UINT width, UINT height, void * initData, ID3D11Texture2D ** texture, bool cpuWrite = true, bool gpuWrite = true, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	static ID3D11Texture2D*WriteTexture(DXGI_FORMAT writeFormat, ID3D11Texture2D*copyTex);
};