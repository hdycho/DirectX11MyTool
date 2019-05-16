#include "Framework.h"
#include "GameAnimator.h"
#include "Renders\TrailRenderer.h"


GameAnimator::GameAnimator(Effect*lineEffect)
	:GameModel(lineEffect),
	mode(Mode::Stop)
	, currentKeyframe(0)
	, nextKeyframe(0)
	, currentClipName(L"")
	, frameTime(0.0f)
	, time(0.0f)
	, blendStartTime(0.0f)
	, isInit(false)
	, isBlending(false)
	, isFirstModelName(false)
	, changeInplace(false)
{
	typeName = L"Animation";
	pass = 1;
}

GameAnimator::GameAnimator(UINT instanceId, wstring materialFile, wstring meshFile, Effect*lineEffect)
	:GameModel(instanceId, materialFile, meshFile, lineEffect)
	, mode(Mode::Stop)
	, currentKeyframe(0)
	, nextKeyframe(0)
	, currentClipName(L"")
	, frameTime(0.0f)
	, time(0.0f)
	, blendStartTime(0.0f)
	, isInit(false)
	, isBlending(false)
	, isFirstModelName(false)
	, changeInplace(false)
{
	typeName = L"Animation";
	pass = 1;
}

GameAnimator::GameAnimator(GameAnimator & copy)
	:GameModel(copy)
	, mode(Mode::Stop)
	, currentKeyframe(0)
	, nextKeyframe(0)
	, currentClipName(L"")
	, frameTime(0.0f)
	, time(0.0f)
	, blendStartTime(0.0f)
	, isInit(false)
	, isBlending(false)
	, changeInplace(false)
{
	typeName = L"Animation";
	pass = 1;

	clipIndex = copy.clipIndex;
	map<wstring, ModelClip*>::iterator cIter = copy.clips.begin();

	for (; cIter != copy.clips.end(); cIter++)
	{
		clips[cIter->first] = new ModelClip(*cIter->second);

	}

	isFirstModelName = copy.isFirstModelName;
}

GameAnimator::~GameAnimator()
{
	map<wstring, class ModelClip*>::iterator animIter = clips.begin();

	for (; animIter != clips.end(); animIter++)
		SAFE_DELETE(animIter->second);

	clips.clear();

	SAFE_DELETE_ARRAY(InboneMatrix);
	SAFE_DELETE_ARRAY(InitMatrix);
	SAFE_DELETE_ARRAY(boneTransforms);
	SAFE_DELETE_ARRAY(renderTransforms);
}


void GameAnimator::AddClip(wstring clipFile, bool isRepeat, bool isInplace)
{
	wstring animName = L" " + Path::GetFileName(clipFile);

	clips[animName] = new ModelClip(clipFile, isRepeat, isInplace);


	clips[animName]->ClipIndex() = clipIndex;

	if (!isLoad)
		InstanceManager::Get()->FindAnim(model->GetModelName())->AddClip(clips[animName]);

	clipIndex++;
}

