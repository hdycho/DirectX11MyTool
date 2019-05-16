#include "Framework.h"
#include "ModelInstance.h"
#include "Viewer\Frustum.h"

ModelInstance::ModelInstance(Model*model, wstring shaderFile)
	:maxCount(0),model(model)
{
	for (Material*material : model->Materials())
	{
		if (material->GetEffect() == NULL)
		{
			material->SetEffect(shaderFile);
			frustumVars.push_back(material->GetEffect()->AsVector("ClipPlane"));
			renderVars.push_back(material->GetEffect()->Variable("renderState"));
		}
	}

	ZeroMemory(rDesc, sizeof(RenderDesc) * 64);

	frustum = new Frustum(200);
}

ModelInstance::ModelInstance(Model * model, Effect * effect)
	:maxCount(0), model(model)
{
	for (Material*material : model->Materials())
	{
		if (material->GetEffect() == NULL)
		{
			material->SetEffect(effect);
			frustumVars.push_back(material->GetEffect()->AsVector("ClipPlane"));
			renderVars.push_back(material->GetEffect()->Variable("renderState"));
		}
	}

	ZeroMemory(rDesc, sizeof(RenderDesc) * 64);

	frustum = new Frustum(50);
}

ModelInstance::~ModelInstance()
{
	SAFE_DELETE(model);
	SAFE_RELEASE(transTexture);
	SAFE_RELEASE(transSrv);
	SAFE_DELETE(frustum);
}

void ModelInstance::Ready()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	//   128���� ��Ʈ������ ����
	//   �� �ȼ��� 16����Ʈ �̱� ������ �࿭�� 64����Ʈ �̱� ������ 4�� ������
	desc.Width = 128 * 4;
	desc.Height = 128;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &transTexture);
	assert(SUCCEEDED(hr));

	CsResource::CreateSRV(transTexture, &transSrv);

	D3DXMATRIX boneTransforms[128];

	{
		for (UINT i = 0; i < GetModel()->BoneCount(); i++)
		{
			ModelBone * bone = GetModel()->BoneByIndex(i);

			D3DXMATRIX parentTransform;
			D3DXMATRIX global = bone->Global();

			int parentIndex = bone->ParentIndex();
			if (parentIndex < 0)
			{
				//parentTransform = worlds[count];
				D3DXMatrixIdentity(&parentTransform);
			}
			else
				parentTransform = boneTransforms[parentIndex];

			boneTransforms[i] = parentTransform;
			transforms[0][i] = global * boneTransforms[i];
		}

		D3D11_MAPPED_SUBRESOURCE subResource;
		hr = D3D::GetDC()->Map(transTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		assert(SUCCEEDED(hr));
		{
			for (UINT i = 0; i < 64; i++)
			{
				memcpy((void*)((D3DXMATRIX *)subResource.pData + (i * 128)), transforms[0], sizeof(D3DXMATRIX) * 128);
			}
		}
		D3D::GetDC()->Unmap(transTexture, 0);
	}
}

void ModelInstance::AddWorld(D3DXMATRIX & world)
{
	worlds[maxCount] = world;
	rDesc[maxCount].isRender = true;
	maxCount++;

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE subResource;
	for (UINT i = 0; i < GetModel()->MeshCount(); i++)
	{
		ID3D11Buffer* instanceBuffer = GetModel()->MeshByIndex(i)->InstanceBuffer();

		hr = D3D::GetDC()->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		assert(SUCCEEDED(hr));
		{
			memcpy(subResource.pData, worlds, sizeof(D3DXMATRIX) * maxCount);
		}
		D3D::GetDC()->Unmap(instanceBuffer, 0);
	}

	for (Material* material : GetModel()->Materials())
		material->GetEffect()->AsSRV("Transforms")->SetResource(transSrv);
}

void ModelInstance::DeleteWorld(UINT instanceID)
{
	worlds[instanceID] = 
	{
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0
	};

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE subResource;
	for (UINT i = 0; i < GetModel()->MeshCount(); i++)
	{
		ID3D11Buffer* instanceBuffer = GetModel()->MeshByIndex(i)->InstanceBuffer();

		hr = D3D::GetDC()->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		assert(SUCCEEDED(hr));
		{
			memcpy(subResource.pData, worlds, sizeof(D3DXMATRIX) * maxCount);
		}
		D3D::GetDC()->Unmap(instanceBuffer, 0);
	}

	for (Material* material : GetModel()->Materials())
		material->GetEffect()->AsSRV("Transforms")->SetResource(transSrv);

	maxCount--;
}

Model * ModelInstance::GetModel()
{
	return model;
}

void ModelInstance::SetRenderState(int instNum, bool state)
{
	rDesc[instNum].isRender = state;
}

void ModelInstance::UpdateWorld(UINT instanceNumber, D3DXMATRIX&world)
{
	worlds[instanceNumber] = world;

	//HRESULT hr;
	//D3D11_MAPPED_SUBRESOURCE subResource;
	//for (UINT i = 0; i < model->MeshCount(); i++)
	//{
	//	ID3D11Buffer* instanceBuffer = model->MeshByIndex(i)->InstanceBuffer();

	//	hr = D3D::GetDC()->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	//	assert(SUCCEEDED(hr));
	//	{
	//		//�Ѹ�Ʈ������ �� 64����Ʈ => �Ѹ��� �������ִ°���
	//		//ù��°��Ʈ������ 64*�ν��Ͻ���ȣ(0)=0

	//		void * p = ((D3DXMATRIX*)subResource.pData) + instanceNumber;

	//		memcpy(p, worlds+(instanceNumber), sizeof(D3DXMATRIX));
	//	}
	//	D3D::GetDC()->Unmap(instanceBuffer, 0);
	//}

	//for (Material* material : model->Materials())
	//	material->GetEffect()->AsSRV("Transforms")->SetResource(transSrv);

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE subResource;
	for (UINT i = 0; i < GetModel()->MeshCount(); i++)
	{
		ID3D11Buffer* instanceBuffer = GetModel()->MeshByIndex(i)->InstanceBuffer();

		hr = D3D::GetDC()->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		assert(SUCCEEDED(hr));
		{
			memcpy(subResource.pData, worlds, sizeof(D3DXMATRIX) * maxCount);
		}
		D3D::GetDC()->Unmap(instanceBuffer, 0);
	}

	for (Material* material : GetModel()->Materials())
		material->GetEffect()->AsSRV("Transforms")->SetResource(transSrv);
		
}

void ModelInstance::Update()
{
	frustum->GetPlanes(plane);

	for(size_t i=0; i<frustumVars.size(); i++)
		frustumVars[i]->SetFloatVectorArray((float*)plane, 0, 6);

	for (size_t i = 0; i<renderVars.size(); i++)
		renderVars[i]->SetRawValue(rDesc, 0, sizeof(RenderDesc)*64);
}

void ModelInstance::Render(UINT tec)
{
	for (ModelMesh*mesh : GetModel()->Meshes())
	{
		//���߿� ���͸��� ������ó���ǰ� �ؾ���
		mesh->RenderInstance(maxCount, tec);
	}
}

