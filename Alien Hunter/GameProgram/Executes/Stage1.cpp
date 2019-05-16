#include "stdafx.h"
#include "Stage1.h"
#include "Viewer\Freedom.h"
#include "Environment\CubeSky.h"
#include "Environment\Terrain.h"
#include "Environment\Billboard.h"
#include "Environment\Rain.h"
#include "Environment\Rain.h"
#include "Environment\Shadow.h"
#include "Environment\ParticleSystem.h"
#include "Systems\Window.h"

void Stage1::Initialize()
{
	Context::Get()->GetMainCamera()->RotationDegree(18, -40);
	Context::Get()->GetMainCamera()->Position(9, 12, -12);

	((Freedom*)Context::Get()->GetMainCamera())->MoveSpeed(20.0f);
	((Freedom*)Context::Get()->GetMainCamera())->RotationSpeed(5.0f);

	ObjectManager::Create();
	Window::UseGameMouse();
	mainCam = Context::Get()->GetMainCamera();
	tpsCam = new TPSCamera();
	Context::Get()->GetMainCamera() = tpsCam;

	FileManager::Get()->Load(Datas + L"Scene.scene", true);

	npc = new GameAnimator(0, Models + L"NPC/NPC.material", Models + L"NPC/NPC.mesh");
	ship = new GameModel(0, Models + L"Ship2/Ship2.material", Models + L"Ship2/Ship2.mesh");


	isStageStart = false;
	clearStageTime = 0;
	reStageTime = 0;
	uiInit = false;
	onceFade = false;
	eState = PRESTAGE;
}

void Stage1::Ready()
{
	ObjectManager::Get()->Ready();
	tpsCam->InputInfo
	(
		&((Player*)ObjectManager::Get()->GetPlayer())->GetCamPosition(),
		&((Player*)ObjectManager::Get()->GetPlayer())->GetCamRotation()
	);

	((Player*)ObjectManager::Get()->GetPlayer())->SetCamera(tpsCam);

	npc->Ready();
	npc->AddClip(Models + L"NPC/Dwarf Idle.anim", true);
	npc->StartTargetClip(L" Dwarf Idle.anim");
	npc->Play();
	npc->Position(2.075f, 0, 136.597f);
	npc->Scale(2.5, 2.5, 2.5);
	ObjectManager::Get()->GetShadow()->Add(npc);

	ship->Ready();
	ship->Position(103,42.173, -43.325f);
	ship->Scale(16, 16, 16);
	ship->Rotation(0, Math::ToRadian(-115), 0);
	ObjectManager::Get()->GetShadow()->Add(ship);

	gateOpos= ObjectManager::Get()->GetModel(L"Gate",0)->GetPosition();

	for (int i = 0; i < 9; i++)
		InstanceManager::Get()->FindAnim(L"NearEnemy.mesh")->SetRenderState(i, false);

	InstanceManager::Get()->FindAnim(L"Boss.mesh")->SetRenderState(0, false);
	soundManager::Get()->stop("02loadingScene2");
	soundManager::Get()->play("06Stage",0.7f);
}

void Stage1::Destroy()
{
	Context::Get()->GetMainCamera() = mainCam;
	SAFE_DELETE(tpsCam);

	ObjectManager::Delete();
	InstanceManager::Get()->RemoveAllModel();
}

void Stage1::Update()
{
	ShowCursor(false);
	ShowCursor(false);

	EnemyTargetUpdate();
	NpcUpdate();
	StageUpdate();

	if (!uiInit)
	{
		UIManager::Get()->SetUiRender(L"helmet", true);
		for (int i = 1; i <= 8; i++)
			UIManager::Get()->SetUiRender(L"hp" + to_wstring(i), true);
		UIManager::Get()->SetUiRender(L"healthback", true);
		UIManager::Get()->SetUiRender(L"healthmiddle", true);

		uiInit = true;
	}
	ObjectManager::Get()->Update();

	if (eState != ENDING)
	{
		if (((Player*)ObjectManager::Get()->GetPlayer())->IsDie())
		{
			if (!onceFade)
			{
				UIManager::Get()->UseFadeOut(1.5f);
				onceFade = true;
			}
			UIManager::Get()->SetUiRender(L"gameoverText", true);

			if (UIManager::Get()->EndFadeOut())
			{
				onceFade = true;
				UIManager::Get()->SetUiRender(L"gameoverText", false);
				UIManager::Get()->ResetFadeOut();
				SceneManager::Get()->ChangeScene(SceneManager::SCENETYPE::Stage1);
			}
		}
	}
}

