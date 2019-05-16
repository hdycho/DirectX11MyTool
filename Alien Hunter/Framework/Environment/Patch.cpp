#include "Framework.h"
#include "Patch.h"

Patch::Patch()
{
}

Patch::~Patch()
{
	SAFE_RELEASE(vertexBuffer);
}

void Patch::InitPatchData(int patchWidth)
{
	width = (WORD)patchWidth;

	BuildCenterIndices();
	BuildTopEdges();
	BuildLeftEdges();
}

void Patch::BuildCenterIndices()
{
	for (WORD tessLevel = 0; tessLevel <= 6; tessLevel++)
	{
		WORD t = (WORD)pow(2, tessLevel);

		vector<WORD> indices;
		for (WORD z = 0 + t, z0 = t; z < width; z += t, z0 += t)
		{
			for (WORD x = 0 + t, x0 = t; x < width; x += t, x0 += t)
			{
				indices.push_back(z0 * (width + 1) + x0);
				indices.push_back(z0 * (width + 1) + x0 + t);
				indices.push_back((z0 + t) * (width + 1) + x0);

				indices.push_back((z0 + t) * (width + 1) + x0);
				indices.push_back(z0 * (width + 1) + x0 + t);
				indices.push_back((z0 + t) * (width + 1) + x0 + t);
			} // for(x)
		} // for(z)

		if (indices.size() > 0)
		{
			//Create Index Buffer
			{
				D3D11_BUFFER_DESC desc = { 0 };
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.ByteWidth = sizeof(WORD) * indices.size();
				desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				desc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = &indices[0];

				HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &centerIndexBuffer[t]);
				assert(SUCCEEDED(hr));
			}
		}
		else
		{
			centerIndexBuffer[t] = NULL;
		}
		centerIndexCount[t] = (WORD)indices.size();
	}
}

void Patch::BuildTopEdges()
{
	tuple<WORD, WORD> key;
	vector<WORD> indices;

	WORD z0, t;
	for (WORD i = 0; i < 6; i++)
	{
		t = (WORD)powf(2, i);
		key = make_tuple(t, t);
		z0 = 0;

		for (WORD x = 0, x0 = 0; x < width; x += t, x0 += t) {
			indices.push_back(z0 * (width + 1) + x0);
			indices.push_back(z0 * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0);

			indices.push_back((z0 + t) * (width + 1) + x0);
			indices.push_back(z0 * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(WORD) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &edgeIbs[NeighborDir::Top][key]);
		assert(SUCCEEDED(hr));
		edgeIndicesCount[NeighborDir::Top][key] = (WORD)indices.size();

		indices.clear();
	}


	for (WORD i = 0; i < 6; i++)
	{
		t = (WORD)pow(2, i);
		WORD t1 = (WORD)pow(2.0, i + 1);
		key = make_tuple(t, t1);
		z0 = 0;

		indices.push_back(0);
		indices.push_back(t1);
		indices.push_back((z0 + t) * (width + 1) + t);

		indices.push_back(t1);
		indices.push_back(t * (width + 1) + t1);
		indices.push_back((t) * (width + 1) + t);

		for (WORD x = 0 + t1, x0 = t1; x < width; x += t1, x0 += t1) {
			indices.push_back(z0 * (width + 1) + x0);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0);

			indices.push_back((z0) * (width + 1) + x0 + t1);
			indices.push_back((z0 + t) * (width + 1) + x0 + t1);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);

			indices.push_back((z0) * (width + 1) + x0);
			indices.push_back((z0) * (width + 1) + x0 + t1);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(WORD) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &edgeIbs[NeighborDir::Top][key]);
		assert(SUCCEEDED(hr));
		edgeIndicesCount[NeighborDir::Top][key] = (WORD)indices.size();

		indices.clear();
	}

	for (WORD i = 1; i <= 6; i++)
	{
		t = (WORD)pow(2, i);
		WORD t1 = (WORD)pow(2, i - 1);
		key = make_tuple(t, t1);

		z0 = 0;

		for (WORD x = 0, x0 = 0; x <width - t1; x += t, x0 += t) {
			indices.push_back(z0 * (width + 1) + x0);
			indices.push_back((z0) * (width + 1) + x0 + t1);
			indices.push_back((z0 + t) * (width + 1) + x0);

			indices.push_back((z0) * (width + 1) + x0 + t1);
			indices.push_back((z0) * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);

			indices.push_back((z0) * (width + 1) + x0 + t1);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(WORD) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &edgeIbs[NeighborDir::Top][key]);
		assert(SUCCEEDED(hr));
		edgeIndicesCount[NeighborDir::Top][key] = (WORD)indices.size();

		indices.clear();
	}
}

