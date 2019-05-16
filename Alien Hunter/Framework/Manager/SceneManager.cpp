#include "Framework.h"
#include "SceneManager.h"

SceneManager*SceneManager::instance = NULL;

void SceneManager::Create()
{
	if (instance == NULL)
	{
		instance = new SceneManager();
		instance->sType = SCENETYPE::StartScene;

		instance->scenes.resize(10);
		//instance->pbar = new ProgressBar();
		//instance->pbar->Init();
	}
}

void SceneManager::Delete()
{
	/*for (size_t i = 0; i < instance->scenes.size(); i++)
		SAFE_DELETE(instance->scenes[i]);

	instance->scenes.clear();
*/
	SAFE_DELETE(instance);
}

void SceneManager::InputScene(SCENETYPE sceneName, IExecute * scene)
{
	scenes[(int)sceneName] = scene;
}

void SceneManager::Init()
{
	scenes[(int)sType]->Initialize();
}

void SceneManager::Update()
{
	//if (mu != NULL)
	//{
	//	bool temp = false;
	//	mu->lock();
	//	temp = bInitialize;
	//	mu->unlock();

	//	if (temp == false)
	//	{
	//		//로딩씬 업데이트
	//		pbar->Update();
	//	}
	//	else
	//	{
	//		t.join();
	//		SAFE_DELETE(mu);

	//		scenes[(int)sType]->Ready();
	//	}
	//}
	//else
	//{
	//	scenes[(int)sType]->Update();
	//}

	scenes[(int)sType]->Update();
}

void SceneManager::Ready()
{
	scenes[(int)sType]->Ready();
	//pbar->Ready();

	ParticleInit();
	UIInit();
	SoundInit();
}

void SceneManager::PreRender()
{
	/*if (mu != NULL)
	{
		bool temp = false;
		mu->lock();
		temp = bInitialize;
		mu->unlock();

		if (temp == false)
		{

		}
	}
	else
	{
		scenes[(int)sType]->PreRender();
	}*/

	scenes[(int)sType]->PreRender();
}

void SceneManager::Render()
{
	/*if (mu != NULL)
	{
		bool temp = false;
		mu->lock();
		temp = bInitialize;
		mu->unlock();

		if (temp == false)
		{
			pbar->Render();
		}
	}
	else
	{
		scenes[(int)sType]->Render();
	}*/

	scenes[(int)sType]->Render();
}

void SceneManager::PostRender()
{
	/*if (mu != NULL)
	{
		bool temp = false;
		mu->lock();
		temp = bInitialize;
		mu->unlock();

		if (temp == false)
		{

		}
	}
	else
	{
		scenes[(int)sType]->PostRender();
	}*/

	scenes[(int)sType]->PostRender();
}

void SceneManager::Destroy()
{
	//scenes[curSceneName]->Destroy();
}

void SceneManager::ChangeScene(SCENETYPE changeSceneName)
{
	/*scenes[(int)sType]->Destroy();
	
	pbar->Init();
	mu = new mutex();
	t = thread([&]()
	{
		scenes[(int)changeSceneName]->Initialize();

		mu->lock();
		bInitialize = true;
		mu->unlock();
	});
	
	sType = changeSceneName;*/

	scenes[(int)sType]->Destroy();

	scenes[(int)changeSceneName]->Initialize();
	scenes[(int)changeSceneName]->Ready();

	sType = changeSceneName;
}

void SceneManager::StartLoadingScene()
{
	thread t;
}