void Stage1::PreRender()
{
	ObjectManager::Get()->PreRender();
}

void Stage1::Render()
{
	ObjectManager::Get()->Render();
}

void Stage1::PostRender()
{
	ObjectManager::Get()->PostRender();
}

void Stage1::ResizeScreen()
{
}

void Stage1::NpcUpdate()
{
	npc->Update();

	D3DXVECTOR3 playerPos;
	ObjectManager::Get()->GetPlayer()->Position(&playerPos);

	if (Math::Distance(playerPos, npc->GetPosition()) < 15)
	{
		D3DXVECTOR3 toPlayerVec = playerPos - npc->GetPosition();
		float angle=Math::VectorToAngle(-npc->Direction(), toPlayerVec);

		D3DXVECTOR3 curAng;
		npc->Rotation(&curAng);

		if (angle < 0)
		{
			curAng.y -= Time::Get()->Delta()*2.5f;
		}
		else
		{
			curAng.y += Time::Get()->Delta()*2.5f;
		}
		npc->Rotation(curAng);
	}
	
	if (Math::Distance(playerPos, npc->GetPosition()) < 7)
	{
		if(preStageCount==0)
			UIManager::Get()->SetUiRender(L"talk", true);
	}
	else
	{
		UIManager::Get()->SetUiRender(L"talk", false);
		if (preStageCount != 6&& preStageCount != 3)
		{
			UIManager::Get()->SetUiRender(L"event1", false);
			UIManager::Get()->SetUiRender(L"event2", false);
			UIManager::Get()->SetUiRender(L"event3", false);
			UIManager::Get()->SetUiRender(L"event4", false);
			UIManager::Get()->SetUiRender(L"event5", false);
			preStageCount = 0;
		}
	}
}

