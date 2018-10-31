#pragma once

class ModelTweener
{
public:
	ModelTweener();
	~ModelTweener();

	void Play(class ModelClip*clip,
		bool bRepeat = false,
		float blendTime = 0,
		float speed = 1.0f,
		float startTime = 0.0f);

	void Play(class ModelClip*clip,
		bool bRepeat,
		float blendTime = 0,
		int startTime = 0.0f);

	void UpdateBlending(class ModelBone*bone, float time);

	bool&IsPlay() { return isPlay; }
	
private:
	class ModelClip*current;
	class ModelClip*next;

	float blendTime;
	float elapsedTime;		//���� �����Ӻ��� ���������ӱ��� �ð�

	bool isPlay;


};