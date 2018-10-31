#include "stdafx.h"
#include "Texture.h"

using namespace DirectX;

vector<TextureDesc> Textures::descs;

Texture::Texture(const Texture&texture)
{
	file = texture.file;
	Textures::Load(this, NULL);
	samplerState = new SamplerState();
}

Texture::Texture(wstring file, D3DX11_IMAGE_LOAD_INFO* loadInfo)
	: file(file)
{
	Textures::Load(this, loadInfo);

	samplerState = new SamplerState();
}

Texture::~Texture()
{
	SAFE_DELETE(samplerState);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixels(DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels)
{
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	return ReadPixels(srcTexture, readFormat, pixels);
}

void Texture::WritePixels(DXGI_FORMAT writeFormat, vector<D3DXCOLOR>& pixels, bool isInteger)
{
#if false
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	WritePixels(srcTexture, writeFormat, pixels, isInteger);
#else
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	D3D11_TEXTURE2D_DESC writeTexturedesc;
	ZeroMemory(&writeTexturedesc, sizeof(D3D11_TEXTURE2D_DESC));
	writeTexturedesc.Width = metaData.width;
	writeTexturedesc.Height = metaData.height;
	writeTexturedesc.MipLevels = metaData.mipLevels;
	writeTexturedesc.ArraySize = metaData.arraySize;
	writeTexturedesc.Format = writeFormat;
	writeTexturedesc.SampleDesc.Count = 1;
	writeTexturedesc.Usage = D3D11_USAGE_STAGING;
	writeTexturedesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	writeTexturedesc.MiscFlags = metaData.miscFlags;

	HRESULT hr;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&writeTexturedesc, NULL, &texture);
	assert(SUCCEEDED(hr));

	D3D11_MAPPED_SUBRESOURCE map;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_WRITE, NULL, &map);
	{
		for (UINT y = 0; y < writeTexturedesc.Height; y++)
		{
			for (UINT x = 0; x < writeTexturedesc.Width; x++)
			{
				UINT index = writeTexturedesc.Width * y + x;

				UINT val = 0;
				if (isInteger)
				{
					UINT r = ((UINT)(pixels[index].r) & 0x000000FF) << 0;
					UINT g = ((UINT)(pixels[index].g) & 0x000000FF) << 8;
					UINT b = ((UINT)(pixels[index].b) & 0x000000FF) << 16;
					UINT a = ((UINT)(pixels[index].a) & 0x000000FF) << 24;
					*((UINT*)(map.pData) + index) = r + g + b + a;
				}
				else
				{
#if true
					UINT r = ((UINT)(255 * pixels[index].r) & 0x000000FF) << 0;
					UINT g = ((UINT)(255 * pixels[index].g) & 0x000000FF) << 8;
					UINT b = ((UINT)(255 * pixels[index].b) & 0x000000FF) << 16;
					UINT a = ((UINT)(255 * pixels[index].a) & 0x000000FF) << 24;
					*((UINT*)(map.pData) + index) = r + g + b + a;
#else
					*((UINT*)(map.pData) + index) =
						((UINT)(255.0f * pixels[index].a) << 24)
						+ ((UINT)(255.0f * pixels[index].b) << 16)
						+ ((UINT)(255.0f * pixels[index].g) << 8)
						+ ((UINT)(255.0f * pixels[index].r) << 0);
#endif
				}
			}
		}
	}
	D3D::GetDC()->Unmap(texture, 0);

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), texture, NULL, srcTexture);
	assert(SUCCEEDED(hr));
#endif
}

void Texture::SaveFile(wstring file)
{
	ID3D11Texture2D* srcTexture;
	view->GetResource((ID3D11Resource **)&srcTexture);

	SaveFile(file, srcTexture);
}

void Texture::SetSamplerState(class SamplerState*state)
{
	SAFE_DELETE(samplerState);

	samplerState = state;
}

void Texture::SetShaderResource(UINT slot)
{
	D3D::GetDC()->PSSetShaderResources(slot, 1, &view);
}

void Texture::SetShaderSampler(UINT slot)
{
	samplerState->PSSetSamplers(slot);
}

void Texture::SetShaderResources(UINT slot, UINT count, Texture ** textures)
{
	vector<ID3D11ShaderResourceView *> views;
	for (UINT i = 0; i < count; i++)
		views.push_back(textures[i]->GetView());

	D3D::GetDC()->PSSetShaderResources(slot, count, &views[0]);
}

void Texture::SetBlankShaderResource(UINT slot)
{
	ID3D11ShaderResourceView* null[1]{ NULL };

	D3D::GetDC()->PSSetShaderResources(slot, 1, null);
}

void Texture::SetBlankSamplerState(UINT slot)
{
	ID3D11SamplerState* null[1]{ NULL };

	D3D::GetDC()->PSSetSamplers(slot, 1, null);
}