void Stage1::StageUpdate()
{
	switch (eState)
	{
		case Stage1::PRESTAGE:
		{
			if (UIManager::Get()->GetUiRenderState(L"talk"))
			{
				if (Keyboard::Get()->Down(VK_RETURN))
					isStageStart = true;
			}

			if (preStageCount == 0)
			{
				if (isStageStart)
				{
					preStageCount++;
					soundManager::Get()->play("16NPC1");
				}
			}
			else if (preStageCount == 1)
			{
				UIManager::Get()->SetUiRender(L"event1", true);
				UIManager::Get()->SetUiRender(L"event2", false);
				UIManager::Get()->SetUiRender(L"event3", false);
				UIManager::Get()->SetUiRender(L"event4", false);
				UIManager::Get()->SetUiRender(L"event5", false);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					soundManager::Get()->stop("16NPC1");
					soundManager::Get()->play("17NPC2");
					preStageCount++;
				}
			}
			else if (preStageCount == 2)
			{
				UIManager::Get()->SetUiRender(L"event1", false);
				UIManager::Get()->SetUiRender(L"event2", true);
				UIManager::Get()->SetUiRender(L"event3", false);
				UIManager::Get()->SetUiRender(L"event4", false);
				UIManager::Get()->SetUiRender(L"event5", false);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					soundManager::Get()->stop("17NPC2");
					soundManager::Get()->play("18NPC3");
					preStageCount++;
				}
			}
			else if (preStageCount == 3)
			{
				UIManager::Get()->SetUiRender(L"event1", false);
				UIManager::Get()->SetUiRender(L"event2", false);
				UIManager::Get()->SetUiRender(L"event3", true);
				UIManager::Get()->SetUiRender(L"event4", false);
				UIManager::Get()->SetUiRender(L"event5", false);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					soundManager::Get()->stop("18NPC3");
					soundManager::Get()->play("19NPC4");
					preStageCount++;
				}
			}
			else if (preStageCount == 4)
			{
				UIManager::Get()->SetUiRender(L"event1", false);
				UIManager::Get()->SetUiRender(L"event2", false);
				UIManager::Get()->SetUiRender(L"event3", false);
				UIManager::Get()->SetUiRender(L"event4", true);
				UIManager::Get()->SetUiRender(L"event5", false);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					soundManager::Get()->stop("19NPC4");
					soundManager::Get()->play("20NPC5");
					preStageCount++;
				}
			}
			else if (preStageCount == 5)
			{
				UIManager::Get()->SetUiRender(L"event1", false);
				UIManager::Get()->SetUiRender(L"event2", false);
				UIManager::Get()->SetUiRender(L"event3", false);
				UIManager::Get()->SetUiRender(L"event4", false);
				UIManager::Get()->SetUiRender(L"event5", true);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					soundManager::Get()->stop("20NPC5");
					soundManager::Get()->play("22QuestAccess");
					soundManager::Get()->play("35Ship",0.3f);
					preStageCount++;
				}
			}

			if (preStageCount == 6)
			{
				UIManager::Get()->SetUiRender(L"event1", false);
				UIManager::Get()->SetUiRender(L"event2", false);
				UIManager::Get()->SetUiRender(L"event3", false);
				UIManager::Get()->SetUiRender(L"event4", false);
				UIManager::Get()->SetUiRender(L"event5", true);

				{
					float width = Context::Get()->GetViewport()->GetWidth();
					float height = Context::Get()->GetViewport()->GetHeight();

					D3DXVECTOR2 uPos;
					UIManager::Get()->GetRender2D(L"event5")->Position(&uPos);
					
					D3DXVECTOR2 uScale;
					UIManager::Get()->GetRender2D(L"event5")->Scale(&uScale);

					if (uPos.x < width*0.05f)
						uPos.x += Time::Get()->Delta()*150;
					
					if (uPos.y < height*0.78f)
						uPos.y += Time::Get()->Delta()*150;

					if (uScale.x > width / 5)
						uScale.x -= Time::Get()->Delta()*150;

					if (uScale.y > width / 9.5)
						uScale.y -= Time::Get()->Delta()*150;

					UIManager::Get()->GetRender2D(L"event5")->Position(uPos);
					UIManager::Get()->GetRender2D(L"event5")->Scale(uScale);
				}
				

				D3DXVECTOR3 pos;
				ObjectManager::Get()->GetModel(L"AngleST", 0)->Position(&pos);
				ParticleManager::Get()->Shot(L"buster3", D3DXVECTOR3(pos.x, pos.y+23, pos.z), D3DXVECTOR3(0, 1, 0));

				ObjectManager::Get()->GetModel(L"AngleST", 1)->Position(&pos);
				ParticleManager::Get()->Shot(L"buster4", D3DXVECTOR3(pos.x, pos.y +23, pos.z), D3DXVECTOR3(0, 1, 0));


				if (pos.y > -20)
				{
					ObjectManager::Get()->GetModel(L"Gate", 0)->Position(&pos);

					pos.y -= Time::Get()->Delta() * 8;
					ObjectManager::Get()->GetModel(L"Gate", 0)->Position(pos);
				}

				ship->Position(&pos);
				pos.y -= Time::Get()->Delta()*4;
				ship->Position(pos);

				if (pos.y < 0)
				{
					for (int i = 0; i < 9; i++)
						InstanceManager::Get()->FindAnim(L"NearEnemy.mesh")->SetRenderState(i, true);

					soundManager::Get()->play("36Ship2",0.3f);
					UIManager::Get()->SetUiRender(L"stage1", true);
					//preStageCount = 0;
					isStageStart = false;
					eState = STAGE1;
				}
			}
		}
		break;
		case Stage1::STAGE1:
		{
			{
				D3DXVECTOR3 pos;
				ship->Position(&pos);
				if (pos.y < 42.173f)
				{
					pos.y += Time::Get()->Delta() * 6;
					ship->Position(pos);
				}
				else
				{
					soundManager::Get()->stop("35Ship");
				}
			}


			if (InstanceManager::Get()->GetIsRenderAnim(L"NearEnemy.mesh") == 0)
			{
				float width = Context::Get()->GetViewport()->GetWidth();
				float height = Context::Get()->GetViewport()->GetHeight();
				UIManager::Get()->GetRender2D(L"event5")->Position(0, height*0.37f);
				UIManager::Get()->GetRender2D(L"event5")->Scale(width / 2.5f, width / 5);
				UIManager::Get()->SetUiRender(L"event5", false);
				UIManager::Get()->SetUiRender(L"event6", true);
				preStageCount = 0;
				UIManager::Get()->SetUiRender(L"stage1", false);
				eState = PRESTAGE2;
			}

		}
		break;
		case Stage1::PRESTAGE2:
		{
			if (UIManager::Get()->GetUiRenderState(L"talk"))
			{
				if (Keyboard::Get()->Down(VK_RETURN))
					isStageStart = true;
			}

			if (preStageCount == 0)
			{
				if (isStageStart)
				{
					preStageCount++;
					soundManager::Get()->play("30NPC7");
				}
			}
			else if (preStageCount == 1)
			{
				UIManager::Get()->SetUiRender(L"event6", false);
				UIManager::Get()->SetUiRender(L"event7", true);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					preStageCount++;
					soundManager::Get()->stop("30NPC7");
					soundManager::Get()->play("31NPC8");
				}
			}
			else if (preStageCount == 2)
			{
				UIManager::Get()->SetUiRender(L"event6", false);
				UIManager::Get()->SetUiRender(L"event7", false);
				UIManager::Get()->SetUiRender(L"event8", true);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					preStageCount++;
					soundManager::Get()->stop("31NPC8");
					soundManager::Get()->stop("06Stage");
					soundManager::Get()->play("33BossEnter", 0.7f);
					soundManager::Get()->play("35Ship",0.3f);

				}
			}
			else if (preStageCount == 3)
			{
				UIManager::Get()->SetUiRender(L"event6", false);
				UIManager::Get()->SetUiRender(L"event7", false);
				UIManager::Get()->SetUiRender(L"event8", true);
				
				{
					float width = Context::Get()->GetViewport()->GetWidth();
					float height = Context::Get()->GetViewport()->GetHeight();

					D3DXVECTOR2 uPos;
					UIManager::Get()->GetRender2D(L"event8")->Position(&uPos);

					D3DXVECTOR2 uScale;
					UIManager::Get()->GetRender2D(L"event8")->Scale(&uScale);

					if (uPos.x < width*0.05f)
						uPos.x += Time::Get()->Delta() * 150;

					if (uPos.y < height*0.78f)
						uPos.y += Time::Get()->Delta() * 150;

					UIManager::Get()->GetRender2D(L"event8")->Position(uPos);

					if (uScale.x > width / 5)
						uScale.x -= Time::Get()->Delta() * 150;

					if (uScale.y > width / 9.5)
						uScale.y -= Time::Get()->Delta() * 150;

					UIManager::Get()->GetRender2D(L"event8")->Scale(uScale);
				}

				D3DXVECTOR3 pos;
				ObjectManager::Get()->GetModel(L"AngleST", 0)->Position(&pos);
				ParticleManager::Get()->Shot(L"buster3", D3DXVECTOR3(pos.x, pos.y + 23, pos.z), D3DXVECTOR3(0, 1, 0));

				ObjectManager::Get()->GetModel(L"AngleST", 1)->Position(&pos);
				ParticleManager::Get()->Shot(L"buster4", D3DXVECTOR3(pos.x, pos.y + 23, pos.z), D3DXVECTOR3(0, 1, 0));

				ship->Position(&pos);
				pos.y -= Time::Get()->Delta() * 4;
				ship->Position(pos);

				if (pos.y < 0)
				{
					InstanceManager::Get()->FindAnim(L"Boss.mesh")->SetRenderState(0, true);
					//여기다 보스몹그리는거 추가
					soundManager::Get()->play("36Ship2", 0.3f);
					UIManager::Get()->SetUiRender(L"stage2", true);
					//preStageCount = 0;
					isStageStart = false;
					eState = STAGE2;
				}
			}
		}
		break;
		case Stage1::STAGE2:
		{
			{
				D3DXVECTOR3 pos;
				ship->Position(&pos);
				if (pos.y < 42.173f)
				{
					pos.y += Time::Get()->Delta() * 6;
					ship->Position(pos);
				}
				else
				{
					soundManager::Get()->stop("35Ship");
				}
			}

			if (Keyboard::Get()->Down('P'))
			{
				float width = Context::Get()->GetViewport()->GetWidth();
				float height = Context::Get()->GetViewport()->GetHeight();
				UIManager::Get()->GetRender2D(L"event8")->Position(0, height*0.37f);
				UIManager::Get()->GetRender2D(L"event8")->Scale(width / 2.5f, width / 5);
				UIManager::Get()->SetUiRender(L"event8", false);
				UIManager::Get()->SetUiRender(L"event6", true);
				preStageCount = 0;
				UIManager::Get()->SetUiRender(L"stage2", false);
				eState = PREENDING;
				soundManager::Get()->stop("33BossEnter");
				soundManager::Get()->play("34Clear",0.5f);
			}

			if (InstanceManager::Get()->GetIsRenderAnim(L"Boss.mesh") == 0)
			{
				float width = Context::Get()->GetViewport()->GetWidth();
				float height = Context::Get()->GetViewport()->GetHeight();
				UIManager::Get()->GetRender2D(L"event8")->Position(0, height*0.37f);
				UIManager::Get()->GetRender2D(L"event8")->Scale(width / 2.5f, width / 5);
				UIManager::Get()->SetUiRender(L"event8", false);
				UIManager::Get()->SetUiRender(L"event6", true);
				preStageCount = 0;
				UIManager::Get()->SetUiRender(L"stage2", false);
				eState = PREENDING;
				soundManager::Get()->stop("33BossEnter");
				soundManager::Get()->play("34Clear", 0.5f);
			}
		}
		break;
		case Stage1::PREENDING:
		{
			if (UIManager::Get()->GetUiRenderState(L"talk"))
			{
				if (Keyboard::Get()->Down(VK_RETURN))
					isStageStart = true;
			}


			if (preStageCount == 0)
			{
				if (isStageStart)
				{
					soundManager::Get()->play("30NPC7");
					preStageCount++;
				}
			}
			else if (preStageCount == 1)
			{
				UIManager::Get()->SetUiRender(L"event6", false);
				UIManager::Get()->SetUiRender(L"event9", true);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					soundManager::Get()->stop("30NPC7");
					soundManager::Get()->play("31NPC8");
					preStageCount++;
				}
			}
			else if (preStageCount == 2)
			{
				UIManager::Get()->SetUiRender(L"event6", false);
				UIManager::Get()->SetUiRender(L"event9", false);
				UIManager::Get()->SetUiRender(L"event10", true);
				if (Keyboard::Get()->Down(VK_RETURN))
				{
					soundManager::Get()->stop("31NPC8");
					preStageCount++;
				}
			}
			else if (preStageCount == 3)
			{
				UIManager::Get()->SetUiRender(L"event6", false);
				UIManager::Get()->SetUiRender(L"event9", false);
				UIManager::Get()->SetUiRender(L"event10",true);
				
				eState = ENDING;
				//nextStageTime += Time::Get()->Delta();
			}
		}
		break;
		case Stage1::ENDING:
		{
			if (!onceFade)
			{
				UIManager::Get()->UseFadeOut(1.5f);
				onceFade = true;
			}

			if (UIManager::Get()->EndFadeOut())
			{
				onceFade = true;
				UIManager::Get()->SetUiRender(L"event10", false);
				UIManager::Get()->SetUiRender(L"helmet", false);
				for (int i = 1; i <= 8; i++)
					UIManager::Get()->SetUiRender(L"hp" + to_wstring(i), false);
				UIManager::Get()->SetUiRender(L"healthback", false);
				UIManager::Get()->SetUiRender(L"healthmiddle", false);
				UIManager::Get()->SetUiRender(L"gun1", false);
				UIManager::Get()->SetUiRender(L"gun2", false);
				UIManager::Get()->SetUiRender(L"gun3", false);
				UIManager::Get()->SetUiRender(L"bullet3", false);
				UIManager::Get()->SetUiRender(L"bullet1_1", false);
				UIManager::Get()->SetUiRender(L"bullet1_2", false);
				UIManager::Get()->SetUiRender(L"cross1", false);
				UIManager::Get()->SetUiRender(L"cross2", false);
				UIManager::Get()->SetUiRender(L"cross3", false);

				UIManager::Get()->SetUiRender(L"bullet2_1", false);
				UIManager::Get()->SetUiRender(L"bullet2_2", false);
				UIManager::Get()->SetUiRender(L"bullet2_3", false);
				UIManager::Get()->SetUiRender(L"bullet2_4", false);
				UIManager::Get()->SetUiRender(L"bullet2_5", false);
				UIManager::Get()->SetUiRender(L"bullet2_6", false);
				UIManager::Get()->SetUiRender(L"bullet2_7", false);
				UIManager::Get()->SetUiRender(L"bullet2_8", false);
				UIManager::Get()->SetUiRender(L"bullet2_9", false);
				UIManager::Get()->SetUiRender(L"bullet2_10", false);

				UIManager::Get()->SetUiRender(L"talk", false);

				UIManager::Get()->ResetFadeOut();
				soundManager::Get()->stop("34Clear");
				soundManager::Get()->stop("36Ship2");
				SceneManager::Get()->ChangeScene(SceneManager::SCENETYPE::EndScene);
			}
		}
		break;
		case Stage1::FAIL:
		{

		}
		break;
	}
}

