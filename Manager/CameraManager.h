#pragma once
#include "../Viewer/Fixity.h"
#include "../Viewer/Freedom.h"
#include "../Viewer/Orbit.h"
#include "../Viewer/Follow.h"
#include "../Viewer/TPSCamera.h"

class CameraManager
{
public:
	enum class CAMERASTATE
	{
		Freedom,
		Fixity,
		Orbit,
		Follow,
		TPS
	};
public:
	static CameraManager*GetInstance();

	void Init(ExecuteValues*values);
	void Update();
	void Render();
	void ImGuiRender();

	//설정값들 셋팅되고 버튼누르면 되는걸로
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
	class GameModel*model;

	class Frustum*frustum;
	//Follow카메라에 들어온 모델이름
	wstring modelName;

	vector<Camera*> cams;
	ExecuteValues*values;

	//누르기전 설정값들
	CAMERASTATE prevState;
	D3DXVECTOR3 position;

	//TPS
	D3DXVECTOR3*TpsPos,*TpsRotate;

	//Follow
	D3DXVECTOR3*FstartPos, *Frotate;

	//Orbit
	D3DXVECTOR3 OstartPos;


};