void SceneManager::ParticleInit()
{
	//=================파티클추가==================//
	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 60;
		pinfo.startRange = 0.1f;
		pinfo.lifeTime = 0.556f;
		pinfo.particleCount = 500;
		pinfo.perSecontNum = 651;
		pinfo.size = D3DXVECTOR2(10, 10);
		pinfo.gravity = -100;
		pinfo.velocityVar = 7;
		pinfo.velocity = 15;
		pinfo.type = ParticleSystem::ParticleType::Cylinder;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"fireball/" + to_wstring(i) + L".png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"Fire", 1, pinfo);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 9;
		pinfo.startRange = 0;
		pinfo.lifeTime = 0.438;
		pinfo.particleCount = 60;
		pinfo.perSecontNum = 240;
		pinfo.size = D3DXVECTOR2(1, 1);
		pinfo.gravity = 100;
		pinfo.velocityVar = 10;
		pinfo.velocity = 18;
		pinfo.type = ParticleSystem::ParticleType::Cylinder;
		pinfo.option = ParticleSystem::ParticleOption::Once;
		vector<wstring> names;
		for (int i = 1; i <= 7; i++)
			names.push_back(Textures + L"BloodAnim/" + to_wstring(i) + L".png");
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"Blood", 10, pinfo);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 60;
		pinfo.startRange = 0;
		pinfo.lifeTime = 0.1f;
		pinfo.particleCount = 1;
		pinfo.perSecontNum = 451;
		pinfo.size = D3DXVECTOR2(2.5, 2.5);
		pinfo.gravity = 0;
		pinfo.velocityVar = 0;
		pinfo.velocity = 15;
		pinfo.type = ParticleSystem::ParticleType::None;
		pinfo.option = ParticleSystem::ParticleOption::Once;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"gunfire2.png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"gunfire", 10, pinfo);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 0.1f;
		pinfo.lifeTime = 0.445f;
		pinfo.particleCount = 300;
		pinfo.perSecontNum = 451;
		pinfo.size = D3DXVECTOR2(2.2, 2.2);
		pinfo.gravity = 0;
		pinfo.velocityVar = 7;
		pinfo.velocity = 15;
		pinfo.type = ParticleSystem::ParticleType::Cylinder;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 16; i++)
		{
			names.push_back(Textures + L"FireAnim/" + to_wstring(i) + L".png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"buster1", 1, pinfo);
		ParticleManager::Get()->InputParticle(L"buster2", 1, pinfo);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 0.1f;
		pinfo.lifeTime = 0.845f;
		pinfo.particleCount = 600;
		pinfo.perSecontNum = 851;
		pinfo.size = D3DXVECTOR2(8.5, 8.5);
		pinfo.gravity = 0;
		pinfo.velocityVar = 9;
		pinfo.velocity = 25;
		pinfo.type = ParticleSystem::ParticleType::Cylinder;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 16; i++)
		{
			names.push_back(Textures + L"FireAnim/" + to_wstring(i) + L".png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"buster3", 1, pinfo);
		ParticleManager::Get()->InputParticle(L"buster4", 1, pinfo);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 0.03f;
		pinfo.lifeTime = 0.4f;
		pinfo.particleCount = 300;
		pinfo.perSecontNum = 516;
		pinfo.size = D3DXVECTOR2(0.35, 0.35);
		pinfo.gravity = 0;
		pinfo.velocityVar = 7;
		pinfo.velocity = 40;
		pinfo.type = ParticleSystem::ParticleType::None;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"RedPart.png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"bulletEffect", 10, pinfo);
	}{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 0.03f;
		pinfo.lifeTime = 0.4f;
		pinfo.particleCount = 300;
		pinfo.perSecontNum = 516;
		pinfo.size = D3DXVECTOR2(0.35, 0.35);
		pinfo.gravity = 0;
		pinfo.velocityVar = 7;
		pinfo.velocity = 40;
		pinfo.type = ParticleSystem::ParticleType::None;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"RedPart.png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"bulletEffect", 10, pinfo);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 1.3f;
		pinfo.lifeTime = 0.438f;
		pinfo.particleCount = 500;
		pinfo.perSecontNum = 651;
		pinfo.size = D3DXVECTOR2(1.2, 1.2);
		pinfo.gravity = 0;
		pinfo.velocityVar = 0;
		pinfo.velocity = 20;
		pinfo.type = ParticleSystem::ParticleType::None;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"GameScene/DashEffect.png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"DashEffect", 3, pinfo);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 0.2f;
		pinfo.lifeTime = 0.244f;
		pinfo.particleCount = 600;
		pinfo.perSecontNum = 944;
		pinfo.size = D3DXVECTOR2(4, 4);
		pinfo.gravity = 0;
		pinfo.velocityVar = 20;
		pinfo.velocity = 50;
		pinfo.type = ParticleSystem::ParticleType::Cylinder;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"GameScene/PoisonCloud.png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"PoisonCloud", 5, pinfo,false);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 4;
		pinfo.lifeTime = 0.6;
		pinfo.particleCount = 300;
		pinfo.perSecontNum = 451;
		pinfo.size = D3DXVECTOR2(1, 1);
		pinfo.gravity = 0;
		pinfo.velocityVar = 0;
		pinfo.velocity = 15;
		pinfo.type = ParticleSystem::ParticleType::Cylinder;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"GameScene/bossPage1.png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"bossPage1", 5, pinfo, false);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 4;
		pinfo.lifeTime = 0.6;
		pinfo.particleCount = 300;
		pinfo.perSecontNum = 451;
		pinfo.size = D3DXVECTOR2(1, 1);
		pinfo.gravity = 0;
		pinfo.velocityVar = 0;
		pinfo.velocity = 15;
		pinfo.type = ParticleSystem::ParticleType::Cylinder;
		pinfo.option = ParticleSystem::ParticleOption::Repeat;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"GameScene/bossPage2.png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"bossPage2", 5, pinfo, false);
	}

	{
		ParticleManager::PTinfo pinfo;
		pinfo.animSpeed = 5;
		pinfo.startRange = 3.5f;
		pinfo.lifeTime = 0.6;
		pinfo.particleCount = 20;
		pinfo.perSecontNum = 800;
		pinfo.size = D3DXVECTOR2(20, 20);
		pinfo.gravity = 0;
		pinfo.velocityVar = 0;
		pinfo.velocity = 15;
		pinfo.type = ParticleSystem::ParticleType::None;
		pinfo.option = ParticleSystem::ParticleOption::Once;

		vector<wstring> names;
		for (int i = 1; i <= 1; i++)
		{
			names.push_back(Textures + L"GameScene/Smoke.png");
		}
		pinfo.textures.assign(names.begin(), names.end());
		ParticleManager::Get()->InputParticle(L"Smoke", 1, pinfo);
	}
	//============================================//
}

void SceneManager::UIInit()
{
	float width = Context::Get()->GetViewport()->GetWidth();
	float height = Context::Get()->GetViewport()->GetHeight();
	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/helmet.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, 0);
		uiDesc.dcImg->Scale(width, height);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"helmet", uiDesc);
	}

	{
		for (int i = 1; i <= 8; i++)
		{
			UIManager::UIdesc uiDesc;

			uiDesc.uiImg = new Texture(Textures + L"GameScene/hp"+to_wstring(i)+L".png");

			uiDesc.dcImg = new Render2D();
			uiDesc.dcImg->Position(1, height/16.9f);
			uiDesc.dcImg->Scale(width/5.2f, width/ 5.2f);
			uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

			UIManager::Get()->InputUI(L"hp"+to_wstring(i), uiDesc);
		}
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/healthback.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width/240, height/27.5f);
		uiDesc.dcImg->Scale(width/4.7f, width/4.7f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"healthback", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/healthmiddle.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width / 240, height / 27.5f);
		uiDesc.dcImg->Scale(width / 4.7f, width / 4.7f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"healthmiddle", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/bullet1.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(4, 28);
		uiDesc.dcImg->Scale(350, 350);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet1", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/bullet1.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.85f, height*0.78f);
		uiDesc.dcImg->Scale(width/50, width / 30);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet1_1", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.88f, height*0.78f);
		uiDesc.dcImg->Scale(width / 50, width / 30);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());
		UIManager::Get()->InputUI(L"bullet1_2", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/gun1.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.75f, height*0.83f);
		uiDesc.dcImg->Scale(width/6.6f, width/16.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"gun1", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/gun2.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.75f, height*0.83f);
		uiDesc.dcImg->Scale(width / 6.6f, width / 16.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"gun2", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/gun3.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.75f, height*0.83f);
		uiDesc.dcImg->Scale(width / 5.6f, width / 16.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"gun3", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/bullet3.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.85f, height*0.78f);
		uiDesc.dcImg->Scale(width / 25, width / 35);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet3", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/cross1.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.44f, height*0.5f);
		uiDesc.dcImg->Scale(width/16, width / 16);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"cross1", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/cross2.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.44f, height*0.5f);
		uiDesc.dcImg->Scale(width / 16, width / 16);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"cross2", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/cross3.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.44f, height*0.5f);
		uiDesc.dcImg->Scale(width / 16, width / 16);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"cross3", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/search.png");

		uiDesc.dcImg = new Render2D();
		//uiDesc.dcImg->Position(width*0.2f, height*0.035f);
		//uiDesc.dcImg->Scale(width *0.55f, width *0.58f);
		uiDesc.dcImg->Position(width*0.2f, height*0.035f);
		uiDesc.dcImg->Scale(width *0.55f, width *0.58f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"search", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/search2.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.2f, height*0.035f);
		uiDesc.dcImg->Scale(width *0.15f, width *0.15f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"search2", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/inhealth.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.2f, height*0.035f);
		uiDesc.dcImg->Scale(width *0.15f, width *0.05f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"0inhealth", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/outhealth.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.2f, height*0.035f);
		uiDesc.dcImg->Scale(width *0.1f, width *0.02f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"1outhealth", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/bullet2.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.75f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_1", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.765f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_2", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.78f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_3", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.795f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_4", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.810f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_5", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.825f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_6", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.84f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_7", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.855f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_8", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.87f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_9", uiDesc);

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.885f, height*0.765f);
		uiDesc.dcImg->Scale(width / 85.0f, width / 37.0f);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bullet2_10", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event1.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event1", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event2.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event2", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event3.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event3", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event4.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event4", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event5.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event5", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event6.png");

		uiDesc.dcImg = new Render2D();
		//uiDesc.dcImg->Position(0, height*0.37f);
		//uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event6", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event7.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event7", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event8.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event8", uiDesc);
	}
	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event9.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event9", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event10.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, height*0.37f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event10", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event11.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.6f, height*0.64f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event11", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event12.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.6f, height*0.64f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event12", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/event13.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.6f, height*0.64f);
		uiDesc.dcImg->Scale(width / 2.5f, width / 5);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"event13", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/event/Prog.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, 0);
		uiDesc.dcImg->Scale(width*0.3, width*0.15);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"Prog", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/bloodScreen.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, 0);
		uiDesc.dcImg->Scale(width, height);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"bloodScreen", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/gameoverText.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0, 0);
		uiDesc.dcImg->Scale(width, height);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"gameoverText", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/stage1.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.4f, height*0.7f);
		uiDesc.dcImg->Scale(width/5.3f, width / 5.3f);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"stage1", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/stage2.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.4f, height*0.7f);
		uiDesc.dcImg->Scale(width / 5.3f, width / 5.3f);
		//uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		//uiDesc.dcImg->Scale(width / 5, width / 9.5);
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"stage2", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/talk.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.6f, height*0.6f);
		uiDesc.dcImg->Scale(width / 12.5f, width / 25);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"talk", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/talk.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(width*0.6f, height*0.6f);
		uiDesc.dcImg->Scale(width / 12.5f, width / 25);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"talk", uiDesc);
	}

	{
		UIManager::UIdesc uiDesc;

		uiDesc.uiImg = new Texture(Textures + L"GameScene/clearScene2.png");

		uiDesc.dcImg = new Render2D();
		uiDesc.dcImg->Position(0,0);
		uiDesc.dcImg->Scale(width,height);
		/*uiDesc.dcImg->Position(width*0.05f, height*0.78f);
		uiDesc.dcImg->Scale(width / 5, width / 9.5);*/
		uiDesc.dcImg->SRV(uiDesc.uiImg->SRV());

		UIManager::Get()->InputUI(L"clearScene2", uiDesc);
	}
}

