#include "Framework.h"
#include "ParticleSystem.h"
#include "Collider\ColliderSphere.h"

#define NON 2000
ParticleSystem::ParticleSystem(wstring textureName)
{
	texture = NULL;
	vertexBuffer = NULL;
	targetPos = { NON,NON,NON };
	emitVector = { NON,NON,NON };
	vertices.clear();
	isEnd = true;

	textureNames.push_back(Textures+textureName);
}

ParticleSystem::ParticleSystem(vector<wstring>& textureName, float frameSpeed, float isAlpha)
{
	texture = NULL;
	vertexBuffer = NULL;
	targetPos = { NON,NON,NON };
	emitVector = { NON,NON,NON };
	vertices.clear();
	isEnd = true;
	this->isAlpha = isAlpha;

	this->frameSpeed = frameSpeed;
	textureNames.assign(textureName.begin(), textureName.end());

}

ParticleSystem::~ParticleSystem()
{
	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(particleMaterial);

	SAFE_DELETE(texture);
	SAFE_DELETE_ARRAY(particleDesc);
	SAFE_DELETE_ARRAY(pInfo);
}

void ParticleSystem::Initialize(float vel, float velVar, D3DXVECTOR2 pSize, float pPerNum, float lifeTime,float startLange, float maxPaticle)
{
	this->startRange = startLange;
	pType = ParticleType::Cylinder;
	pOption = ParticleOption::Repeat;

	// 파티클의 속도와 속도 변화를 설정합니다.
	m_particleVelocity = vel;

	m_particleVelocityVariation = velVar;

	// 파티클의 물리적 크기를 설정합니다.
	m_particleSize = pSize;

	// 초당 방출 할 파티클 수를 설정합니다.
	m_particlesPerSecond = pPerNum;

	// 파티클 시스템에 허용되는 최대 파티클 수를 설정합니다.
	m_maxParticles = maxPaticle;

	particleDesc = new InstanceParticle[1360];
	pInfo = new ParticleInfo[1360];

	for (int i = 0; i < 1360; i++)
	{
		Vertex particle;

		particle.Position = { 0,0,0 };

		vertices.push_back(particle);

		particleDesc[i].IsActive = false;
		particleDesc[i].Velocity = 0;
		particleDesc[i].alphaValue = 1;
		particleDesc[i].Pos = { 0,0,0 };
		particleDesc[i].animIdx = 0;
		
		pInfo[i].angle = i;
		pInfo[i].lifeTime = 0;
		pInfo[i].frameTime = 0;
		pInfo[i].isShot = false;
		pInfo[i].curGravity = 0;
		
		particleDesc[i].Size={ m_particleSize.x,m_particleSize.y };
	}

	// 아직 배출되지 않으므로 현재 파티클 수를 0으로 초기화합니다.
	m_currentParticleCount = 0;

	// 초당 파티클 방출 속도의 초기 누적 시간을 지웁니다.
	m_accumulatedTime = 0.0f;

	m_lifeTime = lifeTime;

	gravityValue = 0;
}

void ParticleSystem::Ready()
{
	particleMaterial = new Effect(Effects + L"025_Particle.hlsl");
	
	texture = new TextureArray(textureNames);

	particleMaterial->AsSRV("tex")->SetResource(texture->GetSRV());

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(Vertex);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	descVal = particleMaterial->Variable("particles");
	descVal->SetRawValue(particleDesc, 0, sizeof(InstanceParticle)*1360);
}

void ParticleSystem::SetTargetPos(D3DXVECTOR3 pos)
{
	targetPos = pos;
}

void ParticleSystem::SetTargetVec(D3DXVECTOR3 dir)
{
	emitVector = dir;
}

void ParticleSystem::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	particleMaterial->Draw(0, 0, vertices.size());
}


void ParticleSystem::ResetParticle()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		pInfo[i].isShot = false;
		pInfo[i].curGravity = 0;
		pInfo[i].lifeTime = 0;
		pInfo[i].frameTime = 0;
		particleDesc[i].animIdx = 0;
	}
	isEnd = false;
}