void Texture::SaveFile(wstring file, ID3D11Texture2D * src)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);

	ID3D11Texture2D* dest;
	D3D11_TEXTURE2D_DESC destDesc;
	ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
	destDesc.Width = srcDesc.Width;
	destDesc.Height = srcDesc.Height;
	destDesc.MipLevels = 1;
	destDesc.ArraySize = 1;
	destDesc.Format = srcDesc.Format;
	destDesc.SampleDesc = srcDesc.SampleDesc;
	destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	destDesc.Usage = D3D11_USAGE_STAGING;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&destDesc, NULL, &dest);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), src, NULL, dest);
	assert(SUCCEEDED(hr));

	hr = D3DX11SaveTextureToFile(D3D::GetDC(), dest, D3DX11_IFF_PNG, file.c_str());
	assert(SUCCEEDED(hr));

	SAFE_RELEASE(dest);
}

D3D11_TEXTURE2D_DESC Texture::ReadPixels(ID3D11Texture2D * src, DXGI_FORMAT readFormat, vector<D3DXCOLOR>* pixels)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = srcDesc.Width;
	desc.Height = srcDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = readFormat;
	desc.SampleDesc = srcDesc.SampleDesc;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;


	HRESULT hr;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), src, NULL, texture);
	assert(SUCCEEDED(hr));


	D3D11_MAPPED_SUBRESOURCE map;
	UINT* colors = new UINT[desc.Width * desc.Height];
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, NULL, &map);
	{
		memcpy(colors, map.pData, sizeof(UINT) * desc.Width * desc.Height);
	}
	D3D::GetDC()->Unmap(texture, 0);


	pixels->reserve(desc.Width * desc.Height);
	for (UINT y = 0; y < desc.Height; y++)
	{
		for (UINT x = 0; x < desc.Width; x++)
		{
			UINT index = desc.Width * y + x;

			CONST FLOAT f = 1.0f / 255.0f;
			float r = f * (float)((0xFF000000 & colors[index]) >> 24);
			float g = f * (float)((0x00FF0000 & colors[index]) >> 16);
			float b = f * (float)((0x0000FF00 & colors[index]) >> 8);
			float a = f * (float)((0x000000FF & colors[index]) >> 0);

			pixels->push_back(D3DXCOLOR(a, b, g, r));
		}
	}

	SAFE_DELETE_ARRAY(colors);
	SAFE_RELEASE(src);
	SAFE_RELEASE(texture);

	return desc;
}

void Textures::Create()
{

}

void Textures::Delete()
{
	for (TextureDesc desc : descs)
		SAFE_RELEASE(desc.view);
}

void Textures::Load(Texture * texture, D3DX11_IMAGE_LOAD_INFO * loadInfo)
{
	HRESULT hr;

	TexMetadata metaData;
	wstring ext = Path::GetExtension(texture->file);
	if (ext == L"tga")
	{
		hr = GetMetadataFromTGAFile(texture->file.c_str(), metaData);
		assert(SUCCEEDED(hr));
	}
	else if (ext == L"dds")
	{
		hr = GetMetadataFromDDSFile(texture->file.c_str(), DDS_FLAGS_NONE, metaData);
		assert(SUCCEEDED(hr));
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else
	{
		hr = GetMetadataFromWICFile(texture->file.c_str(), WIC_FLAGS_NONE, metaData);
		assert(SUCCEEDED(hr));
	}

	UINT width = metaData.width;
	UINT height = metaData.height;

	if (loadInfo != NULL)
	{
		width = loadInfo->Width;
		height = loadInfo->Height;

		metaData.width = loadInfo->Width;
		metaData.height = loadInfo->Height;
	}


	TextureDesc desc;
	desc.file = texture->file;
	desc.width = width;
	desc.height = height;

	TextureDesc exist;
	bool bExist = false;
	for (TextureDesc temp : descs)
	{
		if (desc == temp)
		{
			bExist = true;
			exist = temp;

			break;
		}
	}

	if (bExist == true)
	{
		texture->metaData = exist.metaData;
		texture->view = exist.view;
	}
	else
	{
		ScratchImage image;
		if (ext == L"tga")
		{
			hr = LoadFromTGAFile(texture->file.c_str(), &metaData, image);
			assert(SUCCEEDED(hr));
		}
		else if (ext == L"dds")
		{
			hr = LoadFromDDSFile(texture->file.c_str(), DDS_FLAGS_NONE, &metaData, image);
			assert(SUCCEEDED(hr));
		}
		else if (ext == L"hdr")
		{
			assert(false);
		}
		else
		{
			hr = LoadFromWICFile(texture->file.c_str(), WIC_FLAGS_NONE, &metaData, image);
			assert(SUCCEEDED(hr));
		}

		ID3D11ShaderResourceView* view;

		hr = DirectX::CreateShaderResourceView(D3D::GetDevice(), image.GetImages(), image.GetImageCount(), metaData, &view);
		assert(SUCCEEDED(hr));

		desc.file = texture->file;
		desc.width = metaData.width;
		desc.height = metaData.height;
		desc.view = view;

		texture->view = view;
		texture->metaData = metaData;

		descs.push_back(desc);
	}
}