#pragma once

#include "Systems\IExecute.h"

class Main : public IExecute
{
public:
	void Initialize() override; // ��ü ����
	void Ready() override; // ���� ���� ������Ʈ ���� ó�� ��
	void Destroy() override; // �ı� �Ҹ��ڿ��� ȣ��
	void Update() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void ResizeScreen() override; // �׳� D3D�� �°� �Ҳ�

private:
	void Push(IExecute* execute);

private:
	vector<IExecute*> executes;
};