void GameAnimator::Ready()
{
	model = new Model();
	model->ReadMaterial(matFile);
	model->ReadMesh(meshFile);

	for (size_t i = 0; i < loadMaterial.size(); i++)
	{
		if (loadMaterial[i]->GetDiffuseMap() != NULL)
			model->Materials()[i]->SetDiffuseMap(loadMaterial[i]->GetDiffuseMap()->GetFile());

		if (loadMaterial[i]->GetSpecularMap() != NULL)
			model->Materials()[i]->SetSpecularMap(loadMaterial[i]->GetSpecularMap()->GetFile());

		if (loadMaterial[i]->GetNormalMap() != NULL)
			model->Materials()[i]->SetNormalMap(loadMaterial[i]->GetNormalMap()->GetFile());

		model->Materials()[i]->SetDiffuse(loadMaterial[i]->GetDiffuse());
		model->Materials()[i]->SetSpecular(loadMaterial[i]->GetSpecular());
	}

	if (InstanceManager::Get()->FindAnim(model->GetModelName()) != NULL)
	{
		wstring modelName = model->GetModelName();
		SAFE_DELETE(model);
		model = InstanceManager::Get()->FindAnim(modelName)->GetModel();
	}

	//TODO: 나중에 에니메이션케릭터 로드할때 LoadStart함수 처음에 true주고 로드끝나고 false 주기
	if (!isLoad)
		InstanceManager::Get()->AddModel(this, Effects + L"020_AnimInstance.hlsl", InstanceManager::InstanceType::ANIMATION);

	if (!isLoad)
	{
		map<wstring, ModelClip*>::iterator mIter = AnimClips().begin();

		for (; mIter != AnimClips().end(); mIter++)
		{
			InstanceManager::Get()->FindAnim(model->GetModelName())->AddClip(mIter->second);
		}
		//StartTargetClip(AnimClips().begin()->first);
	}

	boneTransforms = new D3DXMATRIX[model->BoneCount()];
	renderTransforms = new D3DXMATRIX[model->BoneCount()];
	InboneMatrix = new D3DXMATRIX[model->BoneCount()];
	InitMatrix = new D3DXMATRIX[model->BoneCount()];

	if (!isFirstModelName)
		isFirstModelName = model->FirstNameModel();

	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone * bone = model->BoneByIndex(i);

		D3DXMATRIX parentTransform;
		D3DXMATRIX global = bone->Global();

		int parentIndex = bone->ParentIndex();
		if (parentIndex < 0)
			D3DXMatrixIdentity(&parentTransform);
		else
			parentTransform = boneTransforms[parentIndex];

		boneTransforms[i] = parentTransform;
		renderTransforms[i] = global * boneTransforms[i];

		InitMatrix[i] = renderTransforms[i];
		D3DXMatrixIdentity(&InboneMatrix[i]);
	}


	Position(LoadPosition());
	Rotation(LoadRotate());
	Scale(LoadScale());

	if (trailEffect == NULL)
		trailEffect = new Effect(Effects + L"021_TrailRender.hlsl");

	for (size_t i = 0; i < trailDesc.size(); i++)
	{
		TrailRenderer*trail = new TrailRenderer(this, trailEffect);
		SAFE_DELETE(trail->GetDiffuseMap());
		trail->GetDiffuseMap() = new Texture(trailDesc[i].diffuseMap);

		SAFE_DELETE(trail->GetAlphaMap());
		trail->GetAlphaMap() = new Texture(trailDesc[i].alphaMap);

		trail->SetBone(trailDesc[i].boneIdx);
		trail->StartPos()._41 = trailDesc[i].startPos.x;
		trail->StartPos()._42 = trailDesc[i].startPos.y;
		trail->StartPos()._43 = trailDesc[i].startPos.z;

		trail->EndPos()._41 = trailDesc[i].endPos.x;
		trail->EndPos()._42 = trailDesc[i].endPos.y;
		trail->EndPos()._43 = trailDesc[i].endPos.z;

		trail->SetLifeTime(trailDesc[i].lifeTime);
		trail->SetRun(true);
		trails.push_back(trail);
	}

	UpdateWorld();
}

void GameAnimator::Update()
{
	__super::Update();


	switch (mode)
	{
		case GameAnimator::Mode::Play:
		{
			if (clipQueue.size() < 1)break;

			InstanceManager::Get()->FindAnim(model->GetModelName())->StartClip(instanceID);
			if (clipQueue.size() < 2)
			{
				AnimationTransforms();
			}
			else
			{
				if (changeInplace)
				{
					Position(inplacePos);
					changeInplace = false;
				}
				BlendTransforms();
			}

			isInit = false;
		}
		break;
		case GameAnimator::Mode::Pause:
		{
			if (clips.size() < 1)break;

			static int preKeyFrame = currentKeyframe;

			if (preKeyFrame != currentKeyframe)
			{
				preKeyFrame = currentKeyframe;
				nextKeyframe = currentKeyframe + 1;
			}
			isInit = false;
			InstanceManager::Get()->FindAnim(model->GetModelName())->UpdateAnimation(instanceID, preKeyFrame, nextKeyframe, frameTime, time);
		}
		break;
		case GameAnimator::Mode::Stop:
		{
			currentKeyframe = 0;
			frameTime = 0;

			memcpy(renderTransforms, InitMatrix, sizeof(D3DXMATRIX)*model->BoneCount());

			InstanceManager::Get()->FindAnim(model->GetModelName())->InitClip(instanceID);
			//버텍스바꾸는코드있어야함 기존의것으로			
		}
		break;
	}


}

