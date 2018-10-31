#pragma once
#include "../Viewer/Fixity.h"
#include "../Viewer/Freedom.h"
#include "../Viewer/Orbit.h"
#include "../Viewer/Follow.h"

class CameraManager
{
public:
	enum class CAMERASTATE
	{
		Freedom,
		Fixity,
		Orbit,
		Follow
	};
public:
	static CameraManager*GetInstance();

	void Init(ExecuteValues*values);
	void Update();
	void Render();
	void ImGuiRender();

	//�������� ���õǰ� ��ư������ �Ǵ°ɷ�
	void SelectCamera(CAMERASTATE cState);

	void InputCameraPtr(class Frustum*frustum)
	{
		this->frustum = frustum;
	}
private:
	static CameraManager*instance;
	CAMERASTATE cState;
	vector<wstring> stateNames;
	wstring selectStateName;
	GameModel*model;

	class Frustum*frustum;
	//Followī�޶� ���� ���̸�
	wstring modelName;

	vector<Camera*> cams;
	ExecuteValues*values;

	//�������� ��������
	CAMERASTATE prevState;
	D3DXVECTOR3 position;

	//Follow
	D3DXVECTOR3*FstartPos, *Frotate;

	//Orbit
	D3DXVECTOR3 OstartPos;


};
