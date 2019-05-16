#pragma once
#include "Interface\IPicked.h"
#include "Interface\IData.h"

class CubeSky :public IData
{
public:
	CubeSky(wstring cubeFile);
	~CubeSky();

	Material* GetMaterial() { return material; }

	void Update();
	void PreRender();
	void Render();

	//인터페이스
	wstring&Name() { return cubeSkyName; }
	wstring&LoadDataFile() { return loadFile; }
	class GData* Save() { return NULL; }
	void Load(wstring fileName) {}
	bool IsPicked() { return isPick; }
	void SetPickState(bool val) { isPick = val; }
	D3DXVECTOR3&LoadScale() { return loadScale; }
	D3DXVECTOR3&LoadRotate() { return loadRotate; }
	D3DXVECTOR3&LoadPosition() { return loadPosition; }

	ID3D11ShaderResourceView* SRV() { return srv; }
	void CreateSRV(wstring fileName);
	wstring&GetFileName() { return fileName; }
private:
	wstring cubeSkyName;
	wstring fileName;
	bool isPick;

	Material* material;
	MeshSphere* sphere;

	ID3D11ShaderResourceView* srv;

private:
	ID3DX11EffectShaderResourceVariable*srvVariable;

	//일단 안씀
	D3DXVECTOR3 loadScale;
	D3DXVECTOR3 loadRotate;
	D3DXVECTOR3 loadPosition;
	wstring loadFile;
};