void SceneManager::SoundInit()
{
	soundManager::Get()->addSound("00startScene",Sounds + "StartScene.wav", true, true);
	soundManager::Get()->addSound("01loadingScene", Sounds + "LoadingScene.wav", true, true);
	soundManager::Get()->addSound("02loadingScene2", Sounds + "LoadingScene2.wav", true, true);
	soundManager::Get()->addSound("03robotVoice1", Sounds + "RobotVoice1.wav",false,false);
	soundManager::Get()->addSound("04robotVoice2", Sounds + "RobotVoice2.wav", false, false);
	soundManager::Get()->addSound("05Select", Sounds + "Select.wav", false, false);
	soundManager::Get()->addSound("06Stage", Sounds + "Stage.wav", true, true);
	soundManager::Get()->addSound("07PlayerVoice", Sounds + "PlayerVoice.wav", false, false);
	soundManager::Get()->addSound("08Gun1", Sounds + "Gun1.wav",false,false);
	soundManager::Get()->addSound("09Gun2", Sounds + "Gun2.wav", false, false);
	soundManager::Get()->addSound("10Gun3", Sounds + "Gun3.wav", false, true);
	soundManager::Get()->addSound("11Reload", Sounds + "Reload.wav", false, false);
	soundManager::Get()->addSound("12ChangeGun", Sounds + "GunChange.wav", false, false);
	soundManager::Get()->addSound("13Walk", Sounds + "Walk.wav", false, true);
	soundManager::Get()->addSound("14Zoomin", Sounds + "Zoomin.wav", false, false);
	soundManager::Get()->addSound("15Zoomout", Sounds + "Zoomout.wav", false, false);
	soundManager::Get()->addSound("16NPC1", Sounds + "NPC1.wav", false, false);
	soundManager::Get()->addSound("17NPC2", Sounds + "NPC2.wav", false, false);
	soundManager::Get()->addSound("18NPC3", Sounds + "NPC3.wav", false, false);
	soundManager::Get()->addSound("19NPC4", Sounds + "NPC4.wav", false, false);
	soundManager::Get()->addSound("20NPC5", Sounds + "NPC5.wav", false, false);
	soundManager::Get()->addSound("21NPC6", Sounds + "NPC6.wav", false, false);
	soundManager::Get()->addSound("22QuestAccess", Sounds + "QuestAccess.flac", false, false);
	soundManager::Get()->addSound("23hit", Sounds + "hit.mp3", false, false);
	soundManager::Get()->addSound("24NearEnemyAtk", Sounds + "NearEnemyAtk.wav", false, false);
	soundManager::Get()->addSound("25NearEnemyHit", Sounds + "NearEnemyHit.wav", false, false);
	soundManager::Get()->addSound("26NearEnemyDeath", Sounds + "NearEnemyDeath.wav", false, false);
	soundManager::Get()->addSound("27PlayerHurt", Sounds + "PlayerHurt.wav", false, false);
	soundManager::Get()->addSound("28PlayerDeath", Sounds + "PlayerDeath.wav", false, false);
	soundManager::Get()->addSound("29NearEnemyFind", Sounds + "NearEnemyFind.wav", false, false);
	soundManager::Get()->addSound("30NPC7", Sounds + "NPC7.wav", false, false);
	soundManager::Get()->addSound("31NPC8", Sounds + "NPC8.wav", false, false);
	soundManager::Get()->addSound("32NPC9", Sounds + "NPC8.wav", false, false);
	soundManager::Get()->addSound("33BossEnter", Sounds + "BossEnter.wav", false, false);
	soundManager::Get()->addSound("34Clear", Sounds + "Clear.mp3", false, false);
	soundManager::Get()->addSound("35Ship", Sounds + "Ship.wav", false, true);
	soundManager::Get()->addSound("36Ship2", Sounds + "Ship2.wav", false, false);
	soundManager::Get()->addSound("37bip", Sounds + "bip.wav", false, false);
	soundManager::Get()->addSound("38Dash", Sounds + "dash.wav", false, false);
	soundManager::Get()->addSound("40hop", Sounds + "hop.wav", false, false);
	soundManager::Get()->addSound("41BossAttack1", Sounds + "BossAttack1.wav", false, false);
	soundManager::Get()->addSound("42BossAttack2", Sounds + "BossAttack2.wav", false, true);
	soundManager::Get()->addSound("43BossAttack3", Sounds + "BossAttack3.wav", false, false);
	soundManager::Get()->addSound("44BossBomb", Sounds + "BossBomb.wav", false, false);
	soundManager::Get()->addSound("45BossDead", Sounds + "BossDead.wav", false, false);
	soundManager::Get()->addSound("46BossHit", Sounds + "BossHit.wav", false, false);
	soundManager::Get()->addSound("47BossPage1", Sounds + "BossPage1.wav", false, false);
	soundManager::Get()->addSound("48BossPage1", Sounds + "BossPage2.wav", false, false);
	soundManager::Get()->addSound("49BossStart", Sounds + "BossStart.wav", false, false);
}
