#pragma once
#include "BillboardObject.h"

// 爆発エフェクト
class Explosion :
	public BillboardObject
{

public:
	void Update() override;

	bool IsFinished()
	{
		return mFinished;
	}

private:
	int mTime;

	bool mFinished = false;

};

