#pragma once
#include "GameAnimModel.h"

class WeaponModel :public GameModel
{
public:
	WeaponModel(wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile,
		ExecuteValues*values);

	WeaponModel(WeaponModel&model);
	virtual ~WeaponModel();

	void Update() override;
	void Render() override;

	virtual void SetParentModel(GameModel*model, wstring parentName)
	{
		parentModel = model;
		this->parentName = parentName;
	}

private:
	GameModel*parentModel;
	wstring parentName;
};