void ParticleSystem::ImGuiRender()
{
	ImGui::Begin("Particle");

	ImGui::Text("Velocity");
	ImGui::SliderFloat("##velocity", &m_particleVelocity, 0, 100);
	
	ImGui::Text("VelocityVariable");
	ImGui::SliderFloat("##velocityvv", &m_particleVelocityVariation, 0, 20);

	ImGui::Text("Gravity");
	ImGui::DragInt("##Gravity", &gravityValue,0.1,-50, 50);

	ImGui::Separator();

	ImGui::Text("Size");
	ImGui::SliderFloat2("##Size", m_particleSize, 0, 5);

	ImGui::Text("ParticleCount");
	ImGui::DragInt("##ParticleCount", &m_maxParticles, 1,0, 1360);

	ImGui::Text("EmitPerSecont");
	ImGui::SliderFloat("##EmitPerSecont", &m_particlesPerSecond, 0, 3000);

	ImGui::Text("LifeTime");
	ImGui::SliderFloat("##LifeTime", &m_lifeTime, 0, 5);

	ImGui::Text("StartRange");
	ImGui::SliderFloat("##StartRange", &startRange, 0, 10);

	ImGui::Text("AnimFrameSpeed");
	ImGui::SliderFloat("##AnimFrameSpeed", &frameSpeed, 0, 60);

	ImGui::Separator();
	
	if (ImGui::TreeNode("Texture"))
	{
		if (texture->GetSRV() != NULL)
		{
			if (ImGui::ImageButton(texture->GetSRV(), ImVec2(100, 100)))
			{
				Path::OpenFileDialog(L"파일", L".png", Textures, bind(&ParticleSystem::ChangeTexture, this, placeholders::_1));
			}
		}
		ImGui::TreePop();
	}

	ImGui::Separator();

	static int num = pType;

	ImGui::Text("ParticleType");
	ImGui::RadioButton("None",&num,0);
	ImGui::RadioButton("Cluster", &num, 1);
	ImGui::RadioButton("Cone", &num, 2);
	ImGui::RadioButton("Cylinder", &num, 3);
	pType = (ParticleType)num;

	static int num2 = pOption;
	ImGui::Text("ParticleOption");
	ImGui::RadioButton("Once", &num2, 0);
	ImGui::RadioButton("Repeat", &num2, 1);
	pOption = ParticleOption(num2);

	ImGui::Separator();
	


	ImGui::End();
}


