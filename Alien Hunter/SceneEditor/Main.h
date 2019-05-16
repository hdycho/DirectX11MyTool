#pragma once

#include "Systems\IExecute.h"

class Main : public IExecute
{
public:
	void Initialize() override; // 객체 생성
	void Ready() override; // 버퍼 생성 업데이트 직전 처음 콜
	void Destroy() override; // 파괴 소멸자에서 호출
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void ResizeScreen() override; // 그냥 D3D에 맞게 할꺼

private:
	void Push(IExecute* execute);

private:
	vector<IExecute*> executes;
};