#pragma once

class ParticleSystem
{
public:
	enum ParticleType
	{
		None,		//	제자리에서 흩뿌림
		Cluster,	//	불규칙 흩뿌림
		Cone,		//	콘모양으로 흩뿌림
		Cylinder	//	실린더 모양으로 흩뿌림
	}pType;

	enum ParticleOption
	{
		Once,		//	한번만 순회/랜더
		Repeat		//	반복 랜더
	}pOption;

public:
	ParticleSystem(wstring textureName);
	ParticleSystem(vector<wstring>&textureName, float frameSpeed, float isAlpha=true);
	~ParticleSystem();

	void Initialize(float vel, float velVar, D3DXVECTOR2 size, float pPerNum, float lifeTime, float startLange = 0.5f, float maxPaticle = 1360);
	void Ready();
	void Render();

	void ResetParticle();			// Particle 초기화
	void EmitParticles();			// Particle 소비자 
	void KillParticle();			// Particle 생산자
	void SortParticle(int curIdx);	// Particle 정렬 : Blend Render문제 해결

	void ChangeTexture(wstring name); 
	void ImGuiRender();
	//====================Get/Set=======================//
	int&Gravity() { return gravityValue; }
	float&Velocity() { return m_particleVelocity; }
	float&VelocityVariation() { return m_particleVelocityVariation; }
	D3DXVECTOR2&Size() { return m_particleSize; }
	float&EmitPerSecont() { return m_particlesPerSecond; }
	float&StartRange() { return startRange; }
	float&LifeTime() { return m_lifeTime; }
	float&AnimFrameSpeed() { return frameSpeed; }
	ParticleType&GParticleType() { return pType; }
	ParticleOption&GParticleOption() { return pOption; }
	bool&IsEnd() { return isEnd; }
	//==================================================//

	void SetTargetPos(D3DXVECTOR3 pos);
	void SetTargetVec(D3DXVECTOR3 dir);
private:
	void UpdatePaticleType(D3DXVECTOR3&pos, D3DXVECTOR3 tvec, float vel, int idx, float gravity);

private:
	struct InstanceParticle
	{
		D3DXVECTOR3 Pos;
		float alphaValue;
		//16
		int IsActive;
		float Velocity;
		D3DXVECTOR2 Size;
		//16
		int animIdx;
		D3DXVECTOR3 padding;
	};

	struct ParticleInfo
	{
		float frameTime;
		float angle;
		float lifeTime;
		bool isShot;
		float curGravity;
	};
	ParticleInfo*pInfo;
	
private:
	vector<Vertex> vertices;
	InstanceParticle*particleDesc;

	ID3D11Buffer*vertexBuffer;

	UINT particleCount;

	TextureArray*texture;
	
	Effect*particleMaterial;
	vector<wstring> textureNames;

	int gravityValue = 0;
	float frameSpeed = 0;
	float startRange = 0;
	float m_particleVelocity = 0;
	float m_particleVelocityVariation = 0;
	D3DXVECTOR2 m_particleSize = {0 ,0};
	float m_particlesPerSecond = 0;
	int m_maxParticles = 0;
	float m_lifeTime;

	int m_currentParticleCount = 0;
	float m_accumulatedTime = 0;

	D3DXVECTOR3 targetPos;
	D3DXVECTOR3 emitVector;

	ID3DX11EffectVariable*descVal;

	bool isEnd = true;
	bool isAlpha = true;
};