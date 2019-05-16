#include "Framework.h"
#include "ProgressBar.h"

ProgressBar::ProgressBar()
	:textures(NULL),render2d(NULL),
	frameIdx(0),progressVal(0)
{
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::Init()
{
	frameIdx = 0;
	progressVal = 0;
}

void ProgressBar::Ready()
{
	float width = Context::Get()->GetViewport()->GetWidth();
	float height = Context::Get()->GetViewport()->GetHeight();

	vector<wstring> names;

	for (int i = 0; i < 60; i++)
		names.push_back(Textures + L"GameScene/loadingImage/layer (" + to_wstring(i + 1) + L").png");

	textures = new TextureArray(names);
	render2d = new Render2D(L"022_2Danimation.hlsl");
	render2d->SRV(textures->GetSRV());
	render2d->Position(0, 0);
	render2d->Scale(width, height);
	frameIdx = 0;
	render2d->GetEffect()->AsScalar("frameIdx")->SetInt(frameIdx);
}

void ProgressBar::Update()
{
	if (frameIdx >= 60)
		frameIdx = 0;

	static float time = 0;
	static float checkTime = 0;
	time += Time::Get()->Delta();
	checkTime += Time::Get()->Delta();

	if (time > 0.03f)
	{
		frameIdx++;
		time = 0;
	}
	render2d->GetEffect()->AsScalar("frameIdx")->SetInt(frameIdx);
}

void ProgressBar::Render()
{
	render2d->Render();

	ImGui::ProgressBar(progressVal);
}