void GameAnimator::StartTargetClip(wstring clipName)
{
	if (clipQueue.size() > 2)return;

	//똑같은에니메이션은 보간하지 않을것으로 넣지않는다
	if (clipQueue.size() != 0)
		if (clipQueue.front()->AnimName() == clipName)return;


	if (!isBlending)
	{
		currentClipName = clipName;
		currentKeyframe = clips[currentClipName]->StartFrame();
		frameTime = 0;
		time = 0;

		clipQueue.push(clips[currentClipName]);

		if (clipQueue.size() == 1)
			InstanceManager::Get()->FindAnim(model->GetModelName())->StartClip(instanceID);

		if (clipQueue.size() == 1) // 들어가는 첫클립일경우
			clipQueue.front()->IsAnimEnd() = false;
	}
}

ModelClip * GameAnimator::GetPlayClip()
{
	if (clipQueue.size() < 1)
	{
		return NULL;
	}
	else
	{
		return clipQueue.front();
	}
}


D3DXMATRIX GameAnimator::GetBoneMatrix(int idx)
{
	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 0.01f, 0.01f, 0.01f);
	D3DXMATRIX matrix;


	if (mode == Mode::Stop)
		matrix = renderTransforms[idx] * S;
	else
	{
		ModelBone*bone = model->BoneByIndex(idx);


		matrix = bone->Global()*renderTransforms[idx] * S;
	}


	return matrix;
}

D3DXMATRIX GameAnimator::GetBoneMatrixOutRotate(int idx)
{
	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 0.01f, 0.01f, 0.01f);
	D3DXMATRIX matrix;


	D3DXMATRIX ScaleMat, PositionMat, RotateMat;
	D3DXVECTOR3 s, t, r;
	D3DXQUATERNION q;

	Rotation(&r);


	if (mode == Mode::Stop)
		matrix = S*renderTransforms[idx];
	else
	{
		ModelBone*bone = model->BoneByIndex(idx);

		D3DXMatrixDecompose(&s, &q, &t, &bone->Global());
		D3DXMatrixScaling(&ScaleMat, s.x, s.y, s.z);
		D3DXMatrixRotationYawPitchRoll(&RotateMat, r.y, r.x, r.z);
		D3DXMatrixTranslation(&PositionMat, t.x, t.y, t.z);

		matrix = ScaleMat*PositionMat*renderTransforms[idx] * S*RotateMat;
	}


	return matrix;
}

D3DXMATRIX GameAnimator::GetPlayerBoneWorld(int idx)
{
	D3DXMATRIX S1;
	D3DXMatrixScaling(&S1, 100.0f, 100.0f, 100.0f);
	D3DXMATRIX matrix;

	if (mode == Mode::Stop)
		matrix = S1*renderTransforms[idx] * World();
	else
	{
		ModelBone*bone = model->BoneByIndex(idx);

		matrix = S1*bone->Global()*renderTransforms[idx] * World();
	}

	return matrix;
}

void GameAnimator::InplaceClipUpdate(bool start)
{
	if (!start)return;
	if (!GetPlayClip()->IsInPlace())
	{

		D3DXVECTOR3 transPos =
		{
			GetBoneMatrixOutRotate(1)._41,
			GetBoneMatrixOutRotate(1)._42,
			GetBoneMatrixOutRotate(1)._43
		};

		D3DXVECTOR3 pos;

		Position(&pos);

		pos.x += transPos.x;
		pos.z += transPos.z;

		Position(pos);
	}
}

