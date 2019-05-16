#pragma once

class IData
{
public:
	virtual class GData* Save() = 0;
	virtual wstring&Name() = 0;

	virtual void Load(wstring fileName) = 0;
	virtual wstring&LoadDataFile() = 0;

	virtual D3DXVECTOR3&LoadScale() = 0;
	virtual D3DXVECTOR3&LoadRotate() = 0;
	virtual D3DXVECTOR3&LoadPosition() = 0;
};
