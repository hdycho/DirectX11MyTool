#include "Framework.h"
#include "InstanceManager.h"

InstanceManager*InstanceManager::instance = NULL;

void InstanceManager::Create()
{
	if (instance == NULL)
		instance = new InstanceManager();
}

void InstanceManager::Delete()
{
	instance->animInstance.clear();
	instance->modelInstance.clear();

	SAFE_DELETE(instance);
}

InstanceManager::InstanceManager()
{
}

InstanceManager::~InstanceManager()
{


}

void InstanceManager::AddModel(GameModel * model, wstring shaderFile, InstanceType instType)
{
	Model*m = model->GetModel();
	wstring modelName = m->GetModelName();
	switch (instType)
	{
		case InstanceManager::InstanceType::MODEL:
		{
			modelsIter msIter = modelInstance.find(modelName);
			if (msIter != modelInstance.end()) // 기존에 해당모델의 인스턴스가 존재하면
			{
				msIter->second->AddWorld(model->World());
			}
			else
			{
				ModelInstance*mInst = new ModelInstance(model->GetModel(), shaderFile);
				mInst->Ready();
				mInst->AddWorld(model->World());

				modelInstance[modelName] = mInst;
			}
		}
		break;
		case InstanceManager::InstanceType::ANIMATION:
		{
			animsIter asIter = animInstance.find(modelName);
			if (asIter != animInstance.end()) // 기존에 해당모델의 인스턴스가 존재하면
			{
				asIter->second->AddWorld((GameAnimator*)model);
			}
			else
			{
				AnimInstance*aInst = new AnimInstance(model->GetModel(), shaderFile);
				aInst->Ready();
				aInst->AddWorld((GameAnimator*)model);

				animInstance[modelName] = aInst;
			}
		}
		break;
	}
}

void InstanceManager::AddModel(GameModel * model, Effect * effect, InstanceType instType)
{
	Model*m = model->GetModel();
	wstring modelName = m->GetModelName();
	switch (instType)
	{
		case InstanceManager::InstanceType::MODEL:
		{
			modelsIter msIter = modelInstance.find(modelName);
			if (msIter != modelInstance.end()) // 기존에 해당모델의 인스턴스가 존재하면
			{
				msIter->second->AddWorld(model->World());
			}
			else
			{
				ModelInstance*mInst = new ModelInstance(model->GetModel(), effect);
				mInst->Ready();
				mInst->AddWorld(model->World());

				modelInstance[modelName] = mInst;
			}
		}
		break;
		case InstanceManager::InstanceType::ANIMATION:
		{
			animsIter asIter = animInstance.find(modelName);
			if (asIter != animInstance.end()) // 기존에 해당모델의 인스턴스가 존재하면
			{
				asIter->second->AddWorld((GameAnimator*)model);
			}
			else
			{
				AnimInstance*aInst = new AnimInstance(model->GetModel(), effect);
				aInst->Ready();
				aInst->AddWorld((GameAnimator*)model);

				animInstance[modelName] = aInst;
			}
		}
		break;
	}
}

void InstanceManager::DeleteModel(GameModel * model, UINT instanceID, InstanceType instType)
{
	Model*m = model->GetModel();
	wstring modelName = m->GetModelName();
	switch (instType)
	{
		case InstanceManager::InstanceType::MODEL:
		{
			modelsIter msIter = modelInstance.find(modelName);
			if (msIter != modelInstance.end()) // 기존에 해당모델의 인스턴스가 존재하면
			{
				msIter->second->DeleteWorld(instanceID);
			}
		}
		break;
		case InstanceManager::InstanceType::ANIMATION:
		{
			animsIter asIter = animInstance.find(modelName);
			if (asIter != animInstance.end()) // 기존에 해당모델의 인스턴스가 존재하면
			{
				asIter->second->DeleteWorld(instanceID);
			}
		}
		break;
	}
}

void InstanceManager::UpdateWorld(GameModel * model, UINT instNum, D3DXMATRIX & world, InstanceType instType)
{
	Model*m = model->GetModel();
	wstring modelName = m->GetModelName();

	switch (instType)
	{
		case InstanceManager::InstanceType::MODEL:
		{
			modelsIter msIter = modelInstance.find(modelName);
			if (msIter != modelInstance.end()) // 기존에 해당모델의 인스턴스가 존재하면
			{
				if(msIter->second->GetRenderState(instNum))
					msIter->second->UpdateWorld(instNum, world);
			}
		}
		break;
		case InstanceManager::InstanceType::ANIMATION:
		{

			animsIter asIter = animInstance.find(modelName);
			if (asIter != animInstance.end()) // 기존에 해당모델의 인스턴스가 존재하면
			{
				if (asIter->second->GetRenderState(instNum))
					asIter->second->UpdateWorld(instNum, world);
			}
		}
		break;
	}
}

ModelInstance * InstanceManager::FindModel(wstring name)
{
	modelsIter msIter = modelInstance.find(name);

	if (msIter != modelInstance.end())
		return msIter->second;
	else
		return NULL;
}

AnimInstance * InstanceManager::FindAnim(wstring name)
{
	animsIter asIter = animInstance.find(name);

	if (asIter != animInstance.end())
		return asIter->second;
	else
		return NULL;
}

int InstanceManager::GetIsRenderModel(wstring name)
{
	modelsIter msIter = modelInstance.find(name);

	int count = 0;
	if (msIter != modelInstance.end())
	{
		for (int i = 0; i < 64; i++)
		{
			if (msIter->second->GetRenderState(i))
				count++;
		}
	}
	return count;
}

int InstanceManager::GetIsRenderAnim(wstring name)
{
	animsIter msIter = animInstance.find(name);

	int count = 0;
	if (msIter != animInstance.end())
	{
		for (int i = 0; i < 64; i++)
		{
			if (msIter->second->GetRenderState(i))
				count++;
		}
	}
	return count;
}

void InstanceManager::Render(UINT tec)
{
	modelsIter msIter = modelInstance.begin();

	for (; msIter != modelInstance.end(); msIter++)
	{
		msIter->second->Render(tec);
	}

	animsIter asIter = animInstance.begin();

	for (; asIter != animInstance.end(); asIter++)
	{
		asIter->second->Render(tec);
	}
}

void InstanceManager::Update()
{
	modelsIter msIter = modelInstance.begin();

	for (; msIter != modelInstance.end(); msIter++)
	{
		msIter->second->Update();
	}

	animsIter asIter = animInstance.begin();

	for (; asIter != animInstance.end(); asIter++)
	{
		asIter->second->Update();
	}
}

void InstanceManager::RemoveAllModel()
{
	//if (modelInstance == NULL)return;
	if (instance == NULL)return;

	/*modelsIter msIter = modelInstance.begin();

	if (modelInstance.size() > 0)
	{
		for (; msIter != modelInstance.end();)
		{
			if (msIter->second != NULL)
			{
				SAFE_DELETE(msIter->second);
				msIter = modelInstance.erase(msIter);
			}
			else
			{
				msIter++;
			}
		}
	}*/
	modelInstance.clear();

	//if (animInstance == NULL)return;
	/*animsIter asIter = animInstance.begin();
	
	if (animInstance.size() > 0)
	{
		for (; asIter != animInstance.end();)
		{
			if (asIter->second != NULL)
			{
				SAFE_DELETE(asIter->second);
				asIter = animInstance.erase(asIter);
			}
			else
			{
				asIter++;
			}
		}
	}*/

	animInstance.clear();
}
