#include "stdafx.h"
#include "Material.h"

Material::Material()
	:shader(NULL)
	, diffuseMap(NULL)
	, bShaderDelete(false)
	, specularMap(NULL)
	, normalMap(NULL)
	, detailMap(NULL)
{
	buffer = new Buffer();
}

Material::Material(wstring shaderFile)
	:diffuseMap(NULL),specularMap(NULL), normalMap(NULL),detailMap(NULL)
{
	assert(shaderFile.length() > 0);
	
	buffer = new Buffer();
	bShaderDelete = true;
	shader = new Shader(shaderFile);
}

Material::~Material()
{
	if (bShaderDelete == true)
		SAFE_DELETE(shader);

	SAFE_DELETE(diffuseMap);
	SAFE_DELETE(specularMap);
	SAFE_DELETE(normalMap);
}

void Material::SetShader(wstring file)
{
	//���ο��� ��������� ���ο����������Ѵ�
	if (bShaderDelete == true)
		SAFE_DELETE(shader);

	bShaderDelete = false;

	//�����̸� �ް� ���ο��� ���̴����λ����Ѵ�
	if (file.length() > 0)
	{
		shader = new Shader(file);
		bShaderDelete = true;
	}
}

void Material::SetShader(string file)
{
	SetShader(String::ToWString(file));
}

void Material::SetShader(Shader * shader)
{
	if (bShaderDelete == true)
		SAFE_DELETE(this->shader);

	//�ܺο��� �޾ƿ��⸸ �ϴϱ� �����ȵȴ�
	//�ܺο��� ���쵵������
	this->shader = shader;
	bShaderDelete = false;
}

void Material::SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDiffuseMap(String::ToWString(file), info);
}

void Material::SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if (diffuseMap != NULL)
		SAFE_DELETE(diffuseMap);
	
	diffuseMap = new Texture(file, info);
}

void Material::SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetSpecularMap(String::ToWString(file), info);
}

void Material::SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if(specularMap!=NULL)
		SAFE_DELETE(specularMap);

	specularMap = new Texture(file, info);
}

void Material::SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetNormalMap(String::ToWString(file), info);
}

void Material::SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if (normalMap != NULL)
		SAFE_DELETE(normalMap);

	normalMap = new Texture(file, info);
}

void Material::SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO * info)
{
	SetDetailMap(String::ToWString(file), info);
}

void Material::SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO * info)
{
	if (detailMap != NULL)
		SAFE_DELETE(detailMap);

	detailMap = new Texture(file, info);
}

void Material::PSSetBuffer()
{
	if (shader != NULL)
		shader->Render();

	UINT slot = 0;
	if (diffuseMap != NULL)
	{
		diffuseMap->SetShaderResource(slot);
		diffuseMap->SetShaderSampler(slot);
	}
	else
	{
		Texture::SetBlankShaderResource(slot);
		Texture::SetBlankSamplerState(slot);
	}

	UINT slot1 = 1;
	if (specularMap != NULL)
	{
		specularMap->SetShaderResource(slot1);
		specularMap->SetShaderSampler(slot1);
	}
	else
	{
		Texture::SetBlankShaderResource(slot1);
		Texture::SetBlankSamplerState(slot1);
	}

	UINT slot2 = 2;
	if (normalMap != NULL)
	{
		normalMap->SetShaderResource(slot2);
		normalMap->SetShaderSampler(slot2);
	}
	else
	{
		Texture::SetBlankShaderResource(slot2);
		Texture::SetBlankSamplerState(slot2);
	}

	UINT slot3 = 3;
	if (detailMap != NULL)
	{
		detailMap->SetShaderResource(slot3);
		detailMap->SetShaderSampler(slot3);
	}
	else
	{
		Texture::SetBlankShaderResource(slot3);
		Texture::SetBlankSamplerState(slot3);
	}

	buffer->SetPSBuffer(1);
}

void Material::Clone(void ** clone)
{
	Material*material = new Material();
	material->name = this->name;

	//�����ڱ⽦�̴��� �ƴ϶� �ܺο��� �޾ƿ½��̴���� �����ϸ� �ȵ�
	//�� ó���� �ٽ��ؾ���
	if (this->shader != NULL)
	{
		if (bShaderDelete)
			material->SetShader(this->shader->GetFile());
		else
			material->SetShader(this->shader);
	}
	
	material->SetDiffuse(*this->GetDiffuse());

	if (this->diffuseMap != NULL)
		material->SetDiffuseMap(this->diffuseMap->GetFile());

	if(this->specularMap!=NULL)
		material->SetSpecularMap(this->specularMap->GetFile());

	material->SetShininess(*this->GetShininess());

	if (this->normalMap != NULL)
		material->SetNormalMap(this->normalMap->GetFile());

	*clone = material;
}
