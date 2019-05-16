#include "stdafx.h"
#include "LoadingScene2.h"
#include "Viewer\Freedom.h"
#include "Viewer\Follow.h"
#include "Environment\CubeSky.h"
#include "Environment\Terrain.h"
#include "Environment\Billboard.h"
#include "Environment\Rain.h"
#include "Environment\Rain.h"
#include "Environment\Shadow.h"
#include "Environment\ParticleSystem.h"
#include "Systems\Window.h"

void LoadingScene2::Initialize()
{
	Context::Get()->GetMainCamera()->RotationDegree(12, -45);
	Context::Get()->GetMainCamera()->Position(8,6, -8);

	//SceneManager::Get()->GerProgress()->GetProgressVal() = 0.2f;

	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(20.0f);
	((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(5.0f);

	//Window::UseGameMouse();
	mainCam = Context::Get()->GetMainCamera();
	follow = new Follow();
	Context::Get()->GetMainCamera() = follow;

	//SceneManager::Get()->GerProgress()->GetProgressVal() = 0.4f;

	value = 1;
	ship = new GameModel(0, Models + L"Ship/Ship.material", Models + L"Ship/Ship.mesh");
	highway = new GameModel(0, Models + L"HighWay/HighWay.material", Models + L"HighWay/HighWay.mesh");
	player = new GameAnimator(0, Models + L"Player/Player.material", Models + L"Player/Player.mesh");

	sky = new CubeSky(Textures+L"Space.dds");

	dcImage = new Render2D(L"023_mosaic.hlsl");

	startImage = new Texture(Textures + L"GameScene/black.png");

	float width = Context::Get()->GetViewport()->GetWidth();
	float height = Context::Get()->GetViewport()->GetHeight();

	//SceneManager::Get()->GerProgress()->GetProgressVal() = 0.8f;

	dcImage->SRV(startImage->SRV());
	dcImage->Position(0, 0);
	dcImage->Scale(width, height);

	size = 1;
	alpha = 1;
	alphaValue = 1;
	isStart = false;
	isNextStage = false;
	isNextStage2 = false;
	startTime = 0;
	nextStage = 0;
	nextTime = 0;

	//SceneManager::Get()->GerProgress()->GetProgressVal() = 1.0f;

	dcImage->GetEffect()->AsScalar("alphaValue")->SetInt(alpha);
	dcImage->GetEffect()->AsScalar("width")->SetFloat(width);
	dcImage->GetEffect()->AsScalar("height")->SetFloat(height);
}

void LoadingScene2::Ready()
{
	ship->Ready();
	ship->Position(0, 0, 0);
	ship->Scale(5, 5, 5);
	ship->Rotation(0, Math::ToRadian(-90), 0);

	highway->Ready();
	highway->Position(0, 0, 0);
	highway->Scale(200, 200, 200);

	player->Ready();
	player->Scale(2, 2, 2);
	player->RotationDegree(10, -180, 0);
	player->Position(0.3, 3.0f,-3);

	player->AddClip(Models + L"Player/Male Sitting Pose.anim",true);
	player->StartTargetClip(L" Male Sitting Pose.anim");
	player->Play();

	player->GetPlayClip()->Speed() = 1.5f;

	D3DXVECTOR3 camPos;
	Context::Get()->GetMainCamera()->Position(&camPos);
	moveVec = ship->GetPosition() - camPos;

	follow->InputInfo(&ship->GetPosition(), &ship->GetRotation());
	follow->GetSetY() = 5;
	follow->GetRotate() = D3DXVECTOR2(0.123f, -2.23f);
	follow->GetSetZ() = 15.2f;

}

void LoadingScene2::Destroy()
{
	Context::Get()->GetMainCamera() = mainCam;
	SAFE_DELETE(follow);
	InstanceManager::Get()->RemoveAllModel();
	SAFE_DELETE(dcImage);
	SAFE_DELETE(startImage);
	//SAFE_DELETE(player);
	//SAFE_DELETE(ship);
	//SAFE_DELETE(sky);

}

void LoadingScene2::Update()
{
	ShowCursor(false);
	ShowCursor(false);

	if(nextStage==0)
		UIManager::Get()->SetUiRender(L"Prog", true);
	else
		UIManager::Get()->SetUiRender(L"Prog", false);
	if (Keyboard::Get()->Press('Q'))
	{
		follow->GetSetZ() += Time::Get()->Delta();
	}
	if (Keyboard::Get()->Press('E'))
	{
		follow->GetSetZ() -= Time::Get()->Delta();
	}

	sky->Update();
	static float checkTime = 0;

	checkTime += Time::Get()->Delta();

	player->Update();
	ship->Update();

	D3DXVECTOR3 curShip,curPlayer;
	ship->Position(&curShip);
	player->Position(&curPlayer);

	curShip.y += value*Time::Get()->Delta();
	curPlayer.y+= value*Time::Get()->Delta();

	ship->Position(curShip);
	player->Position(curPlayer);

	if (checkTime > 1)
	{
		value *= -1;
		checkTime = 0;
	}

	if (Keyboard::Get()->Down(VK_RETURN))
	{
		if (nextStage == 0)
			soundManager::Get()->play("03robotVoice1", 0.5f);
		else if(nextStage==1)
			soundManager::Get()->play("07PlayerVoice", 0.5f);
		else if (nextStage == 2)
			soundManager::Get()->play("04robotVoice2", 0.5f);
		isNextStage2 = false;
		isNextStage = false;
		nextStage++;
		alpha = 0;
	}
	
	dcImage->GetEffect()->AsScalar("alphaValue")->SetFloat(alpha);
	
	if (nextStage == 1)
	{
		UIManager::Get()->SetUiRender(L"event11", true);
		UIManager::Get()->SetUiRender(L"event12", false);
		UIManager::Get()->SetUiRender(L"event13", false);
	}
	else if (nextStage == 2)
	{
		UIManager::Get()->SetUiRender(L"event11", false);
		UIManager::Get()->SetUiRender(L"event12", true);
		UIManager::Get()->SetUiRender(L"event13", false);
	}
	else if (nextStage == 3)
	{
		UIManager::Get()->SetUiRender(L"event11", false);
		UIManager::Get()->SetUiRender(L"event12", false);
		UIManager::Get()->SetUiRender(L"event13", true);
	}

	if (nextStage < 3)
	{
		startTime += Time::Get()->Delta();
		if (startTime > 10)
		{
			if (alpha > 0)
				alpha -= Time::Get()->Delta()*alphaValue*0.3f;
		}
	}
	else
	{
		if (!isNextStage)
		{
			curq = follow->GetRotate();

			curq.y += Time::Get()->Delta()*0.5f;
			curq.x+= Time::Get()->Delta()*0.05f;
			follow->GetSetZ() += Time::Get()->Delta()*5;

			follow->GetRotate() = curq;
			if (curq.y > -1.55f)
			{
				isNextStage = true;
			}
		}
		else
		{
			ParticleManager::Get()->Shot(L"buster1", ship->GetPosition() + D3DXVECTOR3(-0.8, 4.5, -7), -ship->Right());
			ParticleManager::Get()->Shot(L"buster2", ship->GetPosition() + D3DXVECTOR3(0.8, 4.5, -7), -ship->Right());
			if (!isNextStage2)
			{
				nextTime += Time::Get()->Delta();

				if (nextTime > 3.5f)
					isNextStage2 = true;

				if (nextTime < 1.5f)
				{
					D3DXVECTOR3 curPos1;
					ship->Position(&curPos1);

					curPos1.z += Time::Get()->Delta()*25.5f;
					ship->Position(curPos1);

					D3DXVECTOR3 curPos2;
					player->Position(&curPos2);

					curPos2.z += Time::Get()->Delta()*25.5f;
					player->Position(curPos2);
				}
				else
				{
					D3DXVECTOR3 curPos1;
					ship->Position(&curPos1);

					curPos1.z += Time::Get()->Delta()*40.5f;
					ship->Position(curPos1);

					D3DXVECTOR3 curPos2;
					player->Position(&curPos2);

					curPos2.z += Time::Get()->Delta()*40.5f;
					player->Position(curPos2);
				}
				
			}
			else
			{
				UIManager::Get()->SetUiRender(L"event13", false);
				D3DXVECTOR3 curPos1;
				ship->Position(&curPos1);

				curPos1.z += Time::Get()->Delta()*40.5f;
				ship->Position(curPos1);

				D3DXVECTOR3 curPos2;
				player->Position(&curPos2);

				curPos2.z += Time::Get()->Delta()*40.5f;
				player->Position(curPos2);

				alpha += Time::Get()->Delta()*alphaValue*0.3f;

				if (alpha > 1)
				{
					SceneManager::Get()->ChangeScene(SceneManager::SCENETYPE::Stage1);
				}
			}
		}
	}
}

void LoadingScene2::PreRender()
{
	sky->PreRender();
}

void LoadingScene2::Render()
{
	InstanceManager::Get()->Render(0);

	sky->Render();
}

void LoadingScene2::PostRender()
{
	dcImage->Render();
}

void LoadingScene2::ResizeScreen()
{
}