void Stage1::EnemyTargetUpdate()
{
	// 플레이어가 줌상태고 줌상태에서 2d화면공간에 에너미의 위치가 들어오면
	//1. 플레이어가 줌상태인지 본다.
	//2. 에너미들을 전부확인한다.
	//3. 줌상태라면 에너미의 3d좌표를 2d좌표로 바꿔준다.
	//4. 2d좌표상에 일정범위에 들어가면 표적ui를 그려주고 아니면 그리지 않는다.

	// 에너미들을 모든경우 검사해서 vector에 담은다음 가운데좌표차이가 가장적은걸로
	// 정렬하여 첫번째 요소에 해당하는 유아이만 뺀다.

	//예외처리 - 플레이어와의 거리, 화면에 렌더아닌애들은 검사x

	vector<ModelUI> renderUImodel;

	if (((Player*)ObjectManager::Get()->GetPlayer())->IsZoom())
	{
		renderUImodel.clear();
		for (size_t i = 0; i < ObjectManager::Get()->GetModelVector(L"NearEnemy").size(); i++)
		{
			if (!InstanceManager::Get()->FindAnim(L"NearEnemy.mesh")->GetRenderState(i))continue;

			//에너미와 플레이어거리 따진다
			float distance = Math::Distance(ObjectManager::Get()->GetPlayer()->GetPosition(), ObjectManager::Get()->GetModel(L"NearEnemy", i)->GetPosition());

			if (distance > 50)continue;

			//월드 뷰 프로젝션 이차원 좌표를 구해야함
			D3DXMATRIX V, P;
			Context::Get()->GetMainCamera()->Matrix(&V);
			Context::Get()->GetPerspective()->GetMatrix(&P);

			D3DXMATRIX matrix;
			D3DXMatrixMultiply(&matrix, &V, &P);
			D3DXMATRIX WVP = ObjectManager::Get()->GetModel(L"NearEnemy", i)->World()*matrix;
			
			D3DXVECTOR4 worldToScreen(0, 0, 0, 1);
			D3DXVec4Transform(&worldToScreen, &worldToScreen, &WVP);

			//NDC 공간으로 변환
			float wScreenX = worldToScreen.x / worldToScreen.w;
			float wScreenY = worldToScreen.y / worldToScreen.w;
			float wScreenZ = worldToScreen.z / worldToScreen.w;

			//-1~1 구간을 0~1구간으로 변환
			float nScreenX = (wScreenX + 1)*0.5f;
			float nScreenY = (wScreenY + 1)*0.5f;

			//최종화면의 좌표
			float width= Context::Get()->GetViewport()->GetWidth();
			float height= Context::Get()->GetViewport()->GetHeight();

			float resultX = nScreenX*width;
			float resultY = nScreenY*height;

			// 좌표가 일정범위안에 들어오는지 체크
			if ((width*0.4f)<resultX&&(width*0.53f)>resultX)
			{
				//거리 비율은 0~49사이로 둔다 그걸기준으로 크기를 비율에 맞춘다
				//그럼 1/50 초반크기는 몇일까?
				float startScale = width *0.15f;

				//distance는 0~50사이 실수
				float scale = startScale*(1/distance)*25;

				float startHpScaleX = width *0.1f;
				float startHpScaleY = width *0.011f;

				float scaleHpX = startHpScaleX*(1 / distance) * 25;
				float scaleHpY = startHpScaleY*(1 / distance) * 25;

				float setX = resultX - (scale/2.0f);
				float setY = resultY - (scale/4.0f);

				float sethpX = resultX - (scaleHpX / 2.0f);
				float sethpY = resultY + scaleHpY*9.5f;

				ModelUI mUi = { (NearEnemy*)ObjectManager::Get()->GetModel(L"NearEnemy", i), setX,setY,scale,scale,sethpX,sethpY,scaleHpX, scaleHpY };
				
				renderUImodel.push_back(mUi);
				
				//정렬시킨다 중심과의 거리기준으로
				for (int k = 0; k < renderUImodel.size() - 1; k++)
				{
					for (int j = 0; j < (renderUImodel.size() - 1) - k; j++)
					{
						/*float dis1 = fabsf(width*0.46 - renderUImodel[j].x);
						float dis2 = fabsf(width*0.46 - renderUImodel[j+1].x);*/

						float dis1 = Math::Distance(renderUImodel[j].enemy->GetPosition(), ObjectManager::Get()->GetPlayer()->GetPosition());
						float dis2 = Math::Distance(renderUImodel[j+1].enemy->GetPosition(), ObjectManager::Get()->GetPlayer()->GetPosition());

						if (dis1 > dis2)
						{
							ModelUI temp = renderUImodel[j];
							renderUImodel[j] = renderUImodel[j + 1];
							renderUImodel[j + 1] = temp;
						}
					}
				}
			}
			else
				continue;
		}

		for (size_t i = 0; i < ObjectManager::Get()->GetModelVector(L"Boss").size(); i++)
		{
			if (!InstanceManager::Get()->FindAnim(L"Boss.mesh")->GetRenderState(i))continue;

			//에너미와 플레이어거리 따진다
			float distance = Math::Distance(ObjectManager::Get()->GetPlayer()->GetPosition(), ObjectManager::Get()->GetModel(L"Boss", i)->GetPosition());

			if (distance > 50)continue;

			//월드 뷰 프로젝션 이차원 좌표를 구해야함
			D3DXMATRIX V, P;
			Context::Get()->GetMainCamera()->Matrix(&V);
			Context::Get()->GetPerspective()->GetMatrix(&P);

			D3DXMATRIX matrix;
			D3DXMatrixMultiply(&matrix, &V, &P);
			D3DXMATRIX WVP = ObjectManager::Get()->GetModel(L"Boss", i)->World()*matrix;

			D3DXVECTOR4 worldToScreen(0, 0, 0, 1);
			D3DXVec4Transform(&worldToScreen, &worldToScreen, &WVP);

			//NDC 공간으로 변환
			float wScreenX = worldToScreen.x / worldToScreen.w;
			float wScreenY = worldToScreen.y / worldToScreen.w;
			float wScreenZ = worldToScreen.z / worldToScreen.w;

			//-1~1 구간을 0~1구간으로 변환
			float nScreenX = (wScreenX + 1)*0.5f;
			float nScreenY = (wScreenY + 1)*0.5f;

			//최종화면의 좌표
			float width = Context::Get()->GetViewport()->GetWidth();
			float height = Context::Get()->GetViewport()->GetHeight();

			float resultX = nScreenX*width;
			float resultY = nScreenY*height;

			// 좌표가 일정범위안에 들어오는지 체크
			if ((width*0.4f)<resultX && (width*0.53f)>resultX)
			{
				//거리 비율은 0~49사이로 둔다 그걸기준으로 크기를 비율에 맞춘다
				//그럼 1/50 초반크기는 몇일까?
				float startScale = width *0.15f;

				//distance는 0~50사이 실수
				float scale = startScale*(1 / distance) * 25;

				float startHpScaleX = width *0.1f;
				float startHpScaleY = width *0.011f;

				float scaleHpX = startHpScaleX*(1 / distance) * 25;
				float scaleHpY = startHpScaleY*(1 / distance) * 25;

				float setX = resultX - (scale / 2.0f);
				float setY = resultY - (scale / 4.0f);

				float sethpX = resultX - (scaleHpX / 2.0f);
				float sethpY = resultY + scaleHpY*9.5f;

				ModelUI mUi = { (NearEnemy*)ObjectManager::Get()->GetModel(L"Boss", i), setX,setY,scale,scale,sethpX,sethpY,scaleHpX, scaleHpY };

				renderUImodel.push_back(mUi);

				//정렬시킨다 중심과의 거리기준으로
				for (int k = 0; k < renderUImodel.size() - 1; k++)
				{
					for (int j = 0; j < (renderUImodel.size() - 1) - k; j++)
					{
						/*float dis1 = fabsf(width*0.46 - renderUImodel[j].x);
						float dis2 = fabsf(width*0.46 - renderUImodel[j+1].x);*/

						float dis1 = Math::Distance(renderUImodel[j].enemy->GetPosition(), ObjectManager::Get()->GetPlayer()->GetPosition());
						float dis2 = Math::Distance(renderUImodel[j + 1].enemy->GetPosition(), ObjectManager::Get()->GetPlayer()->GetPosition());

						if (dis1 > dis2)
						{
							ModelUI temp = renderUImodel[j];
							renderUImodel[j] = renderUImodel[j + 1];
							renderUImodel[j + 1] = temp;
						}
					}
				}
			}
			else
				continue;
		}
		if (renderUImodel.size() != 0)
		{
			if(renderUImodel[0].enemy!=preUi.enemy)
				soundManager::Get()->play("37bip");
			//UIManager::Get()->SetUiRender(L"1outhealth", true);
			UIManager::Get()->GetRender2D(L"1outhealth")->Position(renderUImodel[0].hpX, renderUImodel[0].hpY);
			UIManager::Get()->GetRender2D(L"1outhealth")->Scale(renderUImodel[0].hpScaleX, renderUImodel[0].hpScaleY);

			float hpRatio = (float)renderUImodel[0].enemy->Hp() / (float)renderUImodel[0].enemy->InitHp();
			UIManager::Get()->SetUiRender(L"0inhealth", true);
			UIManager::Get()->GetRender2D(L"0inhealth")->Position(renderUImodel[0].hpX, renderUImodel[0].hpY);
			UIManager::Get()->GetRender2D(L"0inhealth")->Scale(renderUImodel[0].hpScaleX*hpRatio, renderUImodel[0].hpScaleY*0.7f);

			UIManager::Get()->SetUiRender(L"search2", true);
			UIManager::Get()->GetRender2D(L"search2")->Position(renderUImodel[0].x, renderUImodel[0].y);
			UIManager::Get()->GetRender2D(L"search2")->Scale(renderUImodel[0].scaleX, renderUImodel[0].scaleY);

			preUi = renderUImodel[0];
		}
		else
		{
			UIManager::Get()->SetUiRender(L"1outhealth", false);
			UIManager::Get()->SetUiRender(L"0inhealth", false);
			UIManager::Get()->SetUiRender(L"search2", false);
			preUi.enemy = NULL;
		}
	}
	else
	{
		UIManager::Get()->SetUiRender(L"1outhealth", false);
		UIManager::Get()->SetUiRender(L"0inhealth", false);
		UIManager::Get()->SetUiRender(L"search2", false);
		preUi.enemy = NULL;
	}
}


