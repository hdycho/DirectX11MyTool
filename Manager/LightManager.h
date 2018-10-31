#pragma once

class LightManager
{
public:
	enum class SHADOWTYPE
	{
		NONE,
		DEPTH_SHADOW
	};
public:
	void Init(ExecuteValues*values);

	static LightManager*GetInstance();

	void Update();
	void Render();
	void ImGuiRender();
	void PreRender();

	class Shadow*GetShadow() { return shadow; }
	bool UseShadow();
private:
	static LightManager*instance;

	class Shadow*shadow;
	SHADOWTYPE sType;

	ExecuteValues*values;
};