void ParticleSystem::EmitParticles()
{
	//// 프레임 시간을 증가시킵니다.
	m_accumulatedTime += Time::Get()->Delta();

	// 이제 파티클 출력을 false로 설정합니다.
	bool emitParticle = false;

	// 새 파티클을 방출할 시간인지 확인합니다.
	if (m_accumulatedTime > (1 / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	D3DXVECTOR3 tPos;

	if (targetPos.x != NON)
		tPos = targetPos;
	else
		tPos = { 0,0,0 };

	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);

	m._41 = tPos.x;
	m._42 = tPos.y;
	m._43 = tPos.z;
	

	if (emitParticle)
	{
		switch (pType)
		{
			case ParticleSystem::ParticleType::None:
			{
				if (m_currentParticleCount >= m_maxParticles)
					m_currentParticleCount = 0;

				if (particleDesc[m_currentParticleCount].IsActive)return;

				float positionX = tPos.x + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				float positionY = tPos.y + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				float positionZ = tPos.z + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				particleDesc[m_currentParticleCount].Pos = { positionX,positionY,positionZ };

				SortParticle(m_currentParticleCount);

				pInfo[m_currentParticleCount].lifeTime = 0;

				float sizeX = pInfo[m_currentParticleCount].lifeTime*m_particleSize.x;
				float sizeY = pInfo[m_currentParticleCount].lifeTime*m_particleSize.y;
				particleDesc[m_currentParticleCount].Size = { sizeX,sizeY };

				particleDesc[m_currentParticleCount].alphaValue = 1.0f;
				particleDesc[m_currentParticleCount].Velocity = 0;
				particleDesc[m_currentParticleCount].IsActive = true;

				if (pOption == Once&&pInfo[m_currentParticleCount].isShot)
				{
					particleDesc[m_currentParticleCount].IsActive = false;
				}

				pInfo[m_currentParticleCount].isShot = true;

				m_currentParticleCount++;
			}
			break;
			case ParticleSystem::ParticleType::Cluster:
			{
				if (m_currentParticleCount >= m_maxParticles)
					m_currentParticleCount = 0;

				if (particleDesc[m_currentParticleCount].IsActive)return;

				float positionX = tPos.x + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				float positionY = tPos.y + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				float positionZ = tPos.z + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				particleDesc[m_currentParticleCount].Pos = { positionX,positionY,positionZ };

				SortParticle(m_currentParticleCount);

				pInfo[m_currentParticleCount].lifeTime = 0;

				float sizeX = pInfo[m_currentParticleCount].lifeTime*m_particleSize.x;
				float sizeY = pInfo[m_currentParticleCount].lifeTime*m_particleSize.y;
				particleDesc[m_currentParticleCount].Size = { sizeX,sizeY };

				float velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

				if (velocity < 0)
					velocity *= -1;

				particleDesc[m_currentParticleCount].alphaValue = 1.0f;
				particleDesc[m_currentParticleCount].Velocity = velocity;
				particleDesc[m_currentParticleCount].IsActive = true;

				if (pOption == Once&&pInfo[m_currentParticleCount].isShot)
				{
					particleDesc[m_currentParticleCount].IsActive = false;
				}

				pInfo[m_currentParticleCount].isShot = true;

				m_currentParticleCount++;
			}
			break;
			case ParticleSystem::ParticleType::Cone:
			{
				int count = 0;
				for (int i = 0; i < m_maxParticles; i++)
				{
					if (count > 360)break;
					if (particleDesc[i].IsActive)continue;


					float positionX = tPos.x + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
					float positionY = tPos.y + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
					float positionZ = tPos.z + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
					particleDesc[i].Pos = { positionX,positionY,positionZ };
					
					//SortParticle(m_currentParticleCount);
					
					pInfo[i].lifeTime = 0;
					float sizeX = pInfo[i].lifeTime*m_particleSize.x;
					float sizeY = pInfo[i].lifeTime*m_particleSize.y;
					particleDesc[i].Size = { sizeX,sizeY };

					float velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

					if (velocity < 0)
						velocity *= -1;

					particleDesc[i].alphaValue = 1.0f;
					particleDesc[i].Velocity = velocity;
					particleDesc[i].IsActive = true;

					count++;
				}
			}
			break;
			case ParticleSystem::ParticleType::Cylinder:
			{
				if (m_currentParticleCount >= m_maxParticles)
					m_currentParticleCount = 0;

				if (particleDesc[m_currentParticleCount].IsActive)return;

				float positionX = tPos.x + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				float positionY = tPos.y + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				float positionZ = tPos.z + (((float)rand() - (float)rand()) / RAND_MAX) * startRange;
				particleDesc[m_currentParticleCount].Pos = { positionX,positionY,positionZ };

				SortParticle(m_currentParticleCount);

				pInfo[m_currentParticleCount].lifeTime = 0;

				float sizeX = pInfo[m_currentParticleCount].lifeTime*m_particleSize.x;
				float sizeY = pInfo[m_currentParticleCount].lifeTime*m_particleSize.y;
				particleDesc[m_currentParticleCount].Size = { sizeX,sizeY };

				float velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

				if (velocity < 0)
					velocity *= -1;

				particleDesc[m_currentParticleCount].alphaValue = 1.0f;
				particleDesc[m_currentParticleCount].Velocity = velocity;
				particleDesc[m_currentParticleCount].IsActive = true;

				if (pOption == Once&&pInfo[m_currentParticleCount].isShot)
				{
					particleDesc[m_currentParticleCount].IsActive = false;
				}

				pInfo[m_currentParticleCount].isShot = true;

				m_currentParticleCount++;
			}
			break;
		}
	}
}

void ParticleSystem::KillParticle()
{
	D3DXVECTOR3 tPos;

	if (emitVector.x != NON)
		tPos = emitVector;
	else
		tPos = { 0,0,1 };

	// 특정 높이 범위를 벗어난 모든 파티클을 제거합니다.
	for (int i = 0; i<m_maxParticles; i++)
	{
		if (!particleDesc[i].IsActive)continue;
		
		pInfo[i].lifeTime += Time::Get()->Delta();

		float sizeX = (pInfo[i].lifeTime*m_particleSize.x) / m_lifeTime;
		float sizeY = (pInfo[i].lifeTime*m_particleSize.y)/ m_lifeTime;

		if (sizeX > m_particleSize.x)
			sizeX = m_particleSize.x;

		if (sizeY > m_particleSize.y)
			sizeY = m_particleSize.y;

		particleDesc[i].Size = { sizeX ,sizeY };

		if (isAlpha)
			particleDesc[i].alphaValue = 1.0f - (pInfo[i].lifeTime / m_lifeTime);
		else
			particleDesc[i].alphaValue = 0.7f;

		pInfo[i].curGravity += pInfo[i].lifeTime*gravityValue*0.0001f;

		if (pType == Cluster)
		{
			tPos.x += Math::Random(-0.5f, 0.5f);
			tPos.y += Math::Random(-0.5f, 0.5f);
			tPos.z += Math::Random(-0.5f, 0.5f);

			D3DXVec3Normalize(&tPos, &tPos);
		}

		UpdatePaticleType(particleDesc[i].Pos, tPos, particleDesc[i].Velocity, i, pInfo[i].curGravity);


		pInfo[i].frameTime += Time::Get()->Delta()*frameSpeed;
		if (pInfo[i].frameTime > 1)
		{
			pInfo[i].frameTime = 0;
			particleDesc[i].animIdx++;
		}

		if (particleDesc[i].animIdx >= textureNames.size())
			particleDesc[i].animIdx = 0;


		if (pInfo[i].lifeTime > m_lifeTime)
		{
			particleDesc[i].IsActive = false;

			if (pOption == Repeat)
			{
				pInfo[i].isShot = false;
				if (i == m_maxParticles - 1)
					isEnd = true;
			}
			
			pInfo[i].curGravity = 0;
			pInfo[i].lifeTime = 0;
		}
	}

	descVal->SetRawValue(particleDesc, 0, sizeof(InstanceParticle)*1360);
}

void ParticleSystem::SortParticle(int curIdx)
{
	for (int i = 0; i < curIdx-1; i++)
	{
		if (!particleDesc[i].IsActive)continue;

		for (int j = 0; j < curIdx - 1 - i; j++)
		{
			D3DXVECTOR3 pos;
			Context::Get()->GetMainCamera()->Position(&pos);

			float dis1 = Math::Distance(particleDesc[j].Pos, pos);
			float dis2= Math::Distance(particleDesc[j+1].Pos, pos);

			if (dis1 < dis2)
			{
				InstanceParticle temp = particleDesc[j];
				particleDesc[j] = particleDesc[j + 1];
				particleDesc[j + 1] = temp;

				ParticleInfo temp2 = pInfo[j];
				pInfo[j] = pInfo[j + 1];
				pInfo[j + 1] = temp2;
			}
		}
	}
}

void ParticleSystem::ChangeTexture(wstring name)
{
	if (texture != NULL)
		SAFE_DELETE(texture);

	textureNames.clear();
	
	textureNames.push_back(name);

	texture = new TextureArray(textureNames);

	particleMaterial->AsSRV("tex")->SetResource(texture->GetSRV());
}


void ParticleSystem::UpdatePaticleType(D3DXVECTOR3 & pos,D3DXVECTOR3 tvec, float vel, int idx,float gravity)
{
	switch (pType)
	{
		case ParticleSystem::ParticleType::Cluster:
		{
			pos += Time::Get()->Delta() * vel*tvec;
			
			pos.y -= gravity;
		}
		break;
		case ParticleSystem::ParticleType::Cone:
		{
			D3DXVECTOR3 d = tvec;

			d.x = 5 * sinf(Math::ToRadian(3))*cosf(Math::ToRadian(pInfo[idx].angle)) + pos.x;
			d.y = 5 * sinf(Math::ToRadian(3))*sinf(Math::ToRadian(pInfo[idx].angle)) + pos.y;

			D3DXVECTOR3 vec;
			
			vec.x = (d.x - pos.x);
			vec.y = (d.y - pos.y);
			vec.z = 1*tvec.z;


			pos += Time::Get()->Delta() * vel*vec;

			pos.y -= gravity;
		}
		break;
		case ParticleSystem::ParticleType::Cylinder:
		{
			pos+= Time::Get()->Delta() * vel*tvec;

			pos.y -= gravity;
		}
		break;
	}
}