GData * GameAnimator::Save()
{
	AnimData*data = new AnimData();

	data->modelData.modelName = model->GetModelName();
	data->modelData.type = typeName;
	data->modelData.meshFile = meshFile;
	data->modelData.materialFile = matFile;
	data->modelData.materials.assign(model->Materials().begin(), model->Materials().end());
	data->modelData.min = model->Min();
	data->modelData.max = model->Max();
	data->modelData.colliders.assign(colliders.begin(), colliders.end());
	data->modelData.loadFileData = LoadDataFile();

	Position(&data->modelData.loadPosition);
	Scale(&data->modelData.loadScale);
	Rotation(&data->modelData.loadRotate);

	map<wstring, ModelClip*>::iterator mIter = AnimClips().begin();

	for (; mIter != AnimClips().end(); mIter++)
	{
		data->clips.push_back(mIter->second);
	}

	data->modelData.trails.assign(trails.begin(), trails.end());

	return (GData*)data;
}

void GameAnimator::Load(wstring fileName)
{
	GameModel::Load(fileName);

	Xml::XMLDocument*document = new Xml::XMLDocument();

	wstring tempFile = fileName;
	LoadDataFile() = fileName;
	Xml::XMLError error = document->LoadFile(String::ToString(tempFile).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement*root = document->FirstChildElement();
	Xml::XMLElement*matNode = root->FirstChildElement();

	Xml::XMLElement*node = NULL;

	node = matNode->FirstChildElement();
	wstring bPoint = String::ToWString(node->GetText());
	while (bPoint != L"Clip")
	{
		node = node->NextSiblingElement();
		bPoint = String::ToWString(node->GetText());
	}

	node = node->NextSiblingElement(); // clipSize
	UINT clipSize = node->IntText();

	node = node->NextSiblingElement(); // clips
	for (UINT i = 0; i < clipSize; i++)
	{
		Xml::XMLElement*clipNode = node->FirstChildElement();
		wstring fileName = String::ToWString(clipNode->GetText());

		clipNode = clipNode->NextSiblingElement(); // Speed
		float speed = clipNode->FloatText();

		clipNode = clipNode->NextSiblingElement(); // StartFrame
		float startFrame = clipNode->FloatText();

		clipNode = clipNode->NextSiblingElement(); // EndFrame
		float endFrame = clipNode->FloatText();

		clipNode = clipNode->NextSiblingElement(); // Repeat
		bool repeat = clipNode->BoolText();

		clipNode = clipNode->NextSiblingElement(); // BlendTime
		float blendTime = clipNode->FloatText();

		clipNode = clipNode->NextSiblingElement(); // InPlace
		bool inplace = clipNode->BoolText();

		AddClip(fileName, repeat, inplace);

		wstring animName = L" " + Path::GetFileName(fileName);

		AnimClips()[animName]->Speed() = speed;
		AnimClips()[animName]->StartFrame() = (int)startFrame;
		AnimClips()[animName]->EndFrame() = (int)endFrame;
		AnimClips()[animName]->BlendTime() = blendTime;

		node = node->NextSiblingElement();
	}

	clipIndex = AnimClips().size() + 1;

	mode = Mode::Play;
	SAFE_DELETE(document);
}

void GameAnimator::UpdateWorld()
{
	GameRender::UpdateWorld();

	InstanceManager::Get()->UpdateWorld(this, instanceID, World(), InstanceManager::InstanceType::ANIMATION);

	for (size_t i = 0; i < colliders.size(); i++)
	{
		colliders[i]->Transform(World());
	}
}

void GameAnimator::AnimationTransforms()
{
	ModelClip*clip = clipQueue.front();

	if (clip->ClipIndex() == 0)return;

	frameTime += Time::Delta()*clip->Speed();
	float invFrameRate = 1.0f / clip->FrameRate();
	UpdateTransforms();
	if (frameTime >= invFrameRate)
	{
		//UpdateTransforms();
		//반복 에니메이션일때
		if (clip->IsRepeat())
		{
			if (currentKeyframe < clip->EndFrame())
			{
				currentKeyframe++;
				nextKeyframe = currentKeyframe + 1;
			}
			else // 끝났을때
			{
				currentKeyframe = clip->StartFrame();
				nextKeyframe = currentKeyframe + 1;
			}
			frameTime = 0.0f;
		}
		else
		{
			if (currentKeyframe < clip->EndFrame())
			{
				currentKeyframe++;
				nextKeyframe = currentKeyframe + 1;
			}
			else // 끝났을때
			{
				if (clip->IsAnimEnd() == false)
				{
					if (!GetPlayClip()->IsInPlace())
					{
						D3DXVECTOR3 transPos =
						{
							GetPlayerBoneWorld(1)._41,
							GetPlayerBoneWorld(1)._42,
							GetPlayerBoneWorld(1)._43
						};

						inplacePos = transPos;
						changeInplace = true;
					}
					currentKeyframe = clip->EndFrame();
					nextKeyframe = currentKeyframe;
					clip->IsAnimEnd() = true;
				}
			}
		}
		frameTime = 0.0f;
	}
	time = frameTime / invFrameRate;

	InstanceManager::Get()->FindAnim(model->GetModelName())->UpdateAnimation(instanceID, currentKeyframe, nextKeyframe, frameTime, time);
}

void GameAnimator::BlendTransforms()
{
	//큐에 에니메이션이 두개가 들어오면 보간한다음 보간끝나면 큐에서 빼준다
	//기존에니메이션->들어온 에니메이션
	blendStartTime += Time::Delta()*clipQueue.front()->BlendTime();
	UINT blendClipIdx;
	if (blendStartTime <= 1)
	{
		isBlending = true;
		// 새로들어온 프레임의 인덱스번호를 next에넣어준다

		UINT boneNum;

		if (!isFirstModelName) boneNum = 0;
		else boneNum = 1;

		if (useBoneTransforms)
		{
			for (UINT i = boneNum; i < model->BoneCount(); i++)
			{
				ModelBone*bone = model->BoneByIndex(i);

				ModelKeyFrame*curClipFrame;
				ModelKeyFrame*nextClipFrame;


				if (!isFirstModelName)
				{
					curClipFrame = clipQueue.front()->Keyframe(bone->Index());
					nextClipFrame = clipQueue.back()->Keyframe(bone->Index());
				}
				else
				{
					curClipFrame = clipQueue.front()->Keyframe(bone->Index() - 1);
					nextClipFrame = clipQueue.back()->Keyframe(bone->Index() - 1);
				}


				if (curClipFrame == NULL || nextClipFrame == NULL)
				{
					continue;
				}

				D3DXMATRIX transform;
				D3DXMATRIX parentTransform;

				D3DXMATRIX invGlobal = bone->Global();
				D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

				D3DXMATRIX S, R, T;

				ModelKeyFrameData current = curClipFrame->Transforms[currentKeyframe];
				ModelKeyFrameData next = nextClipFrame->Transforms[clipQueue.back()->StartFrame()];


				D3DXVECTOR3 s1 = current.Scale;
				D3DXVECTOR3 s2 = next.Scale;

				D3DXVECTOR3 s;
				D3DXVec3Lerp(&s, &s1, &s2, blendStartTime);
				D3DXMatrixScaling(&S, s.x, s.y, s.z);


				D3DXQUATERNION q1 = current.Rotation;
				D3DXQUATERNION q2 = next.Rotation;

				D3DXQUATERNION q;
				D3DXQuaternionSlerp(&q, &q1, &q2, blendStartTime);
				D3DXMatrixRotationQuaternion(&R, &q);


				D3DXVECTOR3 t1 = current.Translation;
				D3DXVECTOR3 t2 = next.Translation;

				D3DXVECTOR3 t;
				D3DXVec3Lerp(&t, &t1, &t2, blendStartTime);
				D3DXMatrixTranslation(&T, t.x, t.y, t.z);

				transform = S * R * T;

				int parentIndex = bone->ParentIndex();
				if (parentIndex < 0)
					D3DXMatrixIdentity(&parentTransform);
				else
					parentTransform = boneTransforms[parentIndex];

				boneTransforms[i] = transform*parentTransform* InboneMatrix[i];
				renderTransforms[i] = invGlobal*boneTransforms[i];
			}
		}
		nextKeyframe = clipQueue.back()->StartFrame();
		blendClipIdx = clipQueue.back()->ClipIndex();
	}
	else
	{
		//1초넘어가면 큐에서빼고 바꿔준다
		clipQueue.front()->IsAnimEnd() = false;
		clipQueue.pop();
		currentKeyframe = clipQueue.front()->StartFrame();
		isBlending = false;
		blendClipIdx = 0;
		blendStartTime = 0;
		InstanceManager::Get()->FindAnim(model->GetModelName())->StartClip(instanceID);
	}
	InstanceManager::Get()->FindAnim(model->GetModelName())->UpdateBlending(instanceID, currentKeyframe, nextKeyframe, blendStartTime, blendClipIdx);
}

void GameAnimator::UpdateTransforms()
{
	if (!useBoneTransforms)return;

	UINT boneNum;

	if (!isFirstModelName) boneNum = 0;
	else boneNum = 1;

	for (UINT i = boneNum; i < model->BoneCount(); i++)
	{
		ModelBone*bone = model->BoneByIndex(i);

		int a = bone->Index();

		ModelKeyFrame*frame;

		frame = clipQueue.front()->Keyframe(bone->Index());
		if (!isFirstModelName)
			frame = clipQueue.front()->Keyframe(bone->Index());
		else
			frame = clipQueue.front()->Keyframe(bone->Index() - 1);

		if (frame == NULL)
		{
			continue;
		}

		D3DXMATRIX transform;
		D3DXMATRIX parentTransform;

		D3DXMATRIX invGlobal = bone->Global();
		D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

		D3DXMATRIX S, R, T;

		ModelKeyFrameData current = frame->Transforms[currentKeyframe];
		ModelKeyFrameData next = frame->Transforms[nextKeyframe];

		D3DXVECTOR3 s1 = current.Scale;
		D3DXVECTOR3 s2 = next.Scale;

		D3DXVECTOR3 s;
		D3DXVec3Lerp(&s, &s1, &s2, frameTime);
		D3DXMatrixScaling(&S, s.x, s.y, s.z);


		D3DXQUATERNION q1 = current.Rotation;
		D3DXQUATERNION q2 = next.Rotation;

		D3DXQUATERNION q;
		D3DXQuaternionSlerp(&q, &q1, &q2, frameTime);
		D3DXMatrixRotationQuaternion(&R, &q);


		D3DXVECTOR3 t1 = current.Translation;
		D3DXVECTOR3 t2 = next.Translation;

		D3DXVECTOR3 t;
		D3DXVec3Lerp(&t, &t1, &t2, frameTime);
		D3DXMatrixTranslation(&T, t.x, t.y, t.z);

		transform = S * R * T;


		int parentIndex = bone->ParentIndex();
		if (parentIndex < 0)
			D3DXMatrixIdentity(&parentTransform);
		else
			parentTransform = boneTransforms[parentIndex];

		boneTransforms[i] = transform*parentTransform* InboneMatrix[i];
		renderTransforms[i] = invGlobal*boneTransforms[i];
	}
}

void GameAnimator::SetBoneMatrix(D3DXMATRIX mat, int index)
{
	InboneMatrix[index] = mat;
}


