#pragma once

//있는 기능을 더 추가하여 좀더
//편하게 사용하는 클래스
//래퍼클래스

//레스터라이즈는 프로젝션 변환까지 끝나고 VS처리 끝나고 
//PS처리 들어가기전에 처리
class RasterizerState
{
public:
	RasterizerState();
	~RasterizerState();

	void RSSetState();

	//안티엘리어싱을 켤지 안켤지
	void AntialiasedLineEnable(bool val);
	
	//컬링을 할지 컬링을 시계방향으로 할지 반시계방향으로 할지
	//컬링은 버텍스쉐이더 처리바로 이전에 일어남(버텍스)
	void CullMode(D3D11_CULL_MODE val);
	
	//클리핑은 픽셀쉐이더 처리바로 이전에 일어남(픽셀)

	void DepthClipEnable(bool val);
	
	void FillMode(D3D11_FILL_MODE val);
	
	//그릴 방향을 정하는것=>시계 방향,반시계 방향
	void FrontCounterClockwise(bool val);
	
	//화면에 보이는 퀄리티를 높일수있지만 속도느려짐
	void MultisampleEnable(bool val);
private:
	void Changed();

private:
	D3D11_RASTERIZER_DESC desc;
	ID3D11RasterizerState* state;
};
