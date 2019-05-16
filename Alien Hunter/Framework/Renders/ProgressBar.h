#pragma once

class ProgressBar
{
public:
	ProgressBar();
	~ProgressBar();

	void Init();
	void Ready();
	void Update();
	void Render();

	float&GetProgressVal() { return progressVal; }
private:
	TextureArray*textures;
	Render2D*render2d;

	int frameIdx;
	float progressVal;
};