void Patch::BuildLeftEdges()
{
	tuple<WORD, WORD> key;
	vector<WORD> indices;

	WORD x0, t;
	for (WORD i = 0; i < 6; i++)
	{
		t = (WORD)powf(2, i);
		key = make_tuple(t, t);

		x0 = 0;
		for (WORD z = 0, z0 = 0; z < width; z += t, z0 += t) {
			indices.push_back(z0 * (width + 1) + x0);
			indices.push_back(z0 * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0);

			indices.push_back((z0 + t) * (width + 1) + x0);
			indices.push_back(z0 * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(WORD) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &edgeIbs[NeighborDir::Left][key]);
		assert(SUCCEEDED(hr));
		edgeIndicesCount[NeighborDir::Left][key] = (WORD)indices.size();

		indices.clear();
	}

	for (WORD i = 0; i < 6; i++)
	{
		t = (WORD)pow(2, i);
		WORD t1 = (WORD)pow(2, i + 1);
		key = make_tuple(t, t1);

		x0 = 0;

		indices.push_back(0);
		indices.push_back((width + 1) + t);
		indices.push_back(t1*(width + 1));

		indices.push_back((width + 1) + t);
		indices.push_back(t1 * (width + 1) + t);
		indices.push_back(t1 * (width + 1));


		for (WORD z = 0 + t1, z0 = t1; z < width; z += t1, z0 += t1) {
			indices.push_back(z0 * (width + 1) + x0);
			indices.push_back(z0 * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);

			indices.push_back((z0 + t) * (width + 1) + x0 + t);
			indices.push_back((z0 + t1) * (width + 1) + x0 + t);
			indices.push_back((z0 + t1) * (width + 1) + x0);

			indices.push_back(z0 * (width + 1) + x0);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);
			indices.push_back((z0 + t1) * (width + 1) + x0);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(WORD) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &edgeIbs[NeighborDir::Left][key]);
		assert(SUCCEEDED(hr));
		edgeIndicesCount[NeighborDir::Left][key] = (WORD)indices.size();

		indices.clear();
	}

	for (WORD i = 1; i <= 6; i++)
	{
		t = (WORD)pow(2, i);
		WORD t1 = (WORD)pow(2, i - 1);
		key = make_tuple(t, t1);

		x0 = 0;

		for (WORD z = 0, z0 = 0; z < width - t1; z += t, z0 += t) {
			indices.push_back(z0 * (width + 1) + x0);
			indices.push_back((z0) * (width + 1) + x0 + t);
			indices.push_back((z0 + t1) * (width + 1) + x0);

			indices.push_back((z0 + t1) * (width + 1) + x0);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0);

			indices.push_back((z0) * (width + 1) + x0 + t);
			indices.push_back((z0 + t) * (width + 1) + x0 + t);
			indices.push_back((z0 + t1) * (width + 1) + x0);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(WORD) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &edgeIbs[NeighborDir::Left][key]);
		assert(SUCCEEDED(hr));
		edgeIndicesCount[NeighborDir::Left][key] = (WORD)indices.size();

		indices.clear();
	}
}
