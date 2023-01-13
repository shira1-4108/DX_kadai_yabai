#include "Explosion.h"

extern DWORD gDeltaTime;

void Explosion::Update()
{
	if (mFinished)
		return;

	const float speed = 0.009f;
	const int verticalFrame = 4;
	const int horizontalFrame = 4;
	const int maxFrame = verticalFrame * horizontalFrame;
	const float u = 1.0f / verticalFrame;
	const float v = 1.0f / horizontalFrame;

	mTime += gDeltaTime;

	int time = static_cast<int>(mTime * speed);

	if (time >= maxFrame)
	{
		//time = mTime = 0;
		mFinished = true;
	}

	Model* pModel = GetModel();
	pModel->mUvOffset.x = static_cast<float>(time % horizontalFrame) * u;
	pModel->mUvOffset.y = static_cast<float>(time / horizontalFrame) * v;

	BillboardObject::Update();
}
