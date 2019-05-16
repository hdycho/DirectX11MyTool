#include "Framework.h"
#include "Mesh.h"
#include "Collider\ColliderBox.h"
#include "Collider\ColliderRay.h"
#include "Collider\ColliderSphere.h"

Mesh::Mesh(Material* material, Effect*lineEffect)
	: position(0, 0, 0)
	, scale(1, 1, 1), rotation(0, 0, 0)
	, vertexBuffer(NULL), indexBuffer(NULL)
	, vertices(NULL), indices(NULL)
	, pass(0), isPicked(false)
	, lineEffect(lineEffect)
{
	D3DXMatrixIdentity(&world);
	//material->Clone((void**)&this->material);

	this->material = material;
}

Mesh::~Mesh()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	for (UINT i = 0; i < colliders.size(); i++)
	{
		SAFE_DELETE(colliders[i]);
	}
	//SAFE_DELETE(material);
}

void Mesh::Render()
{
	if (vertexBuffer == NULL && indexBuffer == NULL)
	{
		CreateBuffer();
	}

	UINT stride = sizeof(VertexTextureNormalTangent);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->WorldVariable()->SetMatrix(world);
	material->GetEffect()->DrawIndexed(0, pass, indexCount);

	for (UINT i = 0; i < colliders.size(); i++)
	{
		colliders[i]->DrawCollider();
	}
}


void Mesh::RenderShadow(UINT tech, UINT pass)
{
	if (vertexBuffer == NULL && indexBuffer == NULL)
	{
		CreateData();
		CreateBuffer();
	}

	UINT stride = sizeof(VertexTextureNormalTangent);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->WorldVariable()->SetMatrix(world);
	material->GetEffect()->DrawIndexed(tech, pass, indexCount);
}

void Mesh::CreateCollider(int type)
{
	switch (type)
	{
		case 0:
		{
			ColliderBox*boxCollider = new ColliderBox(Name() + L"Box" + to_wstring(colliders.size()), min, max, lineEffect);
			boxCollider->Ready();

			colliders.push_back(boxCollider);
		}
		break;
		case 1:
		{
			D3DXVECTOR3 centerPos = Math::GetCenterPos(min, max);

			ColliderRay*rayCollider = new ColliderRay(Name() + L"Ray" + to_wstring(colliders.size()), centerPos, direction, lineEffect);
			rayCollider->Ready();

			colliders.push_back(rayCollider);
		}
		break;
		case 2:
		{
			D3DXVECTOR3 centerPos = Math::GetCenterPos(min, max);


			float radius = (max.x - min.x) / 2.0f;

			ColliderSphere*sphereCollider = new ColliderSphere(Name() + L"Sphere" + to_wstring(colliders.size()), centerPos, radius, lineEffect);
			sphereCollider->Ready();

			colliders.push_back(sphereCollider);
		}
		break;
	}
}

void Mesh::ColliderUpdate()
{
}

void Mesh::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

void Mesh::Position(D3DXVECTOR3 & vec)
{
	position = vec;

	UpdateWorld();
}

void Mesh::Position(D3DXVECTOR3 * vec)
{
	*vec = position;
}

void Mesh::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void Mesh::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void Mesh::Rotation(D3DXVECTOR3 * vec)
{
	*vec = rotation;
}

void Mesh::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

void Mesh::RotationDegree(D3DXVECTOR3 & vec)
{
	rotation = vec * Math::PI / 180.0f;

	UpdateWorld();
}

void Mesh::RotationDegree(D3DXVECTOR3 * vec)
{
	*vec = rotation * 180.0f / Math::PI;
}

void Mesh::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

void Mesh::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void Mesh::Scale(D3DXVECTOR3 * vec)
{
	*vec = scale;
}

void Mesh::Matrix(D3DXMATRIX * mat)
{
	*mat = world;
}

bool Mesh::IsPicked()
{
	return isPicked;
}

void Mesh::PickUpdate()
{
	if (Mouse::Get()->Down(0))
	{
		D3DXVECTOR3 start;
		Context::Get()->GetMainCamera()->Position(&start);

		D3DXVECTOR3 direction = Context::Get()->GetMainCamera()->Direction
		(Context::Get()->GetViewport(), Context::Get()->GetPerspective());

		D3DXMATRIX inverse;
		D3DXMATRIX mWorld;
		D3DXMatrixIdentity(&mWorld);
		mWorld._41 = world._41;
		mWorld._42 = world._42;
		mWorld._43 = world._43;

		D3DXMatrixInverse(&inverse, NULL, &mWorld);

		D3DXVec3TransformNormal(&direction, &direction, &inverse);

		//내가구한 최대최소점의 면과 선이 충돌했을때 트루반환

		vector<D3DXVECTOR3> checkCube;
		checkCube.clear();


		checkCube.resize(8);
		for (int i = 0; i < 8; i++)
			D3DXVec3TransformCoord(&checkCube[i], &pickCube[i], &world);

		vector<UINT> indices;
		indices.resize(36);
		indices = {
			0,1,2,2,1,3,
			2,3,6,6,3,7,
			6,7,4,4,7,5,
			0,1,4,4,1,5,
			1,5,3,3,5,7,
			0,4,2,2,4,6
		};
		for (int i = 0; i < 6; i++)
		{
			D3DXVECTOR3 p[6];
			for (int j = 0; j < 6; j++)
				p[j] = checkCube[indices[6 * i + j]];

			float u, v, distance;

			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
			{
				if (!isPicked)
					isPicked = true;
				else
					isPicked = false;
				break;

			}
			else if (D3DXIntersectTri(&p[3], &p[4], &p[5], &start, &direction, &u, &v, &distance))
			{
				if (!isPicked)
					isPicked = true;
				else
					isPicked = false;
				break;
			}
		}
	}
}

void Mesh::SetShader(D3DXMATRIX v, D3DXMATRIX p)
{
	material->LightView()->SetMatrix(v);
	material->LightProjection()->SetMatrix(p);
}

void Mesh::SetShader(D3DXMATRIX shadowTransform, ID3D11ShaderResourceView * srv)
{
	material->ShadowTransform()->SetMatrix(shadowTransform);
	material->ShadowMap()->SetResource(srv);
}

void Mesh::ShadowUpdate()
{
}

void Mesh::NormalRender()
{
	Render();
}

void Mesh::ShadowRender(UINT tech, UINT pass)
{
	RenderShadow(tech, pass);
}

void Mesh::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(
		&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);

	D3DXVec3Normalize(&direction, &direction);
	D3DXVec3Normalize(&up, &up);
	D3DXVec3Normalize(&right, &right);

	for (size_t i = 0; i < colliders.size(); i++)
	{
		colliders[i]->Transform(world);
	}
}

void Mesh::CreateBuffer()
{
	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormalTangent) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Create Index Buffer
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
}